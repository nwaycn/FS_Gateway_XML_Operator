/*
 * Copyright (c) 2011-2014 zhangjianlin ÕÅ¼öÁÖ
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

 #ifndef _IOP_LIST_H_
#define _IOP_LIST_H_

#ifdef __cplusplus
extern "C" {
#endif

struct tag_iop_list_node_t
{
    struct tag_iop_list_node_t *next;
    void *data;
};
typedef struct tag_iop_list_node_t iop_list_node_t;

struct tag_iop_list_head
{
    iop_list_node_t *head;
    iop_list_node_t *tail;
    iop_list_node_t *free_node;
};
typedef struct tag_iop_list_head iop_list_head_t;

#define IOP_LIST_INIT(list) do{(list)->head = (iop_list_node_t *)0;     \
                            (list)->tail = (iop_list_node_t *)0;    \
                            (list)->free_node = (iop_list_node_t *)0;    \
                        }while(0)
                        
#define IOP_LIST_EMPTY(list) ((list)->head == (iop_list_node_t *)0)

#define IOP_LIST_SIZE(n,list) do{ \
                            n = 0;  \
                            iop_list_node_t *__h = (list)->head;  \
                            while(__h){n++; __h = __h->next;} \
                        }while(0)
                        
#define IOP_LIST_NEW_NODE(tlist,node)  do{ \
                            if((tlist)->free_node){  \
                                node = (tlist)->free_node;  \
                                (tlist)->free_node = node->next; \
                            }else{  \
                                node = (iop_list_node_t *)malloc(sizeof(iop_list_node_t));  \
                            }   \
                        }while(0)

#define IOP_LIST_PUSH_BACK(r, list, item)  do{ \
                            iop_list_node_t *__node = (iop_list_node_t *)0;   \
                            IOP_LIST_NEW_NODE(list,__node);   \
                            if(__node){   \
                                r = 0;  \
                                __node->data = item;  \
                                __node->next = (iop_list_node_t *)0;  \
                                if(!(list)->head){(list)->head = __node; (list)->tail = __node;}    \
                                else{(list)->tail->next = __node; (list)->tail = __node;}  \
                            }else{r = -1;}   \
                        }while(0)

#define IOP_LIST_PUSH_FRONT(r, list, item)  do{ \
                            iop_list_node_t *__node = (iop_list_node_t *)0;   \
                            IOP_LIST_NEW_NODE(list,__node);   \
                            if(__node){   \
                                r = 0;  \
                                __node->data = item;  \
                                __node->next = (list)->head;  \
                                (list)->head = __node;    \
                                if(!(list)->tail){(list)->tail = __node;}    \
                            }else{r = -1;}  \
                        }while(0)

#define IOP_LIST_INSERT(list,cur, node)  do{ \
                            (node)->next = (cur)->next;  \
                            (cur)->next = node; \
                            if((cur) == (list)->tail){(list)->tail = node;} \
                        }while(0)

#define IOP_LIST_DEL_FRONT(list, node)  do{ \
                (list)->head = (node)->next;  \
                (node)->next = (list)->free_node; \
                (list)->free_node = (node);}while(0)  


#define IOP_LIST_DEL(list, node, prev_node)  do{ \
                if(!prev_node){(list)->head = (node)->next;}  \
                else{(prev_node)->next = (node)->next;} \
                (node)->next = (list)->free_node; \
                (list)->free_node = (node);}while(0)  
    
#define IOP_LIST_POP_FRONT(r,list, item)  do{ \
                            iop_list_node_t *__node = (list)->head;  \
                            if(__node){   \
                                r = 0;  \
                                item = __node->data;  \
                                (list)->head = __node->next;    \
                                if(!((list)->head)){(list)->tail = (list)->head;}   \
                                __node->next = (list)->free_node; \
                                (list)->free_node = __node;   \
                            }else{r=-1; item = (void *)0;} \
                        }while(0)

//int func(void *data)   or  bool func(void *data)
#define IOP_LIST_ERASE(list, func)   do{ \
                            iop_list_node_t *__tmp = (iop_list_node_t *)0;   \
                            iop_list_node_t *__prev = (iop_list_node_t *)0;   \
                            iop_list_node_t *__cur = (list)->head;  \
                            while(__cur){ \
                                if(func(__cur->data)){    \
                                    if(!__prev){(list)->head = __cur->next;if(!(__cur->next)){(list)->tail = (iop_list_node_t *)0; } }   \
                                    else{   \
                                        __prev->next = __cur->next; \
                                        if(!(__cur->next)){(list)->tail = __prev;}  \
                                    }  \
                                    __tmp = __cur;  \
                                    __cur = __cur->next;    \
                                    __tmp->next = (list)->free_node;  \
                                    (list)->free_node = __tmp;    \
                                }   \
                                else{__prev = __cur; __cur = __cur->next;}  \
                            } \
                        }while(0)

//void func(void *arg,void *item);
#define IOP_LIST_FOR_EACH(list, func, arg) do{ \
                            iop_list_node_t *__h = (list)->head;  \
                            while(__h){func(arg,__h->data); __h = __h->next;} \
                        }while(0)

#define IOP_LIST_CLEAR(list) do{ \
                            iop_list_node_t *__h = (list)->head;  \
                            iop_list_node_t *__tmp = __h;  \
                            while(__h){__tmp = __h; __h = __h->next;free(__tmp);} \
                            __h = (list)->free_node;  \
                            while(__h){__tmp = __h; __h = __h->next; free(__tmp);}  \
                            (list)->head = (iop_list_node_t *)0;     \
                            (list)->tail = (iop_list_node_t *)0;    \
                            (list)->free_node = (iop_list_node_t *)0;    \
                        }while(0)

#define IOP_LIST_FREE(list, free_func,arg) do{ \
                            iop_list_node_t *__h = (list)->head;  \
                            iop_list_node_t *__tmp = __h;  \
                            while(__h){free_func(arg,__h->data); __tmp = __h; __h = __h->next;free(__tmp);} \
                            __h = (list)->free_node;  \
                            while(__h){__tmp = __h; __h = __h->next; free(__tmp);}  \
                            (list)->head = (iop_list_node_t *)0;     \
                            (list)->tail = (iop_list_node_t *)0;    \
                            (list)->free_node = (iop_list_node_t *)0;    \
                        }while(0)
                                            
#ifdef __cplusplus
}
#endif

#endif

