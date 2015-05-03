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
#include <string.h>
#include "../inc/iop_util.h"
#include "../inc/iop_buf.h"


dbuf_t *dbuf_new(int size)
{
	dbuf_t *dbuf = 0;
	if(size < 1){size = 1;}
    dbuf = (dbuf_t *)malloc(sizeof(dbuf));
    if(!dbuf){return dbuf;}
    dbuf->data = malloc(size);
    if(!(dbuf->data)){free(dbuf); return (dbuf_t *)0;}
    dbuf->size = 0;
    dbuf->capacity = size;
    return dbuf;        
}

void dbuf_free_data(dbuf_t *dbuf)
{
    if(dbuf)
    {
        if(dbuf->data)
        {
            free(dbuf->data);
        }
        dbuf->data = (void *)0;
        dbuf->size = 0;
        dbuf->capacity = 0;
    }
}


void dbuf_free(dbuf_t *dbuf)
{
    if(dbuf)
    {
        if(dbuf->data)
        {
            free(dbuf->data);
        }
        free(dbuf);
    }
}

void dbuf_init(dbuf_t *dbuf)
{
    dbuf->capacity = 0;
    dbuf->data = (void *)0;
    dbuf->size = 0;
}

int dbuf_expand(dbuf_t *dbuf, unsigned int len)
{
    unsigned int new_len = dbuf->capacity + len;
    void *new_data = realloc(dbuf->data,new_len);
    if(new_data)
    {
        dbuf->data = new_data;
        dbuf->capacity = new_len;
        return 0;
    }
    return -1;
}


int dbuf_add_size(dbuf_t *dbuf, int size)
{
    dbuf->size += size;
    if(dbuf->size > dbuf->capacity)
    {
        dbuf->size = dbuf->capacity;
    }
    else
    {
        if(dbuf->size <= 0)
        {
            dbuf->size = 0;
            if(dbuf->capacity >= 64000)
            {
                dbuf->data = realloc(dbuf->data, 1024);
                if(dbuf->data)
                {
                    dbuf->capacity = 1024;
                }
                else
                {
                    dbuf->capacity = 0;
                }
            }
        }
    }
    return dbuf->size;
}


int dbuf_push_back(dbuf_t *dbuf, const void *data, unsigned int len)
{
	void *new_data = 0;
	unsigned int new_len = 0;
    if(dbuf->size + len < dbuf->capacity)
    {
        memcpy((char *)(dbuf->data) + dbuf->size, data,len);
        dbuf->size += len;
        return 0;
    }
    new_len = dbuf->capacity + len;
    new_data = realloc(dbuf->data,new_len);
    if(new_data)
    {
        dbuf->data = new_data;
        dbuf->capacity = new_len;
        memcpy((char *)(dbuf->data) + dbuf->size, data,len);
        dbuf->size += len;
        return 0;
    }
    return -1;
}

int dbuf_push_front(dbuf_t *dbuf,const void *data, unsigned int len)
{
	unsigned int new_len = 0;
	void *new_data = 0;
	if(dbuf->size + len < dbuf->capacity)
    {
        memmove((char *)(dbuf->data) + len, dbuf->data, dbuf->size);
        memcpy(dbuf->data, data,len);
        dbuf->size += len;
        return 0;
    }
    new_len = dbuf->capacity + len;
    new_data = malloc(new_len);
    if(new_data)
    {
        if(dbuf->data){free(dbuf->data);}
        memcpy(new_data, data, len);
        memcpy((char *)new_data + len, dbuf->data, dbuf->size);
        dbuf->capacity = new_len;
        dbuf->size += len;
        dbuf->data = new_data;
        return 0;
    }
    return -1;
}


int dbuf_pop_front(dbuf_t *dbuf,unsigned int len)
{
    if(len >= dbuf->size)
    {
        len = dbuf->size;
    }
    else
    {
        memmove(dbuf->data, (char *)(dbuf->data) + len, dbuf->size - len);
    }
    
    dbuf->size -= len;
    if(dbuf->size == 0)
    {
        if(dbuf->capacity >= 64000)
        {
            dbuf->data = realloc(dbuf->data, 4096);
            if(dbuf->data)
            {
                dbuf->capacity = 4096;
            }
            else
            {
                dbuf->capacity = 0;
            }
        }
    }
    return len;
}

int dbuf_pop_back(dbuf_t *dbuf,unsigned int len)
{
    if(len >= dbuf->size)
    {
        len = dbuf->size;
    }
   
    dbuf->size -= len;
    if(dbuf->size == 0)
    {
        if(dbuf->capacity >= 64000)
        {
            dbuf->data = realloc(dbuf->data, 1024);
            if(dbuf->data)
            {
                dbuf->capacity = 1024;
            }
            else
            {
                dbuf->capacity = 0;
            }
        }
    }
    return len;
}

void *dbuf_data_start(dbuf_t *dbuf)
{
    return dbuf->data;
}

int dbuf_size(dbuf_t *dbuf)
{
    return dbuf->size;
}

int dbuf_capacity(dbuf_t *dbuf)
{
    return dbuf->capacity;
}

void *dbuf_data_end(dbuf_t *dbuf)
{
    return (char *)(dbuf->data) + dbuf->size;
}

