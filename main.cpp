#include "common/gateway_file.h"
#include "common/nwayfile.h"
#include "protobuf/src/head.pb.h"
#include "protobuf/src/gateway_op.pb.h"
#include "network/connections.h"
#include "network/server.h"
#include "configure/configure.h"
using namespace nway_fs_opterator;


int main()
{
	//printf("the head size:%d,%d\n",sizeof(MsgHead),get_head_len());
	char szpath[255];
	short listen_port,push_port;
	if ( load_config(szpath,&listen_port,&push_port) == 0)
	{
		set_fs_conf_path(szpath);
		push_server(push_port);//now, this push thread is not required
//#ifdef WIN32
//		::Sleep(2000);
//#else
//		struct timeval tv;
//		tv.tv_sec = 2000 / 1000;
//		tv.tv_usec = (2000 % 1000) * 1000;
//		select(0, NULL, NULL, NULL, &tv);
//#endif
		exch_server(listen_port);
	}
	else
	{
		save_sample_config("/usr/local/freeswitch/conf/sip_profiles/external",8098,8099);
		printf("has save a simple file,please modify it and restart program\n");
	}
	return 0;
}