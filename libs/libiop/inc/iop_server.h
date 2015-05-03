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
#ifndef _IOP_SERVER_H_
#define _IOP_SERVER_H_
#include <time.h>
#include "iop_config.h"
#include "iop_def.h"
#include "iop.h"
#include "iop_util.h"
#include "array_list.h"
#include "iop_buf.h"
#include "iop_list.h"
#include "iop_hashmap.h"
#include "iop_thread.h"

#ifdef __cplusplus
extern "C" {
#endif




/*tcp连接事件回调函数*/
typedef void (*iop_cb)(iop_base_t *,int,void *);
void iop_default_cb(iop_base_t *base, int id, void *arg);



/*
*   返回-1代表要删除事件,返回0代表不删除
*/
typedef int (*iop_err_cb)(iop_base_t *,int,int,void *);

int iop_default_err_cb(iop_base_t *base, int id, int err, void *arg);


/************************************
*协议解析器，
* parameters:
*	char *buf:数据
*	int len:数据长度
*return:
*	返回0代表还要收更多数据以代解析，-1代表协议错误，>0代表解析成功一个数据包
***********************************/
typedef int (*iop_parser)(char *, int);
int iop_default_parser(char *buf, int len);


/*
*数据处理器
*parameters:
*	base:iop_base_t 指针
*	id:iop对象的id
*	buf:数据包起始点
*	len:数据包长度
*	arg:自带的参数
*return:
	-1: 代表要关闭连接,0代表正常
*/
typedef int (*iop_processor)(iop_base_t *,int,char *,int,void *);


struct tag_iop_tcp_server_arg_t
{
	char host[64];
	unsigned short port;
	iop_cb on_connect;
	iop_cb on_destroy;
	iop_parser parser;
	iop_processor processor;
	iop_err_cb on_error;
	int timeout;
};
typedef struct tag_iop_tcp_server_arg_t iop_tcp_server_arg_t;


/***********************************
*function:
*	创建新的tcp server并加入iop_base中。
*parameters:
*	base:iop_base_t指针
*	host:服务器ip
*	port:服务器port
*	parser:协议解析器
*	processor:数据处理器
*	on_connect:当连接创建时的回调
*	on_destory: 退出事件的回调
*	on_keepalive_tiimeout:超时事件的回调
*	keepalive_tiimeout,keepalive超时
*Return:
*	成功返回>=0的id，失败返回-1.
*/
int iop_add_tcp_server(iop_base_t *base, const char *host, unsigned short port,
					   iop_parser parser, iop_processor processor,
					   iop_cb on_connect,iop_cb on_destroy,
					   iop_err_cb on_error, int keepalive_timeout);


int iop_add_t_server(iop_base_t *base, const char *host, unsigned short port,
					   iop_parser parser, iop_processor processor, int keepalive_timeout);




/*
*function:
*	异步连接
*parameters:
*	base:iop_base_t指针
*	host:服务器host
*	port:服务器端口
*    timeout:超时时间
*    on_timeout:超时回调
*    arg:自定义参数
*Return:
*     -1:失败
*      0,连接成功或正在尝试连接
*      不管什么情况，最终都将回调conn_cb.
*/
int iop_asyn_connect(iop_base_t *base, const char *host, unsigned short port,int timeout,iop_event_cb conn_cb, void *arg);


//int iop_http_get(const char *url, iop_event_cb cb,int timeout, void *arg);

#ifdef __cplusplus
}
#endif

#endif
