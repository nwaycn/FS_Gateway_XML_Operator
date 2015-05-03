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

 #ifndef _IOP_HASHMAP_H_
#define _IOP_HASHMAP_H_

#include "iop_list.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned int (*iop_hasher_t)(void *);
typedef int (*iop_cmp_t)(void *,void *);

struct tag_iop_hashmap
{
    int key_type;
    unsigned int bucket_num;
    iop_list_head_t *buckets;
    unsigned int item_num;
    iop_hasher_t hasher;
    iop_cmp_t cmp;
};
typedef struct tag_iop_hashmap iop_hashmap_t;


struct tag_iop_hashmap_node
{
    void *key;
    void *val;
};
typedef struct tag_iop_hashmap_node iop_hashmap_node_t;

extern unsigned int iop_int_hasher(void *);
extern unsigned int iop_str_hasher(void *);

extern int iop_int_cmp(void *,void *);
extern int iop_str_cmp(void *, void *);

extern iop_hashmap_t *iop_hashmap_new(int bucket_num, iop_cmp_t cmp, iop_hasher_t hasher);
extern void iop_hashmap_free(iop_hashmap_t *hmap);
extern void iop_hashmap_destroy(iop_hashmap_t *hmap, void (*free_cb)(void *));

extern iop_hashmap_t *iop_hashmap_int_new(int bucket_num);
extern iop_hashmap_t *iop_hashmap_str_new(int bucket_num);

extern int iop_hashmap_size(iop_hashmap_t *hmap);
extern int iop_hashmap_put(iop_hashmap_t *hmap, void *key,int key_size, void *val, int val_size);

extern int iop_hashmap_get(iop_hashmap_t *hmap, void *key, void **val);
extern int iop_hashmap_del(iop_hashmap_t *hmap, void *key);


                                            
#ifdef __cplusplus
}
#endif

#endif


