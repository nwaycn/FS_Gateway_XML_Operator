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
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../inc/array_list.h"


_list_node_t *_list_node_new(void *data)
{
	_list_node_t *node = (_list_node_t *)malloc(sizeof(_list_node_t));
	if(!node){return 0;}
	node->data = data;
	node->next = 0;
	return node;
}

_list_node_t * _list_add_before(_list_node_t *node,_list_node_t *new_node)
{
	if(!new_node){return node;}
	new_node->next = node;
	return new_node;
}

_list_node_t * _list_add_after(_list_node_t *node,_list_node_t *new_node)
{
	if(!new_node){return node;}
	new_node->next = 0;
	if(node == 0){return new_node;}
	node->next = new_node;
	return new_node;
}



array_list_t * array_list_init(array_list_t *p, int size, int auto_expand)
{
    int i;

	if(!p)
    {
        p = (array_list_t *)malloc(sizeof(array_list_t));
    }
    if(!p)
    {
        return p;
    }
    if(size <= 0)
    {
        size = 1;
    }
    p->auto_expand = auto_expand;
    p->node = (array_list_node_t *)malloc(sizeof(array_list_node_t) * size);
    if(!(p->node)){free(p);return (array_list_t *)0;}
    p->total_size = size;
    for(i = 0; i < size; i++)
    {
        p->node[i].idx = i;
        p->node[i].item = (void *)0;
        p->node[i].next = i+1;
        p->node[i].prev = -1;
        p->node[i].is_free = 1;
    }
    p->free_head = 0;
    p->free_tail = size-1;
    p->node[size - 1].next = -1;
    p->free_size = size;
    p->used_size = 0;
    p->used_list_head = -1;
    p->used_list_tail = -1;
    return p;   
}

void array_list_clear(array_list_t *p)
{
    array_list_node_t *pn = array_list_begin(p);
    array_list_node_t *pnext;
    while(pn)
    {
        pnext = array_list_next(p, pn);
        array_list_del(p,pn->idx);
        pn = pnext;
    }
}

void array_list_free(array_list_t *p)
{
    if(p)
    {
      if(p->node){free(p->node);}
      p->node = (array_list_node_t *)0;
      p->free_head = -1;
      p->free_tail = -1;
      p->used_list_head = -1;
      p->total_size = 0;
      p->used_size = 0;
      p->free_size = 0;
    }
}


int array_list_full(array_list_t *p)
{
   return (p->free_size == 0);   
}

int array_list_empty(array_list_t *p)
{
   return p->used_size == 0;   
}

int array_list_size(array_list_t *p)
{
  return p->used_size;   
}

int array_list_capacity(array_list_t *p)
{
  return p->total_size;   
}


int array_list_expand(array_list_t *p)
{
	int new_size;
    int i = 0;
	array_list_node_t *tmp_node;
	if(p->free_size > 0){return 0;}
    new_size = (p->total_size)*3/2 + 1;
    tmp_node = (array_list_node_t *)malloc( sizeof(array_list_node_t)*new_size);

    if(!tmp_node)
    {
        return -1;
    }
    memcpy(tmp_node, p->node, sizeof(array_list_node_t) * p->total_size);
    free(p->node);
    
    p->node = tmp_node;
    for(i=p->total_size; i < new_size; i++)
    {
        tmp_node[i].idx = i;
        tmp_node[i].item = (void *)0;
        tmp_node[i].next = i+1;
        tmp_node[i].prev = -1;
        tmp_node[i].is_free = 1;
    }
    p->free_head = p->total_size;
    p->free_tail = new_size-1;
    p->node[new_size - 1].next = -1;
    p->free_size = new_size - p->total_size;    
    p->total_size = new_size;
    return 0;
}

array_list_node_t * array_list_fetch(array_list_t *p)
{
	int idx;
    //如果空闲链表为空，则返回失败
    if(p->free_size <= 0)
    {
        if(p->auto_expand == 1)
        {
            array_list_expand(p);
        }
        if(p->free_size <= 0)
        {
            return (array_list_node_t *)0;
        }
    }
    idx = p->free_head;
    p->free_head = p->node[idx].next;
    if(p->free_head == -1){p->free_tail = -1;}
    //p->node[idx].item = node;
    p->node[idx].is_free = 0;
    p->node[idx].next = -1;

    ++(p->used_size);
    --(p->free_size);

    if(p->used_list_head == -1)
    {
        p->used_list_head = idx;
        p->used_list_tail = idx;
        p->node[idx].prev = -1;
    }
    else
    {
        p->node[p->used_list_tail].next = idx;
        p->node[idx].prev = p->used_list_tail;
        p->used_list_tail = idx;
    }
    return p->node+idx;  
}

