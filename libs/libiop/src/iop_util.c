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
#include "../inc/iop_util.h"


io_handle_t iop_socket_datagram();                                                


int iop_close_handle(io_handle_t h)
{
    int r = 0;
#ifndef WIN32
    do{r = close(h);}while( (r == -1) && (errno == EINTR));
#else
    r = closesocket(h);
#endif
    return r;
}

int iop_read(io_handle_t h, void *buf, unsigned int count)
{
    int n = 0;
    do{
    #ifdef WIN32
        n = recv(h, buf, count,0);
    #else
        n = read(h,buf, count);
    #endif
    }while((n < 0) && (socket_errno == EINTR));
    return n;    
}

int iop_write(io_handle_t h, const void *buf, unsigned int count)
{
    int n = 0;
    do{
    #ifdef WIN32
        n = send(h, buf, count,0);
    #else
        n = write(h,buf, count);
    #endif
    }while((n < 0) && (socket_errno == EINTR));
    return n;    
}



int iop_readv(io_handle_t h, iov_type *vec, int count)
{
    int n = 0;
#ifdef WIN32
       DWORD bytesRead = 0;
       DWORD flags = 0;
    if (WSARecv(h, vec, count, &bytesRead, &flags, NULL, NULL)) {
        /* The read failed. It might be a close,
         * or it might be an error. */
        if (WSAGetLastError() == WSAECONNABORTED)
            n = 0;
        else
            n = -1;
    } else
        n = bytesRead;

#else
    do{
        n = readv(h,vec, count);
    }while((n < 0) && (socket_errno == EINTR));

#endif
    return n;
}

int iop_writev(io_handle_t h, const iov_type *iov, int count)    
{
    int n = 0;
#ifdef WIN32
    DWORD bytesSent=0;
    if (WSASend(h, (LPWSABUF)iov, count, &bytesSent, 0, NULL, NULL))
        n = -1;
    else
        n = bytesSent;
#else
    do{
        n = writev(h,iov, count);
    }while((n < 0) && (socket_errno == EINTR));

#endif
    return n;
}

int open_dev_null(int fd) {
    int tmpfd=-1;
#ifdef WIN32

#else
    close(fd);
    tmpfd = open("/dev/null", O_RDWR);
    if (tmpfd != -1 && tmpfd != fd) {
        dup2(tmpfd, fd);
        close(tmpfd);
    }
#endif

    return (tmpfd != -1) ? 0 : -1;
}




int iop_set_nonblock(io_handle_t h)
{
#ifdef WIN32
    u_long mode = 1;
    return ioctlsocket(h,FIONBIO,&mode);
#else
    int val = fcntl(h, F_GETFL, 0);
    if (val == -1){return -1;}
    if (val & O_NONBLOCK){ return 0; }
    return fcntl(h, F_SETFL, val | O_NONBLOCK | O_NDELAY);
#endif    
}

int iop_set_block(io_handle_t h)
{
#ifdef WIN32
    u_long mode = 0;
    return ioctlsocket(h,FIONBIO,&mode);
#else
    int val = fcntl(h, F_GETFL, 0);
    if (val == -1){return -1;}
    if (val & O_NONBLOCK){ 
        return fcntl(h, F_SETFL, (val & ~O_NONBLOCK) | O_NDELAY);
    }
    return 0;
#endif    
}


int iop_sock_reuseaddr(io_handle_t h)
{
    int optval = 1;
    unsigned optlen = sizeof(optval);
    int ret = setsockopt(h, SOL_SOCKET, SO_REUSEADDR, (const char *)&optval, optlen);
    return ret;
}


int iop_set_sock_recv_timeo(io_handle_t sock, int ms)
{
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    return setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
}

int iop_set_sock_snd_timeo(io_handle_t sock, int ms)
{
    struct timeval tv;
    tv.tv_sec = ms / 1000;
    tv.tv_usec = (ms % 1000) * 1000;
    return setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv)); 
}



int iop_sock_bind(io_handle_t sock, const char *ip, const unsigned short port)
{
    struct sockaddr_in inaddr;    
    memset (&inaddr, 0,sizeof (struct sockaddr_in));
    inaddr.sin_family = AF_INET;        //ipv4协议族    
    inaddr.sin_port = htons (port);    
       if((!ip) || (ip[0] == 0) || (strcmp(ip, "0.0.0.0") == 0))
    {
        inaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    }
    else
    {
           inaddr.sin_addr.s_addr = inet_addr(ip);
    }
    return bind(sock, (struct sockaddr*)&inaddr, sizeof(struct sockaddr));
}

io_handle_t iop_socket_stream()
{
    return socket(PF_INET, SOCK_STREAM, 0);
}

io_handle_t iop_socket_datagram()
{
    return socket(PF_INET, SOCK_DGRAM, 0);
}

