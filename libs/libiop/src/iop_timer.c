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

#include "../inc/iop_config.h"
#include "../inc/iop.h"
#include "../inc/iop_timer.h"

static void my_timer_free(iop_base_t *base)
{
    base->model_data = (void *)0;
}

static int my_timer_add(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    return 0;
}


static int my_timer_del(iop_base_t *base, int id,io_handle_t handle)
{
    return 0;
}

static int my_timer_mod(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    return 0;
}


static int timer_dispatch(iop_base_t * base, int timeout)
{
#ifdef WIN32    
    Sleep(timeout);
#else
    usleep(timeout);
#endif
    return 0;
}

int iop_init_timer(void *iop_base, int maxev)
{
    iop_base_t *base = (iop_base_t *)iop_base;
	iop_op_t *iop_op = &(base->op_imp);
    if(!iop_op)
    {
        return -1;
    }
    
    iop_op->name = "timer";
    iop_op->base_free = my_timer_free;
    iop_op->base_dispatch = timer_dispatch;
    iop_op->base_add = my_timer_add;
    iop_op->base_del = my_timer_del;
    iop_op->base_mod = my_timer_mod;
    
    base->model_data = (void *)0;
    return 0;
}

