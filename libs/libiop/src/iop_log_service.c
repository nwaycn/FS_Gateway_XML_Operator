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
#include <time.h>
#include <stdarg.h>

#include "../inc/iop_log_service.h"

iop_log_service_t *g_iop_log_service = (iop_log_service_t *)0;

static int __iop_log_reopen(iop_log_service_t *log_service)
{
    int r = 0;
    struct tm t;
	time_t n;
    char file[3000];
	FILE *tmp_fp;
    if(strcmp(log_service->path,"stdout/")==0)
    {
        log_service->fp = stdout;
        return 0;
    }
    n = time(NULL);
    #ifdef WIN32
	localtime_s(&t,&n);
    #else
	t = *localtime(&n);
    #endif
    if( (log_service->fp) && (t.tm_mday == log_service->m_day))
    {
        return 0;
    }
#ifdef WIN32
	sprintf_s(file, sizeof(file)-1,"%s%s%04d%02d%02d.log", log_service->path,log_service->prefix, 1900+t.tm_year, 1+t.tm_mon, t.tm_mday); 
	tmp_fp = 0;
	fopen_s(&tmp_fp,file, "a");
#else
	snprintf(file, sizeof(file)-1,"%s%s%04d%02d%02d.log", log_service->path,log_service->prefix, 1900+t.tm_year, 1+t.tm_mon, t.tm_mday); 
	tmp_fp = fopen(file, "a");
#endif
    if(!tmp_fp)
    {
        r = -1;
    }
    else
    {
        if(log_service->fp){fclose(log_service->fp);}
        log_service->fp = tmp_fp;
    }
    log_service->m_day = t.tm_mday;
    return 0;
}


static void  __log_service_flush(iop_log_service_t *log)
{
    if(log->log_buf_size > 0)
    {
        if(log->fp)
        {
            fwrite(log->log_buf, log->log_buf_size, 1, log->fp);
            fflush(log->fp);
            log->log_buf_size = 0;
        }
    }
}

        

static thread_ret_type thread_func_call __log_service_start_routine(void *arg)
{
    thread_ret_type r = (thread_ret_type)0;
    iop_log_service_t *log = (iop_log_service_t *)arg;
    int nloop = 0;
    while(!(log->stop_flag))
    {
        ++nloop;
        iop_msleep(100);

        iop_lock(&(log->lock));
        __log_service_flush(log);

        if(nloop > 60)
        {
            __iop_log_reopen(log);
            nloop = 0;
        }
        iop_unlock(&(log->lock));

    }
    if(log->fp)
    {
        fclose(log->fp);
        log->fp = (FILE *)0;
    }
    iop_lock_destroy(&(log->lock));
    if(log == g_iop_log_service)
    {
        g_iop_log_service = (iop_log_service_t *)0;
    }
    free(log);
    return r;
}


int iop_log_service_start(const char *path, const char *prefix,char level)
{
	iop_log_service_t *log;
	if(g_iop_log_service)
    {
        return 0;
    }
    g_iop_log_service = (iop_log_service_t *)malloc(sizeof(iop_log_service_t));
    if(!g_iop_log_service)
    {
        return -1;
    }
    log = g_iop_log_service;
    log->log_level = level;
    log->log_buf_size = 0;
    log->m_day = -1;
    log->stop_flag = 0;
    memset((void *)(log->path), 0, IOP_MAX_LOG_PATH+1);
    memset((void *)(log->prefix), 0, IOP_MAX_LOG_PATH+1);
    
    if(path)
    {
        int path_len = strlen(path);
#ifdef WIN32
        strcpy_s((char *)(log->path),IOP_MAX_LOG_PATH-1,path);
#else
        strncpy((char *)(log->path), path,IOP_MAX_LOG_PATH);

#endif
		if(path_len == 0)
        {
            log->path[0] = '/';
            log->path[1] = 0;
        }
        else
        {
            if((log->path)[path_len-1] != '/')
            {
                log->path[path_len] = '/';
                log->path[path_len+1] = 0;
            }
        }
    }
    else
    {
#ifdef WIN32
		strcpy_s((char *)(log->path),IOP_MAX_LOG_PATH, "./");

#else
		strncpy((char *)(log->path), "./",IOP_MAX_LOG_PATH);
#endif
	}
    if(prefix)
    {
#ifdef WIN32
        strcpy_s((char *)(log->prefix), IOP_MAX_LOG_PATH,prefix);

#else
        strncpy((char *)(log->prefix), prefix,IOP_MAX_LOG_PATH);
#endif
	}
    log->fp = (FILE *)0;
    iop_lock_init(&(log->lock));
    __iop_log_reopen(log);
    iop_thread_create(&(log->tid), __log_service_start_routine, (void *)log, 1024*1024);
    return 0;
}





