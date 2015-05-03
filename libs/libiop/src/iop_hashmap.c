
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
#include <string.h>

#include "../inc/iop_hashmap.h"

#define KEY_TYPE_OTHER 0
#define KEY_TYPE_INT 1
#define KEY_TYPE_STR 2

unsigned int iop_int_hasher(void *int_key)
{
    unsigned int key = (unsigned int)(long)int_key;
    key += (key << 12);
    key ^= (key >> 22);
    key += (key << 4);
    key ^= (key >> 9);
    key += (key << 10);
    key ^= (key >> 2);
    key += (key << 7);
    key ^= (key >> 12);

    /* Knuth's Multiplicative Method */
    key = (key >> 3) * 2654435761;    
    return key;     
}

 
unsigned int iop_str_hasher(void *str_key)
{
    const char *str = (const char *)str_key;
    unsigned int hash = 0;
    while (*str)
    {
        // equivalent to: hash = 65599*hash + (*str++);
        hash = (*str++) + (hash << 6) + (hash << 16) - hash;
    }
    return (hash & 0x7FFFFFFF);
}

int iop_int_cmp(void *v1,void *v2)
{
    return ((long)v1 == (long)v2);
}

int iop_str_cmp(void *v1, void *v2)
{
    return (strcmp((const char *)v1, (const char *)v2)==0);
}

static int hashmap_bucket_size(int size)
{
    int table_size[] = {
        7, 13, 31, 61, 127, 251, 509, 1021, 2039, 4093, 8191,
        16381, 32749, 65521, 131071, 
        262143, 524287, 1048575, 2097151, 4194303, 8388607,
        16777211, 33554431, 67108863, 134217727, 268435455,
        536870911, 1073741823, 0};
    int *p = table_size;
    while(*p > 0)
    {
        if(*p >= size){return *p;}
        p++;
    }
    return size;
}
iop_hashmap_t *iop_hashmap_new(int bucket_num, iop_cmp_t cmp, iop_hasher_t hasher)
{
    iop_list_head_t *list_head;
    int i = 0;
    iop_hashmap_t *hm = (iop_hashmap_t *)malloc(sizeof(iop_hashmap_t));
    if(!hm){return hm;}
    bucket_num = hashmap_bucket_size(bucket_num);
    hm->key_type = KEY_TYPE_OTHER;
    hm->bucket_num = bucket_num;
    hm->item_num = 0;
    hm->hasher = hasher;
    hm->cmp = cmp;
    hm->buckets = (iop_list_head_t *)malloc(sizeof(iop_list_head_t)*bucket_num);
    if(!(hm->buckets))
    {
        free(hm);
        return (iop_hashmap_t *)0;
    }
    for(i=0; i < bucket_num; i++)
    {
        list_head = hm->buckets+i;
        IOP_LIST_INIT(list_head);
    }
    return hm;
}

void __hm_free(void *data, void *arg)
{
    void (*free_cb)(void *) = (void (*)(void *))(arg);
    iop_hashmap_t *hm = (iop_hashmap_t *)arg;
    iop_hashmap_node_t *node = (iop_hashmap_node_t *)data;
    if(hm->key_type != KEY_TYPE_INT)
    {
        free(node->key);
    }
    if(free_cb)
    {
        free_cb(node->val);
    }
    else
    {
        free(node->val);
    }
    free(node);
}


void iop_hashmap_destroy(iop_hashmap_t *hm, void (*free_cb)(void *))
{
    unsigned int i = 0;
    for(i=0; i < hm->bucket_num; i++)
    {
        IOP_LIST_FREE(hm->buckets+i, __hm_free, free_cb);
    }
    free(hm->buckets);
    free(hm);
}

void iop_hashmap_free(iop_hashmap_t *hm)
{
    iop_hashmap_destroy(hm,0);
}

