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
#include <stdlib.h>
#include <stdio.h>
#include "../inc/iop.h"
#include "../inc/iop_util.h"
#include "../inc/iop_epoll.h"
#include "../inc/iop_poll.h"
#include "../inc/iop_select.h"
#include "../inc/iop_timer.h"
#include "../inc/iop_log_service.h"
#include "../inc/iop_server.h"


static int _default_iop_event_cb(iop_base_t *base,int id, unsigned int events,void *arg)
{
    return 0;
}

iop_t *_iop_base_get_free_node(iop_base_t *base)
{
	iop_t *iop = NULL;
	if(base->free_list_head == -1)
	{	
		return NULL;
	}
	iop = (base->iops)+base->free_list_head;
	base->free_list_head = iop->next;
	if(base->free_list_head == -1){base->free_list_tail = -1;}
	if(base->free_list_head != -1)
	{
		(base->iops)[base->free_list_head].prev = -1;
	}
	return iop;
}

static void _iop_init(iop_t *iop)
{
	iop->handle = INVALID_HANDLE;
	iop->prev=0;			
	iop->next=0;			
	iop->events=0;		
	iop->timeout=0;		
	iop->evcb = _default_iop_event_cb;
	iop->arg = 0;
	iop->sys_arg = 0;
	iop->sbuf = NULL;
	iop->rbuf=NULL;
}

iop_base_t* iop_base_new(int maxio)
{
#ifdef _HAVE_EVENT_PORTS_
#endif
#ifdef _HAVE_WORKING_KQUEUE_
#endif
#ifdef _HAVE_EPOLL_
	return iop_base_new_special(maxio,"epoll");
#endif
#ifdef _HAVE_DEVPOLL_
#endif
#ifdef _HAVE_POLL_
	return iop_base_new_special(maxio,"poll");
#endif
#ifdef _HAVE_SELECT_
	return iop_base_new_special(maxio,"select");
#endif
    return NULL;
}

static void _iop_op_init(iop_op_t *op)
{
	op->name = "NULL";
	op->base_free = NULL;
    op->base_dispatch=NULL;
	op->base_add=NULL;
    op->base_del = NULL;
    op->base_mod = NULL;
}

static iop_base_t* _iop_base_new(int maxio)
{
	int i = 0;
	int prev = -1;
	int next = 1;
	iop_t *iop = NULL;
	iop_base_t *base = NULL;
	if(maxio <= 0){return NULL;}
	base = (iop_base_t *)malloc(sizeof(iop_base_t));
	if(!base){return NULL;}
	_iop_op_init(&(base->op_imp));
	base->dispatch_interval = DEFAULT_DISPATCH_INTERVAL;
	base->cur_time = time(NULL);
	base->last_time = base->cur_time;
	base->last_keepalive_time = base->cur_time;
	base->iops = (iop_t *)malloc(sizeof(iop_t)*maxio);
	if(!(base->iops)){iop_base_free(base);return NULL;}
	base->maxio = maxio;
	for(i = 0; i < maxio; i++)
	{
		_iop_init((base->iops)+i);
	}
	base->exit_flag = 0;
	base->io_list_head = -1;
	base->timer_list_head = -1;
	base->connect_list_head = -1;
	base->free_list_head = 0;
	base->free_list_tail = maxio-1;
	for(i = 0; i < maxio; i++)
	{
		iop = (base->iops)+i;
		iop->prev = prev;
		iop->next = next;
		prev = i;
		next = next+1;
		iop->id = i;
	}
	base->tcp_protocol_list_head = 0;
	SET_RLIMIT_NOFILE(102400);

	return base;
}

iop_base_t* iop_base_new_special(int maxio,const char *model)
{
	int r = -1;
	iop_base_t *base = NULL;
	if(strcmp(model,"epoll")==0)
	{
		base = _iop_base_new(maxio);
		if(base)
		{
			r = iop_init_epoll(base, maxio);
		}
	}
	else if(strcmp(model,"poll")==0)
	{
		base = _iop_base_new(maxio);
		if(base)
		{
			r = iop_init_poll(base, maxio);
		}

	}
	else if(strcmp(model,"select")==0)
	{
		base = _iop_base_new(maxio);
		if(base)
		{
			r = iop_init_select(base, maxio);
		}

	}
	else if(strcmp(model,"timer")==0)
	{
		base = _iop_base_new(maxio);
		if(base)
		{
			r = iop_init_timer(base, maxio);
		}

	}
	if(base && r != 0)
	{
		iop_base_free(base);
		base = NULL;
	}
	return base;
}








