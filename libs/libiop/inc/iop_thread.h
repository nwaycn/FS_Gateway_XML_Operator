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

 #ifndef _IOP_THREAD_H_
#define _IOP_THREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "iop_config.h"

#ifdef WIN32
    typedef unsigned int thread_ret_type;
    #define thread_func_call __stdcall
#else
    typedef void * thread_ret_type;
    #define thread_func_call
#endif


#ifdef WIN32
    #include <process.h>
    typedef CRITICAL_SECTION iop_lock_t;
    typedef unsigned int ( __stdcall *thread_start_routine)( void * );
    typedef HANDLE iop_thread_t;
#else
    #include <signal.h>
    #include <pthread.h>
    typedef pthread_mutex_t iop_lock_t;
    typedef void *(*thread_start_routine)( void * );
    typedef pthread_t iop_thread_t;
#endif

int iop_lock_init(iop_lock_t *lock);
int iop_lock_destroy(iop_lock_t *lock);
int iop_lock(iop_lock_t *lock);
int iop_unlock(iop_lock_t *lock);

int iop_thread_create(iop_thread_t *thread, thread_start_routine start_routine, void *arg, int stack_size);
int iop_thread_detach(iop_thread_t thread);
int iop_msleep(int n);
int iop_thread_cancel(iop_thread_t t);
void iop_thread_exit(thread_ret_type exit_code);

#ifdef __cplusplus
}
#endif

#endif