iop_hashmap_t *iop_hashmap_int_new(int bucket_num)
{
    iop_hashmap_t *hm = iop_hashmap_new(bucket_num, iop_int_cmp, iop_int_hasher);
    if(hm)
    {
        hm->key_type = KEY_TYPE_INT;
    }
    return hm;
}


iop_hashmap_t *iop_hashmap_str_new(int bucket_num)
{
    iop_hashmap_t *hm = iop_hashmap_new(bucket_num, iop_str_cmp, iop_str_hasher);
    if(hm)
    {
        hm->key_type = KEY_TYPE_STR;
    }
    return hm;
}

int iop_hashmap_size(iop_hashmap_t *hmap)
{
    return hmap->item_num;
}

void iop_hashmap_node_free(iop_hashmap_t *hmap, iop_hashmap_node_t *node)
{
    if(!node){return;}
    if(hmap->key_type != KEY_TYPE_INT)
    {
        if(node->key){free(node->key);}
    }
    if(node->val){free(node->val);}
}

int iop_hashmap_put(iop_hashmap_t *hmap, void *key,int key_size, void *val, int val_size)
{
	iop_list_head_t *head;
	iop_list_node_t * __h;
	iop_hashmap_node_t *xnode;
    int r = 0;
	unsigned int khash;
	unsigned int idx;
	iop_hashmap_node_t *node = (iop_hashmap_node_t *)malloc(sizeof(iop_hashmap_node_t));
    if(!node){return -1;}
    node->key = 0;
    node->val = 0;
    if(hmap->key_type == KEY_TYPE_INT)
    {
        node->key = key;
    }
    else
    {
        node->key = malloc(key_size);
        if(!(node->key))
        {
            free(node);
            return -1;
        }
        memcpy(node->key, key, key_size);
    }
    node->val = malloc(val_size);
    if(!(node->val))
    {
        iop_hashmap_node_free(hmap,node);
        return -1;
    }
    memcpy(node->val, val, val_size);

   
    khash = (hmap->hasher)(key);
    idx = khash % hmap->bucket_num;
    head = hmap->buckets+idx;

    __h = head->head;  
    while(__h)
    {
        xnode = (iop_hashmap_node_t *)(__h->data);
        if((hmap->cmp)(key,xnode->key))
        {
            iop_hashmap_node_free(hmap, xnode);
            __h->data = node;
            return 0;
        } 
        __h = __h->next;
    }
    
    IOP_LIST_PUSH_BACK(r, head, node);
    if(r != 0)
    {
        iop_hashmap_node_free(hmap,node);
        return -1;
    }
    hmap->item_num += 1;
    return 0;
}

int iop_hashmap_get(iop_hashmap_t *hmap, void *key, void **val)

{
    unsigned int khash = (hmap->hasher)(key);
    unsigned int idx = khash % hmap->bucket_num;
    iop_list_head_t *head = hmap->buckets+idx;
    iop_hashmap_node_t *node;
    iop_list_node_t *__h = head->head;  
    while(__h)
    {
        node = (iop_hashmap_node_t *)(__h->data);
        if((hmap->cmp)(key,node->key))
        {
            *val = node->val;
            return 0;
        } 
        __h = __h->next;
    }
    return -1;    
}

int iop_hashmap_del(iop_hashmap_t *hmap, void *key)
{
    unsigned int khash = (hmap->hasher)(key);
    unsigned int idx = khash % hmap->bucket_num;
    iop_list_head_t *head = hmap->buckets+idx;
    iop_hashmap_node_t *node;
    iop_list_node_t *__h = head->head;  
    iop_list_node_t *prev = 0;
    while(__h)
    {
        node = (iop_hashmap_node_t *)(__h->data);
        if((hmap->cmp)(key,node->key))
        {
            iop_hashmap_node_free(hmap, node);
            IOP_LIST_DEL(head, __h, prev);
            if(hmap->item_num > 0)
            {
                hmap->item_num -= 1;
            }
            return 0;
        } 
        prev = __h;
        __h = __h->next;
    }
    return -1;    
}

