/*
 * Copyright (c) 2011-2014 zhangjianlin ’≈ºˆ¡÷
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
#ifndef _HAVE_POLL_
int iop_init_poll(void *iop_base, int maxev)
{
    return -1;
}
#else
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <poll.h>
#include <sys/time.h>
#include <sys/types.h>

#include "iop.h"
#include "iop_poll.h"



struct tag_iop_poll_data {
    int nfds;	
    int cur_num;
    struct pollfd *event_set;
    int *id_map;
    int has_new_ev;
};
typedef struct tag_iop_poll_data iop_poll_data_t;

static unsigned int to_poll_events(unsigned int what)
{
    unsigned int events=0;
    if(what & EV_TYPE_READ)    
    {
        events = POLLIN;
    }
    if(what & EV_TYPE_WRITE)    
    {
        events |= POLLOUT;
    }
    return events;
}

static unsigned int from_poll_events(unsigned int events)
{
    unsigned int what=0;
    if(events & (POLLHUP|POLLERR))
    {
        what = EV_TYPE_READ | EV_TYPE_WRITE;
    }
    else
    {
        if(events & POLLIN){what |= EV_TYPE_READ;}
        if(events & POLLOUT){what |= EV_TYPE_WRITE;}
    }
    return what;            
}


static void poll_free(iop_base_t *base)
{
    iop_poll_data_t *p = (iop_poll_data_t *)(base->model_data);
    if(p)
    {
        if(p->event_set){free(p->event_set);}
        if(p->id_map){free(p->id_map);}
        free(p);
    }
    base->model_data = (void *)0;
}


static int poll_dispatch(iop_base_t * base, int timeout)
{
    unsigned int events;
    int num = 0;
    int i;
    int id;
    iop_t *iop;
    iop_poll_data_t *model_data = (iop_poll_data_t *)(base->model_data);
    if(model_data->has_new_ev)
    {
		int cur_id = base->io_list_head;
        model_data->cur_num = 0;
        
        while(cur_id != -1)
        {
            iop = base->iops+cur_id;
            if(iop->handle != INVALID_HANDLE)
            {
                model_data->event_set[model_data->cur_num].fd = iop->handle;
                model_data->event_set[model_data->cur_num].events = to_poll_events(iop->events);
                model_data->event_set[model_data->cur_num].revents = 0;
                model_data->id_map[model_data->cur_num] = cur_id;
                ++model_data->cur_num;
            }
			cur_id = iop->next;
        }
    }

    int n = 0;
    do{
        n = poll(model_data->event_set, model_data->cur_num,timeout);    
    }while((n < 0) && (errno == EINTR));


    base->cur_time = time(NULL);
    model_data->has_new_ev = 0;
    
    for(i = 0; i < model_data->cur_num; i++)
    {
        events = from_poll_events((model_data->event_set)[i].revents);
        if(events == 0)
        {
            continue;
        }
        id = (model_data->id_map)[i];
		iop = base->iops + id;
        IOP_CB(base,iop,events);
        ++num;
        if(num >= n){break;}
    }
    return n;
}

static int poll_add(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    iop_poll_data_t *model_data = (iop_poll_data_t *)(base->model_data);
    model_data->has_new_ev = 1;
    return 0;
}


static int poll_del(iop_base_t *base, int id,io_handle_t handle)
{
    iop_poll_data_t *model_data = (iop_poll_data_t *)(base->model_data);
    model_data->has_new_ev = 1;
    return 0;
}

static int poll_mod(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    iop_poll_data_t *model_data = (iop_poll_data_t *)(base->model_data);
    model_data->has_new_ev = 1;
    return 0;
}



int iop_init_poll(void *iop_base, int maxev)
{
    iop_base_t *base = (iop_base_t *)iop_base;
    iop_poll_data_t *model_data = (iop_poll_data_t *)(malloc(sizeof(iop_poll_data_t)));
    if(!model_data)
    {
        return -1;
    }
    model_data->nfds = maxev;
    model_data->event_set = (struct pollfd *)(malloc(sizeof(struct pollfd) * maxev));
    if(!model_data)
    {
        free(model_data);
        return -1;
    }
    model_data->nfds = maxev;
    model_data->has_new_ev = 0;

    model_data->id_map = (int *)(malloc(sizeof(int)*maxev));
    if(!model_data->id_map)
    {
        free(model_data->event_set);
        free(model_data);
        return -1;
    }


	iop_op_t *iop_op = &(base->op_imp);
    
    iop_op->name = "poll";
    iop_op->base_free = poll_free;
    iop_op->base_dispatch = poll_dispatch;
    iop_op->base_add = poll_add;
    iop_op->base_del = poll_del;
    iop_op->base_mod = poll_mod;
    
    base->model_data = model_data;
    return 0;
}


#endif