int iop_log_service_stop()
{
    if(!g_iop_log_service){return 0;}
    iop_log_flush(g_iop_log_service);
    iop_lock(&(g_iop_log_service->lock));
    if((g_iop_log_service->fp))
    {
        fclose(g_iop_log_service->fp);
        g_iop_log_service->fp
= 0;
    }
    iop_thread_cancel((g_iop_log_service->tid));
    iop_unlock(&(g_iop_log_service->lock));
    iop_lock_destroy(&(g_iop_log_service->lock));
    free(g_iop_log_service);
    g_iop_log_service = 0;
    return 0;
}


int iop_log_flush(iop_log_service_t *log)
{
    if(!log){fflush(stdout);return 0;}
    iop_lock(&(log->lock));
    {
        if(!(log->fp))
        {
            __iop_log_reopen(log);
        }
        if(log->fp)
        {
            if(log->log_buf_size > 0)
            {
                fwrite(log->log_buf, log->log_buf_size, 1, log->fp);
                log->log_buf_size = 0;
                fflush(log->fp);
            }
        }
    }
    iop_unlock(&(log->lock));
    return 0;
}



static int iop_log_write(iop_log_service_t *log,const void *buf, unsigned int len)
{
    int ret = 0;
    if(!log){fwrite(buf,len, 1, stdout);return 0;}
    iop_lock(&(log->lock));
    if(log->log_buf_size + len <= IOP_MAX_LOG_BUF_SIZE)
    {
        memcpy(log->log_buf + log->log_buf_size, buf, len);
        log->log_buf_size += len;
        ret = (int)len;
    }
    else
    {
        if(!(log->fp))
        {
            __iop_log_reopen(log);
        }
        if(log->fp)
        {
            if(log->log_buf_size > 0)
            {
                fwrite(log->log_buf, log->log_buf_size, 1, log->fp);
                log->log_buf_size = 0;
            }
            fwrite(buf, len,1, log->fp);
            ret = (int)len;
        }
    }
    iop_unlock(&(log->lock));
    return ret;
}


int iop_log_bin(int level, const void *buf, unsigned int len)
{
    if(g_iop_log_service && (level > g_iop_log_service->log_level)){return 0;}    
    return iop_log_write(g_iop_log_service,buf,len);
}





