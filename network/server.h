#ifndef __NWAY_GATEWAY_XML_OP_NETWORK_SERVER_H__
#define __NWAY_GATEWAY_XML_OP_NETWORK_SERVER_H__
#include "../libs/libiop/inc/iop.h"
#include "../libs/libiop/inc/iop_log_service.h"
#include "../libs/libiop/inc/iop_server.h"

void set_fs_conf_path(const char* fs_path);
int push_server(short port); //用于将信息推送给连接的服务器

int exch_server(short port); //用于远端客户端与服务器间进行数据交换 

#endif