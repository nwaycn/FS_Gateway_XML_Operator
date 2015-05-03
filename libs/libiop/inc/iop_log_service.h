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
#ifndef _IOP_LOG_SERVICE_H_
#define _IOP_LOG_SERVICE_H_

#include <stdio.h>

#include "iop_config.h"
#include "iop_thread.h"

#ifdef __cplusplus
extern "C" {
#endif

//日志级别，默认支持  错误，信息，调试 三类
#define IOP_LOG_ALERT 9
#define IOP_LOG_ERR 10
#define IOP_LOG_INFO 11
#define IOP_LOG_DBG  12
#define IOP_LOG_ALL 100         //所有级别
#define IOP_LOG_NONE 0          //不写任何日志

#define IOP_MAX_LOG_PATH 1024
#define IOP_MAX_LOG_BUF_SIZE 1048576
struct tag_iop_log_service
{
    iop_lock_t lock;
    char path[IOP_MAX_LOG_PATH+1];
    char prefix[IOP_MAX_LOG_PATH+1];
    FILE *fp;
    char log_level;
    int m_day;
    iop_thread_t tid;
    char log_buf[IOP_MAX_LOG_BUF_SIZE];
    int log_buf_size;
    volatile int stop_flag;
};
typedef struct tag_iop_log_service iop_log_service_t;
extern iop_log_service_t *g_iop_log_service;

int iop_log_service_start(const char *path, const char *prefix,char level);
int iop_log_service_stop();
int iop_log_flush(iop_log_service_t *log);


int iop_log_bin(int level, const void *buf, unsigned int len);

int iop_log_fmt(void *buf, int len, int level, const char *fmt,...);





int LOG_DBG(const char *fmt,...);
int LOG_INF(const char *fmt,...);
int LOG_ERR(const char *fmt,...);
int LOG_ALT(const char *fmt,...);



#ifdef __cplusplus
}
#endif 		//__Cplusplus

#endif		

