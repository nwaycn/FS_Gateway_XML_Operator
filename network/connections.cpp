#include "connections.h"
conn_manager* conn_manager::pInstance=NULL;
conn_manager::conn_manager()
{

}

conn_manager::~conn_manager()
{

}

conn_manager* conn_manager::get_instance()
{
	if (! conn_manager::pInstance)
	{
		pInstance = new conn_manager();
	}
	return pInstance;
}

void conn_manager::Destroy()
{
	delete pInstance;
	pInstance = NULL;
}
