/*
 * Copyright (c) 2011-2014 zhangjianlin 张荐林
 * eamil:jonas88@sina.com
 * addr: china
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "../inc/iop_config.h"
#include "../inc/iop_select.h"

#ifndef _HAVE_SELECT_
int iop_init_select(void *iop_base, int maxev)
{
    return -1;
}
#else


/////START...
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#ifndef WIN32
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "../inc/iop.h"


struct tag_iop_select_data {
    fd_set rset_out;
    fd_set wset_out;

    fd_set rset;
    fd_set wset;
    int maxfd;
};
typedef struct tag_iop_select_data iop_select_data_t;



static void select_free(iop_base_t *base)
{
    iop_select_data_t *p = (iop_select_data_t *)(base->model_data);
    if(p)
    {
        free(p);
    }
    base->model_data = (void *)0;
}

static int select_dispatch(iop_base_t * base, int timeout)
{
    int tv_sec = timeout/1000;
    int tv_usec = (timeout*1000)%1000000;
    struct timeval tv = {tv_sec, tv_usec};
    int n = 0;
    int num = 0;
    iop_t *iop = 0;
	int cur_id = 0;
	int next_id = 0;
	unsigned int revents = 0;

	iop_select_data_t *model_data = (iop_select_data_t *)(base->model_data);
    memcpy(&(model_data->rset_out), &(model_data->rset), sizeof(model_data->rset));
    memcpy(&(model_data->wset_out), &(model_data->wset), sizeof(model_data->wset));


#ifdef WIN32
        n = select(0, &(model_data->rset_out),&(model_data->wset_out),NULL,&tv);
        if(n < 0)
        {
            if(WSAEINVAL == socket_errno)
            {
                Sleep(timeout);
                n = 0;
            }
        }
#else
    do{
        n = select(model_data->maxfd+1, &(model_data->rset_out),&(model_data->wset_out),NULL,&tv);
     }while((n < 0) && (errno == EINTR));
#endif
    base->cur_time = time(NULL);
    if(n <= 0)
    {
        return n;
    }

	cur_id = base->io_list_head;
	while(cur_id != -1 && num < n)
	{
		iop = base->iops+cur_id;
		next_id = iop->next;
        revents = 0;
        if (FD_ISSET(iop->handle, &(model_data->rset_out)))
        {
            revents = EV_TYPE_READ;
        }
        if (FD_ISSET(iop->handle, &(model_data->wset_out)))
        {
            revents |= EV_TYPE_WRITE;
        }
        if(revents != 0)
        {
            ++num;
            IOP_CB(base,iop,revents);
        }
		cur_id = next_id;
	}

    return n;
}

static int select_add(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    iop_select_data_t *p = (iop_select_data_t *)(base->model_data);
    if(events & EV_TYPE_READ)
    {
        FD_SET(handle, &(p->rset));
    }
    if(events & EV_TYPE_WRITE)
    {
        FD_SET(handle, &(p->wset));
    }
#ifndef WIN32
    if(handle > p->maxfd)
    {
        p->maxfd = handle;
    }
#endif
    return 0;
}


static int select_del(iop_base_t *base, int id,io_handle_t handle)
{
    iop_select_data_t *p = (iop_select_data_t *)(base->model_data);
    FD_CLR(handle, &(p->rset));
    FD_CLR(handle, &(p->wset));
#ifndef WIN32
	/***linux下需要提供maxfd的值 ***/
    if(handle == p->maxfd)
    {
        iop_t *iop;
		int cur_id = base->io_list_head;
        p->maxfd = -1;

		while(cur_id != -1)
		{
			iop = base->iops+cur_id;
			if(cur_id!=id)
			{
				if(p->maxfd < iop->handle)
				{
					p->maxfd = iop->handle;
				}
			}
			cur_id = iop->next;
		}

    }
#endif
    return 0;
}

static int select_mod(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    iop_select_data_t *p = (iop_select_data_t *)(base->model_data);
    if(events & EV_TYPE_READ)
    {
        FD_SET(handle, &(p->rset));
    }
    else
    {
        FD_CLR(handle, &(p->rset));
    }

    if(events & EV_TYPE_WRITE)
    {
        FD_SET(handle, &(p->wset));
    }
    else
    {
        FD_CLR(handle, &(p->wset));
    }
    return 0;
}



int iop_init_select(void *iop_base, int maxev)
{
	iop_base_t *base = (iop_base_t *)iop_base;
	iop_op_t *iop_op = 0;
	iop_select_data_t *model_data = 0;
#ifdef WIN32
        static int init = 0;
        if(init == 0)
        {
            WSADATA     wsadata;
            if(WSAStartup(MAKEWORD( 1, 1 ),&wsadata)!=0)
            {
                    return   -1;
            }
            init = 1;
        }
#endif
    model_data = (iop_select_data_t *)(malloc(sizeof(iop_select_data_t)));
    if(!model_data)
    {
        return -1;
    }
    model_data->maxfd = -1;
    FD_ZERO(&(model_data->rset));
    FD_ZERO(&(model_data->wset));
    FD_ZERO(&(model_data->rset_out));
    FD_ZERO(&(model_data->wset_out));

    iop_op = &(base->op_imp);

    iop_op->name = "select";
    iop_op->base_free = select_free;
    iop_op->base_dispatch = select_dispatch;
    iop_op->base_add = select_add;
    iop_op->base_del = select_del;
    iop_op->base_mod = select_mod;

	base->model_data = model_data;
    return 0;
}



#endif

