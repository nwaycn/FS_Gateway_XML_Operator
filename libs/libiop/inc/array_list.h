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


#ifndef _ARRAY_LIST_H_
#define _ARRAY_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif


struct tag_list_node_t
{
	void *data;
	struct tag_list_node_t *next;
};
typedef struct tag_list_node_t _list_node_t;

_list_node_t *_list_node_new(void *data);
_list_node_t * _list_add_before(_list_node_t *node,_list_node_t *new_node);
_list_node_t * _list_add_after(_list_node_t *node,_list_node_t *new_node);





struct tagArrayListNode
{
 char is_free;
 int prev;  
 int next;
 int idx;
 void *item;        
};

typedef struct tagArrayListNode array_list_node_t;

struct tagArrayList
{
 int free_head; //空闲节点单链表头元素位置
 int free_tail; //空闲节点单链表尾元素位置
 int free_size;
 
 int used_list_head;        //已用节点又链表头元素位置,-1代表空
 int used_list_tail;        //已用节点又链表头元素位置,-1代表空
 int used_size;
 
 int total_size;                //总容量total_size = free_size + used_size
 array_list_node_t *node;    
 int auto_expand;
 
};

typedef struct tagArrayList array_list_t;

/*
*初始化
*size:容量   auto_expand:是否支持自动扩展。
*在不支持自动扩展的情况下， 最大容量是size
*/
array_list_t *array_list_init(array_list_t *p, int size, int auto_expand);
void array_list_free(array_list_t *p);
void array_list_clear(array_list_t *p);
int array_list_full(array_list_t *p);
int array_list_empty(array_list_t *p);
int array_list_size(array_list_t *p);
int array_list_capacity(array_list_t *p);
int array_list_add(array_list_t *p, void *node);
int array_list_del(array_list_t *p, int idx);
int array_list_get(array_list_t *p, int idx, void **ppnode);
int array_list_set(array_list_t *p, int idx, void *ppnode);
array_list_node_t * array_list_get_node(array_list_t *p, int idx);
array_list_node_t *array_list_free_begin(array_list_t *p);
//获取一个可用节点
array_list_node_t * array_list_fetch(array_list_t *p);
array_list_node_t *array_list_begin(array_list_t *p);
array_list_node_t *array_list_next(array_list_t *p,array_list_node_t *pn);
void array_list_print(array_list_t *p);

#ifdef __cplusplus
}
#endif

#endif
