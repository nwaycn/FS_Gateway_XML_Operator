#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "configure.h"
#include "../common/nwayfile.h"
#include "../common/gateway_file.h"
int main()
{
	printf("This is the test of configure\n");
	save_sample_config("/usr/local/freeswitch/conf",9099);
	char szpath[255];
	short port;
	load_config(szpath,&port);
	nway_filename* files = (nway_filename*)malloc(sizeof(nway_filename));
	memset(files,0,sizeof(files));
	//files->next = NULL;
	nway_filename* cur_file;
	int res_status = get_file_list("c:",files,0,5);
	cur_file = files->next;
	while (cur_file )
	{
		printf("%s\n",cur_file->szFilename);
		cur_file = cur_file->next;
	}
	destory_file_list(files);
	nway_gateway gw;
	gw.set_username("lihao");
	gw.set_gateway_name("nwaytest");
	gw.set_realm("192.168.1.1");
	gw.set_password("123456");
	gw.set_ping("10");
	gw.set_proxy("192.168.1.12");
	gw.set_register_("false");
	gw.set_register_transport("tport=tcp");
	gw.set_expire_seconds("30");
	add_gateway("c:\\xml",gw);

	int icount = get_gateway_count("c:\\xml");
	nway_filename nf;
	get_file_list("c:\\xml", &nf,0,10);
	//nway_filename* cur_file;
	cur_file = nf.next;
	while (cur_file )
	{
		nway_gateway gwtmp;
		read_gateway_from_file(cur_file->szFullPath,gwtmp);
		printf("%s\n",gwtmp.gateway_name().c_str());
		printf("%s\n",gwtmp.realm().c_str());
		cur_file = cur_file->next;
	}
	return 0;
}