void iop_base_free(iop_base_t *base)
{
	int i = 0;
	//LOG_DBG("%s","base free...\n");
	if(!base){return;}
	if(base->iops)
	{
		while(base->io_list_head != -1)
		{
			//LOG_DBG("XXX###:del io %d.\n", base->io_list_head);
			iop_del(base,base->io_list_head);
			
		}
		while(base->timer_list_head != -1)
		{
			//LOG_DBG("YYYY###:iop_del timer %d.\n",base->timer_list_head);
			iop_del(base,base->timer_list_head);
		}
		while(base->connect_list_head != -1)
		{
			//LOG_DBG("YYYY###:iop_del timer %d.\n",base->timer_list_head);
			iop_del(base,base->connect_list_head);
		}

		for(i=0;i<base->maxio;i++)
		{
			if(base->iops[i].sbuf){dbuf_free(base->iops[i].sbuf);}
			if(base->iops[i].rbuf){dbuf_free(base->iops[i].rbuf);}
		}
		free(base->iops);
		base->iops = NULL;
		base->maxio = 0;
	}

	
	if(base->op_imp.base_free)
	{
		(*((base->op_imp).base_free))(base);
	}

	while(base->tcp_protocol_list_head)
	{
		_list_node_t *next = base->tcp_protocol_list_head->next;
		iop_tcp_server_arg_t *sarg = (iop_tcp_server_arg_t *)(base->tcp_protocol_list_head->data);
		if(sarg != NULL){free(sarg);}
		free(base->tcp_protocol_list_head);
		base->tcp_protocol_list_head = next;
	}
	
	free(base);
}


int iop_dispatch(iop_base_t *base)
{
	int cur_id = 0;
	int next_id = 0;
    int r = 0;
    iop_t *iop = (iop_t *)0;
    
    dispatch_imp_cb dispatch_cb = base->op_imp.base_dispatch;
    r = (*dispatch_cb)(base,base->dispatch_interval);
    if( r == -1)
    {
        return -1;
    }

    
    if(base->cur_time > base->last_time)
    {
		//check timers...
		cur_id = base->timer_list_head;
        while(cur_id != -1)
        {
			iop = base->iops + cur_id;
			next_id = iop->next;
			if(base->cur_time > iop->last_dispatch_time + iop->timeout)
			{
				IOP_CB(base,iop,EV_TYPE_TIMER);
			}
			cur_id = next_id;
        }



        /*********check for connect list.*********************/

		cur_id = base->connect_list_head;
        while(cur_id != -1)
        {
			iop = base->iops + cur_id;
			next_id = iop->next;
			if(base->cur_time > iop->last_dispatch_time + iop->timeout)
			{
				IOP_CB(base,iop,EV_TYPE_TIMEOUT);
			}
			cur_id = next_id;
        }


		/*********clear keepalive, 60 seconds per times***********************/
	    if(base->cur_time > base->last_keepalive_time+60)
		{
			base->last_keepalive_time = base->cur_time;
			cur_id = base->io_list_head;
			while(cur_id != -1)
			{
				iop = base->iops+cur_id;
				next_id = iop->next;
				if(iop->timeout > 0 && iop->last_dispatch_time + iop->timeout < base->cur_time)
				{
					IOP_CB(base,iop,EV_TYPE_TIMEOUT);
				}
				cur_id = next_id;
			}
		}

        base->last_time = base->cur_time;
	}

    return r;
}



void iop_run(iop_base_t *base)
{
	while(base->exit_flag == 0)
	{
		iop_dispatch(base);
	}
	iop_base_free(base);
}


