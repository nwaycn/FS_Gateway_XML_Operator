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
#ifndef _IOP_H_
#define _IOP_H_
#include <time.h>
#include "iop_def.h"
#include "iop_config.h"
#include "iop_util.h"
#include "array_list.h"
#include "iop_list.h"
#include "iop_buf.h"
#include "iop_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/***********************************
*function:
*	创建新的iop_base_t,linux下使用epoll模型，windwos下使用select模型
*parameters:
*	maxio:最大支持的并发io数
*Return:
*	成功返回iop_base_t对象指针，失败返回NULL.
*/
iop_base_t* iop_base_new(int maxio);



/***************************************
*function:
*	创建新的iop_base_t,使用用户指定的模型
*parameters:
*	maxio:最大支持的并发io数
*	model:事件模型, select/poll/epoll/timer, 暂不支持iocp
*Return:
*	成功返回iop_base_t对象指针，失败返回NULL.
*/
iop_base_t* iop_base_new_special(int maxio,const char *model);


/***********************************
*function:
*	释放iop_base_t
*parameters:
*	base:iop_base_t指针
*Return:
*	无
*/
void iop_base_free(iop_base_t *base);


/***********************************
*function:
*	启动一次事件调度
*parameters:
*	base:iop_base_t指针
*Return:
*	本次处理的事件总数
*/
int iop_dispatch(iop_base_t *base);


/*
*function:
*	启动循环事件调度，直到退出
*parameters:
*	base:iop_base_t指针
*Return:
*	无
*/
void iop_run(iop_base_t *base);


/*
*function:
*	在新线程中启动循环事件调度，直到退出，
*	相应的base对象在线程退出时会自动清理
*parameters:
*	base:iop_base_t指针
*	thread:返回的线程id.
*Return:
*	0:成功，-1：失败
*/
int iop_run_in_thread(iop_base_t *base,iop_thread_t *thread);


/*
*function:
*	退出循环事件调度
*parameters:
*	base:iop_base_t指针
*Return:
*	无
*/
void iop_stop(iop_base_t *base);


/*
*function:
*	增加一个新的事件对象到iop_base_t中
*parameters:
*	base:iop_base_t指针
*	handle:socket句柄，如果是定时器，则为INVALID_HANDLE
*	events:关注的事件
*	evcb:事件回调函数
*	arg:用户指定的参数,用户负责释放，一般在event_cb中，EV_TYPE_DESTROY事件中释放
*	timeout:超时值（秒），-1代表永不超时
*Return:
*	成功返回对象id,失败返回-1
*/
int iop_add(iop_base_t *base,io_handle_t handle,unsigned int events,iop_event_cb evcb,void *arg,int timeout);



/*
*function:
*	增加一个新的定时器事件到iop_base_t中
*parameters:
*	base:iop_base_t指针
*	evcb:事件回调函数
*	arg:用户指定的参数,用户负责释放，一般在event_cb中，EV_TYPE_DESTROY事件中释放
*	timeout:超时值（秒），-1代表永不超时
*Return:
*	成功返回对象id,失败返回-1
*/
int iop_add_timer(iop_base_t *base,iop_event_cb evcb,void *arg,int timeout);





/*
*function:
*	从iop_base_t中删除指定id的事件对象
*parameters:
*	base:iop_base_t指针
*	id:事件对象id
*Return:
*	成功返回0,失败返回-1
*/
int iop_del(iop_base_t *base,int id);



/*
*function:
*	从iop_base_t中查找指定id的事件对象,一般情况下不要直接修改iop_t对象
*parameters:
*	base:iop_base_t指针
*	id:事件对象id
*Return:
*	成功返回iop_t指针,失败返回NULL
*/
iop_t* iop_get(iop_base_t *base,int id);



/*
*function:
*	修改关注的事件
*parameters:
*	base:iop_base_t指针
*	id:iop对象id
*	events:新关注的事件
*Return:
*	成功返回0,失败返回-1
*/
int iop_mod_events(iop_base_t *base,int id,unsigned int events);


/*
*function:
*	发送数据，如果不能立即发送，则被加入缓存队列，等等网络可用时自动发送
*parameters:
*	base:iop_base_t指针
*	id:iop对象id
*	data:要发送的数据
*	len:要发送的数据长度
*	events:新关注的事件
*Return:
*	成功返回0,失败返回<0,  IOP_ERR_SOCKET:网络错误 IOP_ERR_NO_MEM：内存异常
*/
int iop_buf_send(iop_base_t *base,int id,const void *data,int len);
/*
*function:
*	接收数据并存入缓存区
*parameters:
*	base:iop_base_t指针
*	id:iop对象id
*Return:
*	成功返回>=0,失败<0 返回IOP_ERR_SOCKET:网络错误 IOP_ERR_NO_MEM：内存异常
*/
int iop_buf_recv(iop_base_t *base, int id);


/*
*function:
*	设置自定义参数
*parameters:
*	base:iop_base_t指针
*	id:iop对象id
*	arg:自定义参数
*Return:
*/
void iop_set_arg(iop_base_t *base, int id,void *arg);




#ifdef __cplusplus
}
#endif

#endif

