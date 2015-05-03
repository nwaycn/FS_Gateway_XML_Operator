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
#include "../inc/iop_config.h"
#include "../inc/iop_thread.h"

int iop_lock_init(iop_lock_t *lock)
{
    int r = 0;
#ifdef WIN32
    InitializeCriticalSection(lock);
#else
    r = pthread_mutex_init(lock, NULL);
#endif
    return r;
}

int iop_lock_destroy(iop_lock_t *lock)
{
    int r = 0;
#ifdef WIN32
    DeleteCriticalSection(lock);
#else
    r = pthread_mutex_destroy(lock);
#endif
    return r;
}

int iop_lock(iop_lock_t *lock)
{
    int r = 0;
#ifdef WIN32
    EnterCriticalSection(lock);
#else
    r = pthread_mutex_lock(lock);
#endif
    return r;
}


int iop_unlock(iop_lock_t *lock)
{
    int r = 0;
#ifdef WIN32
    LeaveCriticalSection(lock);
#else
    r = pthread_mutex_unlock(lock);
#endif
    return r;
}


int iop_thread_create(iop_thread_t *thread, thread_start_routine start_routine, void *arg, int stack_size)
{
    int r = 0;
#ifdef WIN32
    *thread = (iop_thread_t)_beginthreadex(0,stack_size,start_routine,arg,0,0);
    if(*thread == NULL)
    {
        r = -1;
    }
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, stack_size);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);      
    r = pthread_create(thread, &attr, start_routine, arg);
#endif
    return r;
}

int iop_thread_detach(iop_thread_t thread)
{
#ifdef WIN32
    return 0;
#else
    return pthread_detach(thread);
#endif
}

int iop_msleep(int n)
{
#ifdef WIN32
    Sleep(n);
#else
    usleep(n*1000);
#endif
    return 0;
}

int iop_thread_cancel(iop_thread_t t)
{
    int r = 0;
#ifdef WIN32
    TerminateThread(t,0);  
#else
    r = pthread_cancel(t);
#endif
    return r;
}


void iop_thread_exit(thread_ret_type exit_code)
{
#ifdef WIN32
    ExitThread(exit_code ); 
#else
    pthread_exit(exit_code);
#endif
}