int array_list_add(array_list_t *p, void *node)
{
	int idx;
    //如果空闲链表为空，则返回失败
    if(p->free_size <= 0)
    {
        if(p->auto_expand == 0)
        {
            return -1;
        }
        array_list_expand(p);
        if(p->free_size <= 0)
        {
            return -1;
        }
    }

    //使用空闲链表的第一个元素
    idx = p->free_head;
    p->free_head = p->node[idx].next;
    if(p->free_head == -1){p->free_tail = -1;}

    p->node[idx].item = node;
    p->node[idx].is_free = 0;
    p->node[idx].next = -1;
    ++(p->used_size);
    --(p->free_size);
    //新元素插入到已用列表的尾部


    if(p->used_list_head == -1)
    {
        p->used_list_head = idx;
        p->used_list_tail = idx;
        p->node[idx].prev = -1;
    }
    else
    {
        p->node[p->used_list_tail].next = idx;
        p->node[idx].prev = p->used_list_tail;
        p->used_list_tail = idx;
    }
    return idx;  
}

int array_list_del(array_list_t *p, int idx)
{
	array_list_node_t *pn;
    if((idx < 0) || (idx >= p->total_size)){return -1;}
    pn = (p->node)+idx;
    if(pn->is_free == 1){return 0;}
    pn->is_free = 1;
    //从已用元素双向列表中删除
    if(p->used_size == 1)
    {
        p->used_list_head = -1;
        p->used_list_tail= -1;
    }
    else
    {
        if(p->node[idx].prev == -1)
        {
            p->used_list_head = p->node[idx].next;
            p->node[p->node[idx].next].prev = -1;
        }
        else
        {
            p->node[p->node[idx].prev].next = p->node[idx].next;
            if(p->node[idx].next == -1)
            {
                p->used_list_tail = p->node[idx].prev;
            }
            else
            {
                p->node[p->node[idx].next].prev = p->node[idx].prev;
            }
        }
    }
    //将删除元素加到空闲列表的尾部
    if(p->free_tail == -1)
    {
        p->free_head = idx;
    }
    else
    {
        p->node[p->free_tail].next = idx;
    }
    p->free_tail = idx;
    p->node[idx].next = -1;   
    pn->prev = -1;
    pn->next = -1;
    --(p->used_size);
    ++(p->free_size);
    
    return 0;        
}

int array_list_get(array_list_t *p, int idx, void **ppnode)
{
    if(idx < 0 || idx >= p->total_size){*ppnode = 0; return -1;}
    if(p->node[idx].is_free == 1){*ppnode = 0; return -1;}
    *ppnode = p->node[idx].item;
    return 0;    
}

array_list_node_t * array_list_get_node(array_list_t *p, int idx)
{
    if((idx < 0) || (idx >= p->total_size))
    { 
        return (array_list_node_t *)0;
    }
    if(p->node[idx].is_free == 1)
    {
        return (array_list_node_t *)0;
    }
    return p->node+idx;
}


int array_list_set(array_list_t *p, int idx, void *pnode)
{
    if(idx < 0 || idx >= p->total_size){ return -1;}
    p->node[idx].item = pnode;
    return 0;    
}



array_list_node_t *array_list_free_begin(array_list_t *p)
{
    if(p->free_head != -1)
    {
        return p->node + p->free_head;
    }
    return (array_list_node_t *)0;
}

array_list_node_t *array_list_begin(array_list_t *p)
{
    if(p->used_list_head != -1)
    {
        return p->node+p->used_list_head;
    }
    return (array_list_node_t *)0;
}

array_list_node_t *array_list_next(array_list_t *p,array_list_node_t *pn)
{
    if(pn->next != -1)
    {
        return p->node + pn->next;
    }
    else
    {
        return (array_list_node_t *)0;
    }
}


void array_list_print(array_list_t *p)
{
    array_list_node_t *pn = array_list_begin(p);
    printf("array list size = %d\n",array_list_size(p));
    while(pn)
    {
        printf("node idx=%d. prev=%d,next=%d.\n", pn->idx, pn->prev, pn->next);
        pn = array_list_next(p, pn);
    }
}


