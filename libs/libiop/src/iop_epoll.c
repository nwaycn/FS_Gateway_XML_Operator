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
#ifndef _HAVE_EPOLL_
int iop_init_epoll(void *iop_base, int maxev)
{
    return -1;
}
#else
#include <stdlib.h>
#include <errno.h>

#include "iop.h"
#include "iop_epoll.h"

#include <unistd.h>
#include <sys/epoll.h>

struct tag_epoll_data {
    struct epoll_event *events;
    int nevents;
    int epfd;
};
typedef struct tag_epoll_data iop_epoll_data_t;

static uint32_t to_epoll_events(uint32_t what)
{
    uint32_t events=0;
    if(what & EV_TYPE_READ)    
    {
        events = EPOLLIN;
    }
    if(what & EV_TYPE_WRITE)    
    {
        events |= EPOLLOUT;
    }
    return events;
}

static uint32_t from_epoll_events(uint32_t events)
{
    uint32_t what=0;
    if(events & (EPOLLHUP|EPOLLERR))
    {
        what = EV_TYPE_READ | EV_TYPE_WRITE;
    }
    else
    {
        if(events & EPOLLIN){what |= EV_TYPE_READ;}
        if(events & EPOLLOUT){what |= EV_TYPE_WRITE;}
    }
    return what;            
}


static void epoll_free(iop_base_t *base)
{
    iop_epoll_data_t *iop_data = (iop_epoll_data_t *)(base->model_data);
    if(!iop_data){return;}
    if(iop_data->events)
    {
        free(iop_data->events);
    }
    if(iop_data->epfd >= 0)
    {
        close(iop_data->epfd);
    }
    free(iop_data);
    base->model_data = (void *)0;
}

static int epoll_dispatch(iop_base_t * base, int timeout)
{
    int i;
    int id = 0;
    iop_t *iop = NULL;
    iop_epoll_data_t *iop_data = (iop_epoll_data_t *)(base->model_data);
    int n = 0;
    do{
        n = epoll_wait(iop_data->epfd, iop_data->events, iop_data->nevents, timeout);    
    }while((n < 0) && (errno == EINTR));
    base->cur_time = time(NULL);
    for(i = 0; i < n; i++)
    {
        id = (int)((iop_data->events)[i].data.u32);
		if(id >= 0 && id < base->maxio)
		{
			iop = (base->iops)+id;
			IOP_CB(base,iop,from_epoll_events(iop_data->events[i].events));
		}
    }
    return n;
}

static int epoll_add(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    iop_epoll_data_t *iop_data = (iop_epoll_data_t *)(base->model_data);
    struct epoll_event ev;
    ev.data.u32 = id;  
    ev.events = to_epoll_events(events);
    //iop_set_nonblock(handle);
    return epoll_ctl(iop_data->epfd, EPOLL_CTL_ADD, (int)handle, &ev);                
}


static int epoll_del(iop_base_t *base, int id,io_handle_t handle)
{
    iop_epoll_data_t *iop_data = (iop_epoll_data_t *)(base->model_data);
    struct epoll_event ev;
    ev.data.u32 = id;  
    ev.events = 0;
    return epoll_ctl(iop_data->epfd, EPOLL_CTL_DEL, (int)handle, &ev);                
}

static int epoll_mod(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    iop_epoll_data_t *iop_data = (iop_epoll_data_t *)(base->model_data);
    struct epoll_event ev;
    ev.data.u32 = id;  
    ev.events = to_epoll_events(events);
    return epoll_ctl(iop_data->epfd, EPOLL_CTL_MOD, (int)handle, &ev);                
}



int iop_init_epoll(void *iop_base, int maxev)
{
	iop_base_t *base = (iop_base_t *)iop_base;
	iop_op_t *iop_op = &(base->op_imp);

	iop_epoll_data_t *iop_data = (iop_epoll_data_t *)(malloc(sizeof(iop_epoll_data_t)));
    if(!iop_data)
    {
        return -1;
    }
    iop_data->nevents = maxev;
    iop_data->events = (struct epoll_event *)(malloc(sizeof(struct epoll_event) * maxev));
    if(!iop_data)
    {
        free(iop_data);
        return -1;
    }

    iop_op->name = "epoll";
    iop_op->base_free = epoll_free;
    iop_op->base_dispatch = epoll_dispatch;
    iop_op->base_add = epoll_add;
    iop_op->base_del = epoll_del;
    iop_op->base_mod = epoll_mod;
    
    //1024 is not the max events limit.
    int epfd = epoll_create(1024);
    if(epfd < 0)
    {
        free(iop_data->events);
        free(iop_data);
        free(iop_op);
        return -1;
    }
    iop_data->epfd = epfd;



    base->model_data = iop_data;
    
    return 0;
}

#endif

