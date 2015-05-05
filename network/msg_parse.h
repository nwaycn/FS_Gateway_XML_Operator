#ifndef __NWAY_GATEWAY_XML_OP_NETWORK_PARSE_H__
#define __NWAY_GATEWAY_XML_OP_NETWORK_PARSE_H__
#include "../libs/libiop/inc/iop.h"
#include "../libs/libiop/inc/iop_log_service.h"
#include "../libs/libiop/inc/iop_server.h"
int get_head_len();
int exch_msg_parse(iop_base_t* base, int id,const char* data,int len, const char* fs_conf_path);

#endif