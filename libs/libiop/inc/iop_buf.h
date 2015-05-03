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
#ifndef _IOP_BUF_H_
#define _IOP_BUF_H_

#include <time.h>
#include "iop_obj_pool.h"
#ifdef __cplusplus
extern "C" {
#endif

struct tag_dbuf
{
    unsigned int capacity;
    unsigned int size;
    void *data;
};
typedef struct tag_dbuf dbuf_t;

dbuf_t *dbuf_new(int size);
void dbuf_free(dbuf_t *dbuf);
void dbuf_free_data(dbuf_t *dbuf);
void dbuf_init(dbuf_t *dbuf);
int dbuf_add_size(dbuf_t *dbuf, int size);
int dbuf_expand(dbuf_t *dbuf, unsigned int len);
int dbuf_push_back(dbuf_t *dbuf,const void *data, unsigned int len);
int dbuf_push_front(dbuf_t *dbuf,const void *data, unsigned int len);
int dbuf_pop_front(dbuf_t *dbuf,unsigned int len);
int dbuf_pop_back(dbuf_t *dbuf,unsigned int len);
void *dbuf_data_end(dbuf_t *dbuf);
int dbuf_ava_capacity(dbuf_t *dbuf);
void *dbuf_data_start(dbuf_t *dbuf);
int dbuf_size(dbuf_t *dbuf);
int dbuf_capacity(dbuf_t *dbuf);
void dbuf_set_size(dbuf_t *dbuf,int size);

struct tag_iop_buf
{
    iop_obj_pool_t *owner;  //data is come from owner. if null, data is malloc
    time_t time;
    void *arg;
    
    int max_size;
    int cur_size;
    void *data;
    //for udp dgram, remote address may be in the head of data.
};
typedef struct tag_iop_buf iop_buf_t;




struct tag_iop_buf_pool
{
    int min_size;   // 64
    int max_size;   //16384
    int factor;     //  4
    int pool_num;   //5
    // 64,256,1024, 4096,16384
    iop_obj_pool_t** obj_pool;      //实际的data
    iop_obj_pool_t *buf_pool;            //data容器
    int *block_map;
};
typedef struct tag_iop_buf_pool iop_buf_pool_t;


//64,4,5,64M
int iop_buf_pool_init(iop_buf_pool_t *bp,int min_size, int factor, int pool_num, int buf_size);
void iop_buf_pool_destroy(iop_buf_pool_t *bp);

iop_buf_t* iop_buf_pool_get(iop_buf_pool_t *bp, int size);
void iop_buf_pool_free(iop_buf_pool_t *bp, iop_buf_t *buf);

#ifdef __cplusplus
}
#endif
#endif