thread_ret_type thread_func_call iop_dispatch_thread(void *arg)
{
	thread_ret_type ret = 0;
	iop_base_t *base = (iop_base_t *)arg;
	while(base->exit_flag == 0)
	{
		iop_dispatch(base);
	}
	iop_base_free(base);
	return ret;
}


int iop_run_in_thread(iop_base_t *base,iop_thread_t *thread)
{
	return iop_thread_create(thread,iop_dispatch_thread,(void *)base,1024*1024*8);
}



void iop_stop(iop_base_t *base){base->exit_flag = 1;}


int iop_add(iop_base_t *base,io_handle_t handle,unsigned int events,iop_event_cb evcb,void *arg,int timeout)
{
	int r = 0;
	iop_t *iop = _iop_base_get_free_node(base);
	if(!iop){return -1;}
	iop->handle = handle;
	iop->events = events;
	iop->timeout = timeout;
	iop->evcb = evcb;
	iop->last_dispatch_time = base->cur_time;
	iop->arg = arg;

	if(handle != INVALID_HANDLE)
	{
		//LOG_DBG("iop_add io, id=%d.\n", iop->id);
		iop->prev = -1;
		iop->next = base->io_list_head;
		base->io_list_head = iop->id;
		iop->iop_type = IOP_TYPE_IO;
        iop_set_nonblock(handle);
		r = (*(base->op_imp.base_add))(base, iop->id, handle, events);
		if(r != 0)
		{
			iop_del(base,iop->id);
			return -1;
		}
	}
	else
	{
		/*timer*/
		//LOG_DBG("iop_add timer, id=%d.\n", iop->id);
		iop->prev = -1;
		iop->next = base->timer_list_head;
		base->timer_list_head = iop->id;
		iop->iop_type = IOP_TYPE_TIMER;
	}
	return iop->id;	
}


int iop_add_timer(iop_base_t *base,iop_event_cb evcb,void *arg,int timeout)
{
	return iop_add(base,INVALID_HANDLE,EV_TYPE_TIMER,evcb,arg,timeout);
}



int iop_del(iop_base_t *base,int id)
{
	iop_t *iop = (base->iops)+id;
	//LOG_DBG("iop_del id=%d.type=%d.\n",id,iop->iop_type);
	switch(iop->iop_type)
	{
	case IOP_TYPE_IO:
		(iop->evcb)(base,id,EV_TYPE_DESTROY,iop->arg);
		if(iop->handle != INVALID_HANDLE)
		{
			(*(base->op_imp.base_del))(base, iop->id, iop->handle);
			iop_close_handle(iop->handle);
			iop->handle = INVALID_HANDLE;
		}

		if(iop->prev == -1)
		{
			base->io_list_head = iop->next;
			if(iop->next != -1)
			{
				(base->iops)[base->io_list_head].prev = -1;
			}
		}
		else
		{
			iop_t *pnode = (base->iops)+iop->prev;
			pnode->next = iop->next;
			if(pnode->next != -1)
			{
				(base->iops)[pnode->next].prev = pnode->id;
			}
		}

		iop->prev = base->free_list_tail;
		iop->next = -1;
		base->free_list_tail = iop->id;

		if(base->free_list_head==-1)
		{
			base->free_list_head = iop->id;
		}
		break;
	case IOP_TYPE_TIMER:
		(iop->evcb)(base,id,EV_TYPE_DESTROY,iop->arg);	
		if(iop->prev == -1)
		{
			base->timer_list_head = iop->next;
			if(iop->next != -1)
			{
				(base->iops)[base->timer_list_head].prev = -1;
			}
		}
		else
		{
			iop_t *pnode = (base->iops)+iop->prev;
			pnode->next = iop->next;
			if(pnode->next != -1)
			{
				(base->iops)[pnode->next].prev = pnode->id;
			}
		}
		iop->prev = base->free_list_tail;
		iop->next = -1;
		base->free_list_tail = iop->id;

		if(base->free_list_head==-1)
		{
			base->free_list_head = iop->id;
		}
		break;
    case IOP_TYPE_CONNECT:
        (iop->evcb)(base,id,EV_TYPE_DESTROY,iop->arg);
        if(iop->handle != INVALID_HANDLE)
        {
            (*(base->op_imp.base_del))(base, iop->id, iop->handle);
            iop_close_handle(iop->handle);
            iop->handle = INVALID_HANDLE;
        }
    
        if(iop->prev == -1)
        {
            base->connect_list_head = iop->next;
            if(iop->next != -1)
            {
                (base->iops)[base->connect_list_head].prev = -1;
            }
        }
        else
        {
            iop_t *pnode = (base->iops)+iop->prev;
            pnode->next = iop->next;
            if(pnode->next != -1)
            {
                (base->iops)[pnode->next].prev = pnode->id;
            }
        }
    
        iop->prev = base->free_list_tail;
        iop->next = -1;
        base->free_list_tail = iop->id;
    
        if(base->free_list_head==-1)
        {
            base->free_list_head = iop->id;
        }
        break;


	default:
		//LOG_DBG("$$$ del unknown %d, type=%d.\n", iop->id,iop->iop_type);
		break;
	}
	iop->iop_type = IOP_TYPE_FREE;
	return 0;
}


