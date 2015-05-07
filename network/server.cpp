#include "server.h"
#include "../protobuf/src/head.pb.h"
#include "../protobuf/src/gateway_op.pb.h"
#include "connections.h"
#include "msg_parse.h"
#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#endif
#include "../configure/configure.h"
#ifdef WIN32

typedef unsigned threadfunc_t;
typedef void * threadparam_t;
#define STDPREFIX __stdcall
#else

typedef void * threadfunc_t;
typedef void * threadparam_t;
#define STDPREFIX
#endif

#ifdef WIN32
HANDLE m_thread;
unsigned m_dwThreadId;
#else
pthread_t m_thread;
#endif

using namespace nway_fs_opterator;
char fs_conf_path[255];
int push_parse(char *buf, int len)
{
	MsgHead h;
	h.ParseFromArray(buf,len);
	int allLen = h.ByteSize();
	int recvLen = h.nway_length() + sizeof(MsgHead);
	if (recvLen < len)
	{
		return 0;
	}
	return len;
}
int push_processor(iop_base_t *base,int id,char *data,int len,void *arg)
{
	return 0;
}
int push_on_error(iop_base_t *base, int id,int err, void *arg)
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
void push_on_connect(iop_base_t *base, int id, void *arg)
{
	conn_client cc ;
	cc.base = base;
	cc.id = id;
	conn_manager* conn = conn_manager::get_instance();
	if (conn) conn->push_list.push_back(cc);
	 
	LOG_DBG("id %d connected.\n",id);
}

void push_on_destroy(iop_base_t *base, int id, void *arg)
{
	conn_manager* conn =  conn_manager::get_instance();
	if (conn)
	{
		list<conn_client>::iterator it= conn->push_list.begin();
		for (; it!= conn->push_list.end();it++)
		{
			conn_client& c = *it;
			if (c.base == base)
			{
				list<conn_client>::iterator it2 = it++;

				conn->push_list.erase(it2);
				break;
			}
		}
	}
	
	LOG_DBG("id %d destroy.\n",id);
}

threadfunc_t STDPREFIX start_push_thread(threadparam_t zz)
{
	//short* port= (short*)zz;
	char szpath[255];
	short listen_port,push_port;
	if ( load_config(szpath,&listen_port,&push_port) == 0)
	{
		int keepalive_timeout = 60;
		iop_base_t *base = iop_base_new(1024);
		printf("create a new iop_base_t object.\n"); 
		iop_add_tcp_server(base,"0.0.0.0",push_port,
			push_parse,push_processor,
			push_on_connect,push_on_destroy,push_on_error,
			keepalive_timeout);
		printf("create a new push server on port %d.\n",push_port);
		printf("start push server run loop.\n");
		iop_run(base);
	}

#ifdef WIN32
	_endthreadex(0);
#endif
	return (threadfunc_t)NULL;
}

int push_server( short port )
{
	int res_status = 0;
#ifdef WIN32
	//	m_thread = ::CreateThread(NULL, 0, StartThread, this, 0, &m_dwThreadId);
	m_thread = (HANDLE)_beginthreadex(NULL, 0, &start_push_thread, &port, 0, &m_dwThreadId);
#else
	pthread_attr_t attr;

	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	if (pthread_create(&m_thread,&attr, start_push_thread,&port) == -1)
	{
		perror("Thread: create failed");

	}
	//	pthread_attr_destroy(&attr);
#endif
	return res_status ;
}




int exch_parse(char *buf, int len)
{
	for (int i=0; i<len;i++)
	{
		printf("\\x%x",*(buf+i));
	}
	printf("\n");
	int ilen = get_head_len();
	MsgHead h;
	char* mybuf=(char*)malloc(ilen);
	memcpy(mybuf,buf,ilen);
	h.ParseFromArray(mybuf,len);
	int allLen = h.ByteSize();
	int recvLen = h.nway_length() + get_head_len();
	free(mybuf);
	if (recvLen < len)
	{
		return 0;
	}
	return len;
}
int exch_processor(iop_base_t *base,int id,char *data,int len,void *arg)
{
	return exch_msg_parse(base,id,data,len,fs_conf_path);
	 
}
int exch_on_error(iop_base_t *base, int id,int err, void *arg)
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
void exch_on_connect(iop_base_t *base, int id, void *arg)
{
	conn_client cc ;
	cc.base = base;
	cc.id = id;
	conn_manager* conn = conn_manager::get_instance();
	if (conn) conn->exch_list.push_back(cc);
	LOG_DBG("id %d connected.\n",id);
}

void exch_on_destroy(iop_base_t *base, int id, void *arg)
{
	conn_manager* conn =  conn_manager::get_instance();
	if (conn)
	{
		list<conn_client>::iterator it= conn->exch_list.begin();
		for (; it!= conn->exch_list.end();it++)
		{
			conn_client& c = *it;
			if (c.base == base)
			{
				list<conn_client>::iterator it2 = it++;
				
				conn->exch_list.erase(it2);
				break;
			}
		}
	}
	LOG_DBG("id %d destroy.\n",id);
}
int exch_server( short port )
{
	int res_status = 0;
	int keepalive_timeout = 60;
	iop_base_t *base = iop_base_new(10240);
	printf("create a new iop_base_t object.\n"); 
	iop_add_tcp_server(base,"0.0.0.0",port,
		exch_parse,exch_processor,
		exch_on_connect,exch_on_destroy,exch_on_error,
		keepalive_timeout);
	printf("create a new tcp server on port %d.\n",port);
	printf("start iop run loop.\n");
	iop_run(base);
	return res_status ;
}

void set_fs_conf_path( const char* fs_path )
{
	sprintf(fs_conf_path,"%s\0", fs_path);
}



