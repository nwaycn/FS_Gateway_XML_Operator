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
#include "../inc/iop_iocp.h"

#ifndef _HAVE_IOCP_
int iop_init_iocp(void *iop_base, int maxev)
{
    return -1;
}
#else


/////START...
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#ifdef WIN32

#include "../inc/iop.h"


struct tag_iop_iocp_data {
	HANDLE iocp;
};
typedef struct tag_iop_iocp_data iop_iocp_data_t;



static void iocp_free(iop_base_t *base)
{
    iop_iocp_data_t *p = (iop_iocp_data_t *)(base->model_data);
    if(p)
    {
		CloseHandle(p->iocp);
        free(p);
    }
    base->model_data = (void *)0;
}

static int iocp_dispatch(iop_base_t * base, int timeout)
{
    iop_t *iop=0;
	int id = 0;
	int r = 0;
	iop_iocp_data_t *model_data = (iop_iocp_data_t *)(base->model_data);
    DWORD   BytesTransferred = 0;
	LPDWORD   PerHandleData;
    LPOVERLAPPED PerIOData;
	if (GetQueuedCompletionStatus(model_data->iocp,&BytesTransferred,(LPDWORD)&PerHandleData,(LPOVERLAPPED*)&PerIOData,timeout)== 0)
    {
        //printf("GetQueuedCompletionStatus failed with error%d/n",GetLastError());
        return 0;
    }
	id = (int)PerHandleData;



	base->cur_time = time(NULL);
    return 0;
}

static int iocp_add(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    iop_iocp_data_t *p = (iop_iocp_data_t *)(base->model_data);

	if(NULL == CreateIoCompletionPort((HANDLE)handle,p->iocp,(DWORD)id,(ULONG_PTR)0))
	{
		return -1;
	}
    return 0;
}


static int iocp_del(iop_base_t *base, int id,io_handle_t handle)
{
    return 0;
}

static int iocp_mod(iop_base_t *base, int id, io_handle_t handle, unsigned int events)
{
    return 0;
}



int iop_init_iocp(void *iop_base, int maxev)
{
	iop_base_t *base = (iop_base_t *)iop_base;
	iop_op_t *iop_op = 0;
	iop_iocp_data_t *model_data = 0;
    static int init = 0;
    if(init == 0)
    {
        WSADATA     wsadata;
        if(WSAStartup(MAKEWORD( 1, 1 ),&wsadata)!=0)
        {
                return   -1;
        }
        init = 1;
    }
    model_data = (iop_iocp_data_t *)(malloc(sizeof(iop_iocp_data_t)));
    if(!model_data)
    {
        return -1;
    }


    iop_op = &(base->op_imp);
    if(!iop_op)
    {
        free(model_data);
        return -1;
    }

	model_data->iocp =  CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if(NULL == model_data->iocp)
	{
		free(model_data);
		return -1;
	}

    iop_op->name = "iocp";
    iop_op->base_free = iocp_free;
    iop_op->base_dispatch = iocp_dispatch;
    iop_op->base_add = iocp_add;
    iop_op->base_del = iocp_del;
    iop_op->base_mod = iocp_mod;

    base->model_data = model_data;
    return 0;
}



#endif
#endif
