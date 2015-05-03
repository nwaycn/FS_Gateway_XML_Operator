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
#include "../inc/iop_util.h"
#include "../inc/iop_hashmap.h"
#include "../inc/iop.h"
#include "../inc/iop_server.h"
#include "../inc/iop_log_service.h"

void iop_default_cb(iop_base_t *base, int id, void *arg){}

int iop_default_err_cb(iop_base_t *base, int id, int err, void *arg)
{
	LOG_DBG("iop_default_err_cb, id=%d,err=%d.\n", id,err);
	return -1;
}



int iop_default_parser(char *buf, int len){return len;}

int _iop_tcp_conn_cb(iop_base_t *base,int id,unsigned int events,void *arg)
{
	int n = 0;
    int r = 0;
	iop_t *iop = base->iops + id;
	iop_tcp_server_arg_t *sarg = (iop_tcp_server_arg_t *)(iop->sys_arg);
	if(events & EV_TYPE_DESTROY)
	{
		(sarg->on_destroy)(base,id,arg);	
		return 0;
	}
	if(EV_IS_SET_R(events))
	{
		n = iop_buf_recv(base,id);
        //LOG_DBG("iop_buf_recv = %d.\n", n);
		if(n < 0)
		{
			r = (sarg->on_error)(base,id,n,arg);
            if(r== -1){return -1;}
			return 0;
		}
		do{
			n = (sarg->parser)(dbuf_data_start(iop->rbuf), dbuf_size(iop->rbuf));
            //LOG_DBG("parser=%d,dbuf_size=%d.\n", n,dbuf_size(iop->rbuf));
			if(n < 0)
			{
				/*protocol error*/
				r = (sarg->on_error)(base,id,IOP_ERR_PROTOCOL,arg);
                if(r==-1){return -1;}
                break;
			}
			if(n==0){break;}
			r = (sarg->processor)(base,id,dbuf_data_start(iop->rbuf),n,arg);
            //LOG_DBG("processor=%d.\n", n);
			if(0 == r)
            {
                if(n==dbuf_size(iop->rbuf))
                {
                    dbuf_set_size(iop->rbuf,0);
                    break;
                }
                else
                {
                    dbuf_pop_front(iop->rbuf,n);
                }
            }
			else
            { 
                return -1;
            }
		}while(1);
	}

	if(EV_IS_SET_W(events))
	{
		if(dbuf_size(iop->sbuf) == 0)
		{
			iop_mod_events(base,id,EV_CLR_W(iop->events));
			return 0;
		}
		n = iop_write(iop->handle,dbuf_data_start(iop->sbuf),dbuf_size(iop->sbuf));
		if(n < 0)
		{
			if(socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
			{
				/*socket error*/
				r = (sarg->on_error)(base,id,IOP_ERR_SOCKET,arg);
                if(r==-1){return -1;}
			}
			return 0;
		}
		if(n== 0){return 0;}
        if(n >= dbuf_size(iop->sbuf)){dbuf_set_size(iop->sbuf,0);}
        else{dbuf_pop_front(iop->sbuf,n);}
            
	}

    if(EV_IS_SET(events,EV_TYPE_TIMEOUT))
    {
        /*keepalive timeout error*/
        r = (sarg->on_error)(base,id,IOP_ERR_TIMEOUT,arg);
        if(r==-1){return -1;}
    }
    return 0;
}



int _iop_tcp_server_cb(iop_base_t *base,int id,unsigned int events,void *arg)
{
	iop_tcp_server_arg_t *sarg = (iop_tcp_server_arg_t *)arg;
	LOG_DBG("tcp_server_cb, id=%d.events=%d.\n",id,events);
	if(events & EV_TYPE_DESTROY)
	{
		LOG_DBG("tcp server[%s,%d] destroy.\n",sarg->host, sarg->port);
		return 0;
	}
	if(events & EV_TYPE_READ)
	{
		int xid = 0;
		iop_t *iop = base->iops+id;
		io_handle_t h = iop_accept(iop->handle,NULL,NULL);
		LOG_DBG("accept new handle. h=%d.\n",h);
		if(h == INVALID_HANDLE){return 0;}
		xid = iop_add(base,h,EV_TYPE_READ,_iop_tcp_conn_cb,NULL,sarg->timeout);
		LOG_DBG("iop_add =%d.\n",xid);

		if(xid < 0){
			iop_close_handle(h);return 0;}
		iop=base->iops + xid;
		iop->sys_arg = arg;
		(sarg->on_connect)(base,xid,NULL);
	}	
    return 0;
}





int iop_add_tcp_server(iop_base_t *base, const char *host, unsigned short port,
					   iop_parser parser, iop_processor processor,
					   iop_cb on_connect,iop_cb on_destroy,
					   iop_err_cb on_error, int keepalive_timeout)
{
	
	iop_tcp_server_arg_t *sarg = 0;
	io_handle_t h = INVALID_HANDLE;
	sarg = (iop_tcp_server_arg_t *)malloc(sizeof(iop_tcp_server_arg_t));
	if(!sarg){return -1;}
	memset(sarg,0,sizeof(iop_tcp_server_arg_t));
	h = iop_tcp_server(host,port);
	if(h == INVALID_HANDLE){return -1;}
	sarg = (iop_tcp_server_arg_t *)malloc(sizeof(struct tag_iop_tcp_server_arg_t));
	if(!sarg)
	{
		iop_close_handle(h);
		return -1;
	}
#ifdef WIN32
	strcpy_s(sarg->host,sizeof(sarg->host)-1, host);
#else
	strcpy(sarg->host,host);
#endif
	sarg->port = port;
	sarg->timeout = keepalive_timeout;
	sarg->on_connect = on_connect;
	sarg->on_destroy = on_destroy;
	sarg->on_error = on_error;
	sarg->parser = parser;
	sarg->processor = processor;
	_list_add_before(base->tcp_protocol_list_head, _list_node_new(sarg));		 
	return iop_add(base,h,EV_TYPE_READ,_iop_tcp_server_cb,(void *)sarg,-1);
}


int iop_add_t_server(iop_base_t *base, const char *host, unsigned short port,
					   iop_parser parser, iop_processor processor, int keepalive_timeout)
{
	return iop_add_tcp_server(base,host,port,parser,processor,iop_default_cb,iop_default_cb,iop_default_err_cb,keepalive_timeout);
}




static int iop_mov_connect(iop_base_t *base,int id,unsigned int events)
{
    int r = 0;
    iop_t *iop = iop_get(base,id);
    
    iop->events = events;
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


    iop->prev = -1;
    iop->next = base->io_list_head;
    base->io_list_head = iop->id;
    iop->iop_type = IOP_TYPE_IO;
    r = (*(base->op_imp.base_mod))(base, iop->id, iop->handle, events);
    if(r != 0)
    {
        return -1;
    }
    return 0;    
}


extern iop_t *_iop_base_get_free_node(iop_base_t *base);


/*
*function:
*	增加一个新的异步连接事件到iop_base_t中
*parameters:
*	base:iop_base_t指针
*     handle:socket 句柄
*	evcb:事件回调函数
*	arg:用户指定的参数,用户负责释放，一般在event_cb中，EV_TYPE_DESTROY事件中释放
*	timeout:超时值（秒），-1代表永不超时
*Return:
*	成功返回对象id,失败返回-1
*/
int iop_add_connect(iop_base_t *base,io_handle_t handle,iop_event_cb evcb,void *arg,int timeout)
{
    unsigned events = EV_TYPE_RW;
	int r = 0;
	iop_t *iop = 0;
	if(handle == INVALID_HANDLE)
	{
        return -1;
	}
    iop = _iop_base_get_free_node(base);
	if(!iop){return -1;}
	iop->handle = handle;
	iop->events = events;
	iop->timeout = timeout;
	iop->evcb = evcb;
	iop->last_dispatch_time = base->cur_time;
	iop->arg = arg;

	//LOG_DBG("iop_add io, id=%d.\n", iop->id);
	iop->prev = -1;
	iop->next = base->io_list_head;
	base->connect_list_head = iop->id;
	iop->iop_type = IOP_TYPE_CONNECT;
    iop_set_nonblock(handle);
	r = (*(base->op_imp.base_add))(base, iop->id, handle, events);
	if(r != 0)
	{
		iop_del(base,iop->id);
		return -1;
	}

    return iop->id;
}





static int _iop_asyn_conn_cb(iop_base_t *base,int id, unsigned int event,void *user_arg)
{
    int r = 0;
	iop_event_cb cb;
	int error = 0;
    iop_t *iop = iop_get(base,id);
    if(!iop){return -1;}
    cb = (iop_event_cb)(iop->sys_arg);

    
    
    if(EV_IS_SET_R(event) || EV_IS_SET_W(event))
    {
        iop_socklen_t len = sizeof(error);
        if(getsockopt(iop->handle, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0)
        {
           
            cb(base, id, EV_TYPE_ERROR,user_arg);
            iop_del(base,id);
            return 0;
        }
        else
        {
            if(error)
            {
                cb(base, id, EV_TYPE_ERROR,user_arg);
                iop_del(base,id);
                return 0;
            }
            else
            {
                r = iop_mov_connect(base,id,EV_TYPE_READ);
                if(r == -1)
                {
                    cb(base, id, EV_TYPE_ERROR,user_arg);
                    iop_del(base,id);
                    return 0;
                }
                iop->evcb = cb;
                r = cb(base, id, EV_TYPE_CONNECT,user_arg);
                if(r == -1){iop_del(base,id);}
                //printf("$$$$$Connect ok, delay...\n");                
            }
        }
    }
    if(EV_IS_SET(event, EV_TYPE_TIMEOUT))
    {
        r = cb(base, id, EV_TYPE_TIMEOUT,user_arg);
        return r;
    }

    if(EV_IS_SET(event, EV_TYPE_DESTROY))
    {
        r = cb(base, id, EV_TYPE_DESTROY,user_arg);
        return r;
    }
    return 0;
}





/*
* cb :连接成功或失败或超时的回调。
*/

int iop_asyn_connect(iop_base_t *base, 
                                        const char *ip, 
                                        unsigned short port, 
                                        int connect_timeout,
                                        iop_event_cb cb, 
                                        void *cb_arg)
{

    //printf("$$$$ asyn_connect...\n");
	int ret = -1; 
	iop_t *iop = 0;
    int id = -1;
    io_handle_t sock = IOP_SOCK_STREAM();
    if(sock == INVALID_HANDLE)
    {
        cb(base,-1,EV_TYPE_ERROR|EV_TYPE_DESTROY, cb_arg);
        return -1;
    }

    if(iop_set_nonblock(sock) != 0)
    {
        iop_close_handle(sock);
        cb(base,-1,EV_TYPE_ERROR|EV_TYPE_DESTROY, cb_arg);
        return -1;
    }

        
    ret = iop_connect(sock, ip,port);
    if(ret < 0 && socket_errno != EINPROGRESS && socket_errno != EWOULDBLOCK)
    {
        cb(base,-1,EV_TYPE_ERROR|EV_TYPE_DESTROY, cb_arg);
        return -1;
    }


    if(ret == 0)
    {
        id = iop_add(base, sock, EV_TYPE_READ, cb, cb_arg, -1);
        if(id < 0){
            cb(base, id, EV_TYPE_ERROR|EV_TYPE_DESTROY, cb_arg);return -1;}
        cb(base,id,EV_TYPE_CONNECT, cb_arg);
        return 0;
    }

    
    id = iop_add_connect(base, sock, _iop_asyn_conn_cb, cb_arg,connect_timeout);
    if(id==-1)
    {
        iop_close_handle(sock);
        cb(base,-1,EV_TYPE_ERROR|EV_TYPE_DESTROY, cb_arg);
        return -1;
    }
    iop = iop_get(base,id);
    iop->sys_arg = (void *)cb;
    return 0;
}