int dbuf_ava_capacity(dbuf_t *dbuf)
{
    return dbuf->capacity - dbuf->size;
}


void dbuf_set_size(dbuf_t *dbuf,int size)
{
    dbuf->size=size;
}



static void area_size(int i,int factor, int *s, int *e)
{
    int ts = 0;
    int te = 0;
    if(i == 0){*s = 0; *e = 1;return;}
    area_size(i-1,factor,&ts, &te);
    *s = te+1;
    *e = te * factor;
}

int iop_buf_pool_init(iop_buf_pool_t *bp,int min_size, int factor, int pool_num, int buf_size)
{
    iop_obj_pool_t *pool;
	int pool_size = 0;
    int buf_pool_size = 0;
    int i = 0;
    int chunck_size = 0;
    int obj_size = 0;
	int block_num = 0;    
	if(pool_num < 1){pool_num = 1;}
    if(min_size < 16){min_size = 16;}
    if(factor < 2){factor = 2;}
    chunck_size = buf_size / pool_num;
    obj_size = min_size;
    bp->obj_pool = (iop_obj_pool_t **)malloc(sizeof(iop_obj_pool_t *)*pool_num);
    if(!bp->obj_pool){return -1;}
    for(i=0; i < pool_num; i++)
    {
        bp->obj_pool[i] = (iop_obj_pool_t *)0;
    }
    
    for(i=0; i < pool_num; i++)
    {
        bp->max_size = obj_size;
        pool = (iop_obj_pool_t *)0;
        pool_size = chunck_size / obj_size;
        if(pool_size < 1){pool_size = 1;}
        buf_pool_size += pool_size;
        IOP_OBJ_POOL_INIT(pool, pool_size, obj_size);        
        if(pool)
        {
            bp->obj_pool[i] = pool;
        }
        else
        {
            iop_buf_pool_destroy(bp);
            return -1;
        }
        obj_size = obj_size*factor;   
    }
    pool = (iop_obj_pool_t *)0;
    bp->buf_pool = pool;
    IOP_OBJ_POOL_INIT(pool, buf_pool_size, sizeof(iop_buf_t));
    bp->buf_pool = pool;
    if(!pool)
    {
        iop_buf_pool_destroy(bp);
        return -1;
    }
    bp->factor = factor;
    bp->pool_num = pool_num;
    bp->min_size = min_size;

    block_num = bp->max_size/bp->min_size;
    bp->block_map = (int *)malloc(sizeof(int)*(block_num));
    if(!(bp->block_map))
    {
        iop_buf_pool_destroy(bp);
        return -1;
    }
    for( i = 0; i < pool_num; i++)
    {
        int s =0;
        int e = 0;
        int j = 0;
        area_size(i, factor, &s, &e);
        for(j = s; j < e; j++)
        {
            bp->block_map[j] = i;
        }
    }
    return 0;
}




void iop_buf_pool_destroy(iop_buf_pool_t *bp)
{
    int i = 0;
    for(; i < bp->pool_num; i++)
    {
        IOP_OBJ_POOL_DESTROY(bp->obj_pool[i]);
    }
    if(bp->obj_pool)
    {
        free(bp->obj_pool);
    }
    if(bp->buf_pool)
    {
        free(bp->buf_pool);
    }
    if(bp->block_map)
    {
        free(bp->block_map);
    }            
}


iop_buf_t* iop_buf_pool_get(iop_buf_pool_t *bp, int size)
{
	iop_obj_pool_t *op = 0;
	int idx;
	int block_num = 0;
	iop_buf_t *buf = (iop_buf_t *)0;   
    IOP_OBJ_POOL_NEW(iop_buf_t *, buf, bp->buf_pool);
    if(!buf){return buf;}
    buf->time = 0;
    if(size > bp->max_size)
    {
        buf->owner = (iop_obj_pool_t *)0;
        buf->cur_size = 0;
        buf->data = malloc(size);
        if(!(buf->data))
        {
            buf->max_size = 0;
            IOP_OBJ_POOL_FREE(buf, bp->buf_pool);
            return (iop_buf_t *)0;
        }
        buf->max_size = size;
        return buf;        
    }

    block_num = size / bp->min_size;
    if(block_num * bp->min_size < size){++block_num;}
    idx = bp->block_map[block_num];
    op = bp->obj_pool[idx];
    
    buf->cur_size = 0;
    IOP_OBJ_POOL_NEW(void *, buf->data, op);
    if(buf->data)
    {
        buf->max_size = op->obj_size;
        buf->owner = op;
    }
    else
    {
        buf->max_size = 0;
        buf->owner = (iop_obj_pool_t *)0;
        IOP_OBJ_POOL_FREE(buf, bp->buf_pool);
        buf = (iop_buf_t *)0;   
    }

    return buf;
}



void iop_buf_pool_free(iop_buf_pool_t *bp, iop_buf_t *buf)
{
    if(buf->owner)
    {
        IOP_OBJ_POOL_FREE(buf->data, buf->owner);
    }
    else    
    {
        free(buf->data);
    }
    IOP_OBJ_POOL_FREE(buf, bp->buf_pool);
}