int LOG_DBG(const char *fmt,...)
{
	char buf[102400];
	int len = sizeof(buf)-1;
	int xlen = 0;
    iop_log_service_t *log = g_iop_log_service;
    struct tm t;
	int level = IOP_LOG_DBG;
    time_t n ;
	int r;
	if(log && (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;
	n= time(NULL);
	#ifdef WIN32
		localtime_s(&t,&n);
		r = _snprintf_s(buf,len, len,"[DBG %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#else
		t = *localtime(&n);
	    r = snprintf(buf,len, "[DBG %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#endif
	{
	va_list ap;
    va_start(ap, fmt);

#ifdef WIN32
	xlen = vsnprintf_s((char *)buf+r,len-r,len-r, fmt, ap);
#else
	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#endif
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return iop_log_write(log,buf, r);

}



int LOG_INF(const char *fmt,...)
{
	char buf[102400];
	int len = sizeof(buf)-1;
	int xlen = 0;
    iop_log_service_t *log = g_iop_log_service;
    struct tm t;
	int level = IOP_LOG_INFO;
    time_t n ;
	int r;
	if(log && (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;
	n= time(NULL);
	#ifdef WIN32
		localtime_s(&t,&n);
		r = _snprintf_s(buf,len, len,"[INF %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#else
		t = *localtime(&n);
	    r = snprintf(buf,len, "[INF %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#endif
	{
	va_list ap;
    va_start(ap, fmt);

#ifdef WIN32
	xlen = vsnprintf_s((char *)buf+r,len-r,len-r, fmt, ap);
#else
	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#endif
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return iop_log_write(log,buf, r);

}


int LOG_ERR(const char *fmt,...)
{
	char buf[102400];
	int len = sizeof(buf)-1;
	int xlen = 0;
    iop_log_service_t *log = g_iop_log_service;
    struct tm t;
	int level = IOP_LOG_ERR;
    time_t n ;
	int r;
	if(log && (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;
	n= time(NULL);
	#ifdef WIN32
		localtime_s(&t,&n);
		r = _snprintf_s(buf,len, len,"[ERR %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#else
		t = *localtime(&n);
	    r = snprintf(buf,len, "[ERR %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#endif
	{
	va_list ap;
    va_start(ap, fmt);

#ifdef WIN32
	xlen = vsnprintf_s((char *)buf+r,len-r,len-r, fmt, ap);
#else
	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#endif
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return iop_log_write(log,buf, r);

}


int LOG_ALT(const char *fmt,...)
{
	char buf[102400];
	int len = sizeof(buf)-1;
	int xlen = 0;
    iop_log_service_t *log = g_iop_log_service;
    struct tm t;
	int level = IOP_LOG_ALERT;
    time_t n ;
	int r;
	if(log && (level > log->log_level))
    {
        return -1;
    }
	buf[len] = 0;
	n= time(NULL);
	#ifdef WIN32
		localtime_s(&t,&n);
		r = _snprintf_s(buf,len, len,"[ALT %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#else
		t = *localtime(&n);
	    r = snprintf(buf,len, "[ALT %02d:%02d:%02d] ", 
                t.tm_hour,t.tm_min, t.tm_sec);
	#endif
	{
	va_list ap;
    va_start(ap, fmt);

#ifdef WIN32
	xlen = vsnprintf_s((char *)buf+r,len-r,len-r, fmt, ap);
#else
	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#endif
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return iop_log_write(log,buf, r);

}






int iop_log_fmt(void *buf, int len, int level, const char *fmt,...)
{
	char *p = 0;
    int xlen = 0;
    const char *level_str_alert = "ALT";
    const char *level_str_err = "ERR";
    const char *level_str_dbg = "DBG";
    const char *level_str_info = "INF";
    const char *level_str = "LOG";
    iop_log_service_t *log = g_iop_log_service;
    struct tm t;
	time_t n;		
	int r;
	if(log && (level > log->log_level))
    {
        return -1;
    }
    p = (char *)buf;
    p[len-1] = 0;
    len -= 1;
    switch(level)
    {
    case IOP_LOG_ALERT:
            level_str = level_str_alert;
            break;
    case IOP_LOG_ERR:
            level_str = level_str_err;
            break;
    case IOP_LOG_DBG:
            level_str = level_str_dbg;
            break;
    case IOP_LOG_INFO:
            level_str = level_str_info;
            break;
    }
    n = time(NULL);

	#ifdef WIN32
	
		localtime_s(&t,&n);
		r = _snprintf_s(buf,len, len,"[%s %02d:%02d:%02d] ", level_str,
                t.tm_hour,t.tm_min, t.tm_sec);
	#else
		t = *localtime(&n);
	    r = snprintf(buf,len, "[%s %02d:%02d:%02d] ", level_str,
                t.tm_hour,t.tm_min, t.tm_sec);
	#endif
	{
	va_list ap;
    va_start(ap, fmt);

#ifdef WIN32
	xlen = vsnprintf_s((char *)buf+r,len-r,len-r, fmt, ap);
#else
	xlen = vsnprintf((char *)buf+r,len-r, fmt, ap);
#endif
	
	va_end(ap);
	}
    if(xlen + r > len)
    {
        r = len;
    }
    else
    {
        r += xlen;
    }
    return iop_log_write(log,buf, r);
}

/*
int iop_log(int level, const char *fmt,...)
{
    int r = 0;
    char buf[10240];
    va_list ap;
    va_start(ap, fmt);
    r = iop_log_fmt(buf,sizeof(buf),level,fmt,ap);
    va_end(ap);
    return r;    
}
*/

