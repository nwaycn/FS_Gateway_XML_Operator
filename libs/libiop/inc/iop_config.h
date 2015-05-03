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
 #ifndef _IOP_CONFIG_H_
#define _IOP_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#ifndef _NO_SELECT_
    #define _HAVE_SELECT_
#endif
#define WIN32
#ifdef WIN32
#ifndef _NO_IOCP_
	#define _HAVE_IOCP_
#endif	
	#include <winsock2.h>
    #define WIN32_LEAN_AND_MEAN

    typedef SOCKET io_handle_t;
    #define INVALID_HANDLE INVALID_SOCKET
    #define socket_error     SOCKET_ERROR
    #define socket_errno	WSAGetLastError()
    #define SOCKET_EINTR	WSAEINTR
    #define SOCKET_EAGAIN	WSAEINPROGRESS
    #define SOCKET_EWOULDBLOCK WSAEWOULDBLOCK
#else

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
	typedef int io_handle_t;
#define INVALID_HANDLE -1
#define socket_error -1
#define socket_errno errno
#define SOCKET_EINTR	EINTR   
#define SOCKET_EAGAIN	EAGAIN
#define SOCKET_EWOULDBLOCK EWOULDBLOCK
#define SOCKET_EADDRINUSE EADDRINUSE
#ifndef _NO_EPOLL_
    #define _HAVE_EPOLL_
#endif
#ifndef _NO_POLL_
    #define _HAVE_POLL_
#endif

    

#endif

typedef unsigned char iop_uint8_t;
typedef unsigned short iop_uint16_t;
typedef unsigned int iop_uint32_t;
typedef unsigned long long iop_uint64_t;

typedef char iop_int8_t;
typedef short iop_int16_t;
typedef int iop_int32_t;
typedef long long iop_int64_t;

#define IOP_MAKE_UINT64(h,l) ((iop_uint64_t)(h)<<32|(l))
#define IOP_UINT64_HIGH(t) (t>>32)
#define IOP_UINT64_LOW(t) (t&(iop_uint64_t)0xFFFFFF)

    
#define IOP_OK                          0
#define IOP_ERR                  -1
#define IOP_ERR_NO_MEM          -2
#define IOP_ERR_FULL                -3
#define IOP_ERR_EMPTY             -4
#define IOP_ERR_NOT_FOUND    -5

#ifdef __cplusplus
}
#endif
#endif