io_handle_t iop_tcp_server(const char *host, unsigned short port)
{
	int r = -1;
	int ret = -1;
    io_handle_t sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == INVALID_HANDLE)
    {
        return INVALID_HANDLE;
    }

    iop_sock_reuseaddr(sock);
    ret = iop_sock_bind(sock, host, port);
    if(ret != 0)
    {
        iop_close_handle(sock);
        return INVALID_HANDLE;
    }
    r = listen(sock, 32);
    if(r != 0)
    {
        iop_close_handle(sock);
        return INVALID_HANDLE;
    }
    return sock;
}


io_handle_t iop_udp_server(const char *host, unsigned short port)
{
    int ret = -1;
    io_handle_t sock = socket(PF_INET, SOCK_DGRAM, 0);
    if(sock == INVALID_HANDLE)
    {
        return sock;
    }
    iop_sock_reuseaddr(sock);
    ret = iop_sock_bind(sock, host, port);
    if(ret != 0)
    {
        iop_close_handle(sock);
        return INVALID_HANDLE;
    }
    return sock;
}


int iop_connect_timeo(io_handle_t sock, const iop_sockaddr_in *addr, int timeout_usec)
{
	int n = 0;
	int error;
    struct timeval timeout;
    fd_set rset, wset;
	int ret = -1;
    iop_socklen_t addr_len = sizeof(iop_sockaddr_in);
    int retval = 0;
    if (timeout_usec < 0) // 阻塞
    {
        return connect(sock, (struct sockaddr *)addr, addr_len);
    }
       if(iop_set_nonblock(sock) != 0)
       {
            return -1;
       }

        ret = retval= connect(sock, (struct sockaddr *) addr, addr_len);
        if(ret < 0 && socket_errno != EINPROGRESS)
        {
            return -1;
        }

        if(retval == 0)
        {
            return 0;
        }

        if(timeout_usec == 0)   //timeout
        {
            return -1;
        }
        FD_ZERO(&rset);
        FD_SET(sock, &rset);
        FD_ZERO(&wset);
        FD_SET(sock, &wset);
        MAKE_TIMEVAL(timeout, timeout_usec);
        n = select(sock + 1, &rset, &wset, NULL, &timeout);
        if( n == 0) //timeout
        {
            return -1;
        }

        error = 0;
        if(FD_ISSET(sock, &rset) || FD_ISSET(sock, &wset))
        {
            iop_socklen_t len = sizeof(error);
            if(getsockopt(sock, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0)
            {
                return -1;
            }

            if(error)
            {
                return -1;
            }
            return 0;
    }
    //not ready...
    return -1;
}


int iop_connect(io_handle_t sock, const char *ip, unsigned short port)
{
    iop_sockaddr_in    addr;
    iop_socklen_t addrlen = sizeof(addr);
    SOCKADDR_IN(addr, ip, port);
    return  connect(sock,  (const  iop_sockaddr *)&addr, addrlen);
}


int iop_do_connect(io_handle_t sock, const iop_sockaddr_in *addr)
{
      iop_socklen_t addrlen = sizeof(addr);
      return  connect(sock,  (const iop_sockaddr *)addr, addrlen);
}

io_handle_t iop_accept(io_handle_t sock, iop_sockaddr *addr, iop_socklen_t *len)
{
    return accept(sock, addr, len);
}


int iop_sendto(io_handle_t s, const void *buf, unsigned int len, int flags, const iop_sockaddr *to, iop_socklen_t tolen)
{
    return sendto(s,buf, len,flags, to,tolen);
}

int iop_recvfrom(io_handle_t s, void *buf, unsigned int len, int flags, iop_sockaddr *from, iop_socklen_t *fromlen)
{
    return recvfrom(s,buf,len,flags,from,fromlen);
}


iop_uint16_t iop_htons(iop_uint16_t x)
{
    return htons(x);
}

iop_uint32_t iop_htonl(iop_uint32_t x)
{
    return htonl(x);
}

iop_uint16_t iop_ntohs(iop_uint16_t x)
{
    return ntohs(x);
}

iop_uint32_t iop_ntohl(iop_uint32_t x)
{
    return ntohl(x);
}

int iop_send_n(io_handle_t s, const char *buf, int len)
{
    int result, old_len = len;
    do {
        result = iop_write(s, buf, len);
        if (result == socket_error)
            return socket_error;
        len -= result;
        buf += result;
    } while (len);
    return old_len;
}


int iop_recv_n(io_handle_t s, char *buf, int len)
{
    int result, old_len = len;
    do {
        result = iop_read(s, buf, len);
	 if(result == 0){return 0;}
        if (result == socket_error)
            return socket_error;
        len -= result;
        buf += result;
    } while (len);
    return old_len;
}



void iop_usleep(int usec)
{
#ifdef WIN32
    Sleep(usec);
#else
    usleep(usec);
#endif
}