iop_t* iop_get(iop_base_t *base,int id)
{
	return base->iops+id;
}



int iop_mod_events(iop_base_t *base,int id,unsigned int events)
{
	iop_t *iop = base->iops + id;
	if(iop->iop_type != IOP_TYPE_IO){return -1;}
	if(iop->handle == INVALID_HANDLE){return -1;}
	
	return (*(base->op_imp.base_mod))(base, iop->id, iop->handle,events);
}

int iop_buf_send(iop_base_t *base,int id,const void *data,int len)
{
	const char *cache_start=(const char *)data;
	int r = 0;
	iop_t *iop = base->iops + id;

	if(iop->sbuf == NULL)
	{
		iop->sbuf = dbuf_new(len);	
	}
	if(iop->sbuf == NULL){return IOP_ERR_NO_MEM;}

	if(dbuf_size(iop->sbuf)==0)
	{
		r = iop_write(iop->handle,data,len);
		if(r == len){return 0;}/*all send*/
		if(r < 0)
		{
			if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK){return IOP_ERR_SOCKET;}
            r = 0;
		}
		cache_start=(const char *)data+r;
	}

	if(dbuf_capacity(iop->sbuf) > IOP_MAX_BUF_SIZE){return IOP_ERR_NO_MEM;}

	r = dbuf_push_back(iop->sbuf,cache_start,(unsigned int)(len-((const char *)data - cache_start)));
	if(-1 == r)
	{
		/*no memory*/
		return IOP_ERR_NO_MEM;
	}
	if(EV_IS_SET_W(iop->events)){return 0;}
	iop_mod_events(base,id,iop->events | EV_TYPE_WRITE);
	return 0;
}



int iop_buf_recv(iop_base_t *base,int id)
{
	int r = 0;
	iop_t *iop = base->iops + id;
	dbuf_t *buf = iop->rbuf;
	if(buf == NULL)
	{
		buf = dbuf_new(9600);
		if(buf == NULL){return IOP_ERR_NO_MEM;}
		iop->rbuf = buf;
	}
	else
	{
		if(dbuf_capacity(buf) > IOP_MAX_BUF_SIZE){return IOP_ERR_NO_MEM;}
		if(dbuf_ava_capacity(buf)==0)
		{
			if(-1==dbuf_expand(buf,9600)){return IOP_ERR_NO_MEM;}
		}
	}
	//LOG_DBG("buf ava =%d.\n", dbuf_ava_capacity(buf));
	r = iop_read(iop->handle,dbuf_data_end(buf),dbuf_ava_capacity(buf));
	//LOG_DBG("iop_read = %d.\n", r);
	if(r < 0)
	{
		if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK){
			//LOG_DBG("iop_buf_read error:%d.\n", socket_errno);
			return IOP_ERR_SOCKET;}
		//LOG_DBG("iop_buf_read timeout.\n");
		return 0;	/*unreadable*/
	}
	else if(r == 0)
	{
		return IOP_ERR_CLOSED;	/*closed by peer*/
	}
    dbuf_add_size(buf,r);
	return r;		
}



void iop_set_arg(iop_base_t *base, int id,void *arg)
{
	(base->iops)[id].arg = arg;
}

