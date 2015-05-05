#ifndef __NWAY_GATEWAY_XML_OP_NETWORK_CONNECTION_H__
#define __NWAY_GATEWAY_XML_OP_NETWORK_CONNECTION_H__
#include "../libs/libiop/inc/iop.h"
#include "../libs/libiop/inc/iop_log_service.h"
#include "../libs/libiop/inc/iop_server.h"
#include <list>
using namespace std;
typedef struct tag_conn_client 
{
	iop_base_t* base;
	int id;
	
}conn_client;
class conn_manager
{
public:
	static conn_manager* get_instance();
	static conn_manager* pInstance;
	list<conn_client> push_list;
	list<conn_client> exch_list;
	static void Destroy();
private:
	conn_manager();
	
	~conn_manager();

};

#endif