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
#ifndef _IOP_DEF_H_
#define _IOP_DEF_H_
#include <time.h>
#include "iop_config.h"
#include "iop_util.h"
#include "array_list.h"
#include "iop_buf.h"

#ifdef __cplusplus
extern "C" {
#endif



#define IOP_ERR_SOCKET -1
#define IOP_ERR_NO_MEM -2
#define IOP_ERR_PROTOCOL -3
#define IOP_ERR_TIMEOUT -4
#define IOP_ERR_CLOSED -5



//读事件
#define EV_TYPE_READ                         1
//写事件
#define EV_TYPE_WRITE                       2

//创建事件
#define EV_TYPE_CREATE                   4

//销毁事件
#define EV_TYPE_DESTROY                   8

//定时器事件
#define EV_TYPE_TIMER                       16

// 超时
#define EV_TYPE_TIMEOUT				32

//错误事件
#define EV_TYPE_ERROR               64
//连接创建事件
#define EV_TYPE_CONNECT               128


#define IOP_ERR_SYS  -1


//一组操作事件类型的宏
#define EV_IS_SET_R(e) ((e)&EV_TYPE_READ)
#define EV_IS_SET_W(e) ((e)&EV_TYPE_WRITE)
#define EV_IS_SET(e,x) ((e)&x)


#define EV_SET_R(e) ((e) | EV_TYPE_READ)
#define EV_CLR_R(e) ((e) & ~EV_TYPE_READ)
#define EV_SET_W(e) ((e) | EV_TYPE_WRITE)
#define EV_CLR_W(e) ((e) & ~EV_TYPE_WRITE)

#define EV_SET_RW(e) ((e)|EV_TYPE_READ|EV_TYPE_WRITE)
#define EV_TYPE_RW (EV_TYPE_READ|EV_TYPE_WRITE)

//系统支持的事件模型

//NULL代表由系统自动选择
#define IOP_MODEL_NULL                      0
//event ports模型暂未实现
#define IOP_MODEL_EVENT_PORTS       1
//kqueue模型暂未实现
#define IOP_MODEL_KQUEUE                  2
//linux epoll模型已经支持
#define IOP_MODEL_EPOLL                     3
//dev poll模型暂未实现
#define IOP_MODEL_DEV_POLL              4
//poll模型已经支持
#define IOP_MODEL_POLL                       5
//select 模型已经支持. windows & linux
#define IOP_MODEL_SELECT                   6
//windows iocp模型暂未实现
#define IOP_MODEL_IOCP                       7
//只支持定时器
#define IOP_MODEL_TIMER                     8



#define DEFAULT_DISPATCH_INTERVAL	500

/*max socket data buf:64M*/
#define IOP_MAX_BUF_SIZE   67108864
#define IOP_TYPE_FREE	0
#define IOP_TYPE_IO		1
#define IOP_TYPE_TIMER	2
#define IOP_TYPE_CONNECT	3

struct tag_iop_op_t;
typedef struct tag_iop_op_t iop_op_t;
struct tag_iop_base_t;
typedef struct tag_iop_base_t iop_base_t;
struct tag_iop_t;
typedef struct tag_iop_t iop_t;

typedef time_t iop_time_t;

/*协议解析回调函数*/
typedef int (*iop_parse_cb)(const void *,unsigned int);

/*事件回调函数,返回-1代表要删除对象,返回0代表正常*/
typedef int (*iop_event_cb)(iop_base_t *,int,unsigned int,void *);

typedef int (*dispatch_imp_cb)(iop_base_t *, int);

/*********************************************************************/
/*
*tag_iop_t:iop结构，每一个iop对象都会对应一个tag_iop_t结构
*/
struct tag_iop_t
{
	int id;					/*对应的id*/
	io_handle_t handle;		/*关联的句柄*/
	int iop_type;			/*对象类型：0：free,1:io,2:timer*/
	int prev;				/*上一个对象*/
	int next;				/*下一个对象*/
	unsigned int events;				/*关注的事件*/
	int timeout;			/*超时值*/
	iop_event_cb evcb;		/*事件回调*/
	void *arg;				/*用户指定的参数,由用户负责释放资源*/
	void *sys_arg;			/*系统指定的参数，系统自动释放资源*/
	/*以下字段对定时器无用*/
	dbuf_t *sbuf;		/*发送缓存区*/
	dbuf_t *rbuf;		/*接收缓存区*/
	iop_time_t last_dispatch_time;	/*上次调度的时间*/
};


struct tag_iop_op_t
{
    const char *name;                               //模型名称
    void (*base_free)(iop_base_t *);        //资源释放的接口
    int (*base_dispatch)(iop_base_t *, int);  //模型调度接口
    //添加事件
    int (*base_add)(iop_base_t *, int, io_handle_t, unsigned int);
    //删除事件
    int (*base_del)(iop_base_t *, int,io_handle_t);
    //修改事件
    int (*base_mod)(iop_base_t *, int, io_handle_t, unsigned int);
};


struct tag_iop_base_t
{
	iop_t *iops;		/*所有iop*/
	int maxio;			/*最大并发io数,包括定时器在内*/
	int maxbuf;			/*单个发送或接收缓存的最大值*/
	int free_list_head;	/*可用iop列表*/
	int free_list_tail; /*最后一个可用iop*/
	int io_list_head;	/*已用io类型的iop列表*/
	int timer_list_head;	/*已用timer类型的iop列表*/
    int connect_list_head;  /*异步连接的iop列表*/
    volatile int exit_flag;	/*退出标志*/

	int dispatch_interval;		/*高度的间隔时间*/
    iop_op_t op_imp;           /*事件模型的内部实现*/
    void *model_data;         /*事件模型特定的数据*/

	iop_time_t cur_time;		/*当前调度时间*/
	iop_time_t last_time;		/*上次调度时间*/
	iop_time_t last_keepalive_time; /*上次检查keepalive的时间*/

	_list_node_t * tcp_protocol_list_head;	/*use for advance tcp server model.*/
};


#define IOP_CB(base,iop,events)	{if(iop->iop_type != IOP_TYPE_FREE){if(0==(iop->evcb)(base, iop->id,events,iop->arg)){iop->last_dispatch_time = base->cur_time;}else{iop_del(base,iop->id);}}}

#ifdef __cplusplus
}
#endif

#endif


