// iop_test.cpp : 定义控制台应用程序的入口点。
//

#ifdef WIN32
#include "stdafx.h"
#else
typedef char _TCHAR;
#define _tmain main
#endif

#include <stdio.h>
#include <stdlib.h>

#include "../inc/iop_log_service.h"
#include "../inc/iop_server.h"


/*
*测试基本功能，创建，调度10次，退出。
*/
void basic_test()
{
	int i = 0;
	iop_base_t *base = iop_base_new(10240);
	for(i = 0; i < 10; i++)
	{
		iop_dispatch(base);
		printf("dispatch %d.\n", i);
	}
	iop_base_free(base);
}




int my_timer_cb(iop_base_t *base,int id,unsigned int events,void *arg)
{
	if(events & EV_TYPE_DESTROY)
	{
		printf("timer was destroyed.\n");
		return 0;
	}
	if(events & EV_TYPE_TIMER)
	{
		char c = 0;
		printf("timer callback，would you want to exit?(y|n):");
		c = (char)getchar();
		fflush(stdin);
		if(c == 'y' || c== 'Y')
		{
			iop_stop(base);
		}
	}
	return 0;
}



/*
*测试定时器,当定时器触发时，用户可以选择是否退出系统。
*/
void timer_test()
{
	int i = 0;
	iop_base_t *base = iop_base_new(10240);
	printf("create a new iop_base_t object.\n"); 
	iop_add_timer(base,my_timer_cb,NULL,3);
	printf("add a new timer.\n");
	printf("start iop run loop.\n");
	iop_run(base);
}









int my_echo_parser(char *buf, int len){return len;}


int my_echo_processor(iop_base_t *base,int id,char *data,int len,void *arg)
{
	int r = 0;
	char buf[1024];
	memset(buf,0,sizeof(buf));
	if(len>1023){len=1023;}
	memcpy(buf,data,len); 
	printf("recv data len=%d,data=%s",len,buf);
	fflush(stdout);
	r = iop_buf_send(base,id,data,len);
	if( r < 0){printf("iop_buf_send error.\n"); return -1;}
	return 0;
}


int my_echo_on_error(iop_base_t *base, int id,int err, void *arg)
{
	switch(err)
	{
	case IOP_ERR_TIMEOUT:
		LOG_DBG("id %d timeout.\n",id);
		break;
	case IOP_ERR_CLOSED:
		LOG_DBG("id %d closed.\n", id);
		break;
	case IOP_ERR_SOCKET:
		LOG_DBG("id %d socket error.\n",id);
		break;
	case IOP_ERR_NO_MEM:
		LOG_DBG("id %d no memory.\n",id);
		break;
	}
	return -1;
}

void my_echo_on_connect(iop_base_t *base, int id, void *arg)
{
	LOG_DBG("id %d connected.\n",id);
}

void my_echo_on_destroy(iop_base_t *base, int id, void *arg)
{
	LOG_DBG("id %d destroy.\n",id);
}



/*
*测试回显服务器
*/
void echo_server_test()
{
	int keepalive_timeout = 60;
	iop_base_t *base = iop_base_new(10240);
	printf("create a new iop_base_t object.\n"); 
	iop_add_tcp_server(base,"0.0.0.0",7777,
		my_echo_parser,my_echo_processor,
		my_echo_on_connect,my_echo_on_destroy,my_echo_on_error,
		keepalive_timeout);
	printf("create a new tcp server on port 7777.\n");
	printf("start iop run loop.\n");
	iop_run(base);
	
}


void xserver()
{
	int r = iop_tcp_server("0.0.0.0",7777);
	LOG_DBG("a server start on port 7777.\n");
	while(1)
		{
			iop_usleep(1000);
		}
}


int async_conn_cb(iop_base_t *base, int id, unsigned int events, void *arg)
{
	if(EV_IS_SET_R(events))
		{
			//process read events.
		}
	if(EV_IS_SET_W(events))
		{
			//process write events.
		}
	if(EV_IS_SET(events, EV_TYPE_TIMEOUT))
		{
			LOG_DBG("id %d timeout.\n", id);
			return -1;
		}

	if(EV_IS_SET(events,EV_TYPE_CONNECT))
		{
		LOG_DBG("id %d connect ok.\n", id);
		}
	
	if(EV_IS_SET(events, EV_TYPE_ERROR))
		{
			LOG_DBG("id %d error.\n",id);
		}

	if(EV_IS_SET(events, EV_TYPE_DESTROY))
		{
			LOG_DBG("id %d destroy.\n", id);
		}

	return 0;
}


void asyn_connect_test()
{
	int i = 0;
	iop_base_t *base = iop_base_new(10240);


	iop_asyn_connect(base,"127.0.0.1",7777,5,async_conn_cb,0);
	for(i = 0; i < 20; i++)
	{
		iop_dispatch(base);
		printf("dispatch %d.\n", i);
	}
	iop_base_free(base);
	
}


int std_main(int argc, char **argv)
{
	char c;
	printf("iop unit test.\n");
	printf("1:basic_test.\n");
	printf("2:timer_test.\n");
	printf("3:echo_server_test.\n");
	printf("4:asyn_connect test.\n");

	printf("please input your choice:");
	c=(char)getchar();
	fflush(stdin);
	switch(c)
	{
	case '4':
		asyn_connect_test();
		break;
	case '3':
		echo_server_test();
		break;
	case '2':
		timer_test();
		break;
		
	case 'x':
		xserver();
		break;
	default:
		basic_test();
		break;
	}

	printf("Type any char to exit...");
	getchar();
	fflush(stdin);
	return 0;
}






int _tmain(int argc, _TCHAR* argv[])
{
	return std_main(argc, (char **)argv);
}

