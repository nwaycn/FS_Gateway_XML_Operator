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
#ifndef _IOP_OBJ_POOL_H_
#define _IOP_OBJ_POOL_H_
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif



struct tag_iop_obj_pool
{
    int obj_size;           //对象大小
    int pool_size;          //对象池容量
    int ava_num;            //可用对象数
    void **obj_pool;    //对象池
};
typedef struct tag_iop_obj_pool iop_obj_pool_t;

#define IOP_OBJ_POOL_INIT(pool, size,osize) do{\
            iop_obj_pool_t *__p = (iop_obj_pool_t *)malloc(sizeof(iop_obj_pool_t));    \
            if(!__p){(pool) = __p;break;} \
            __p->obj_pool = (void **)malloc(sizeof(void *)*size); \
            if(!__p->obj_pool){free(__p);(pool) = __p;break;} \
            __p->pool_size = size; \
            __p->ava_num =0;   \
            __p->obj_size = osize;    \
            (pool) = __p;   \
        }while(0)



#define IOP_OBJ_POOL_DESTROY(pool) do{\
            if((pool)->obj_pool){   \
                int __i = 0; for(; __i < (pool)->ava_num; __i++){free(((pool)->obj_pool)[__i]);}   \
                free((pool)->obj_pool);} \
            free(pool); \
        }while(0)

#define IOP_OBJ_POOL_FOR_EACH(pool,func,arg) do{\
            if((pool)->obj_pool){   \
                int __i = 0; for(; __i < (pool)->ava_num; __i++){\
                    (func)(arg,((pool)->obj_pool)[__i]);}   }\
        }while(0)

//从对象池中取一个对象
#define IOP_OBJ_POOL_NEW(Type, obj, pool) do{ \
            if((pool)->ava_num > 0){--((pool)->ava_num); (obj) = (Type)(((pool)->obj_pool)[(pool)->ava_num]);}   \
            else{(obj) = (Type)malloc((pool)->obj_size);if(obj){memset(obj,0,(pool)->obj_size);}}   \
        }while(0)        

#define IOP_OBJ_POOL_FREE(obj, pool) do{ \
            if((pool)->ava_num < (pool)->pool_size){ \
                ((pool)->obj_pool)[(pool)->ava_num] = obj;  \
                ++((pool)->ava_num);}   \
            else{free(obj);}   \
        }while(0)        


#ifdef __cplusplus
}
#endif

#endif


