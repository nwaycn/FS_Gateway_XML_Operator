# FS_Gateway_XML_Operator
用于将FreeSWITCH的Gateway接口化，采用c/s模型传输数据，数据打包用google::protobuf,c端为动态库，可以扩展为restful,webservice,或嵌入到第三方app中应用

当前版本支持直接调用库来处理FreeSWITCH的conf/sip_profile/external下的文件，头文件如下：
include "configure.h"
include "../common/nwayfile.h"
include "../common/gateway_file.h"

如果没有配置过路径或其它的配置项，则先配置一个默认的配置文件：
save_sample_config("/usr/local/freeswitch/conf/sip_profile/external",9099);
/usr/local/freeswitch/conf/sip_profile/external 为配置路径
9099为监听网络请求连接的端口，tcp的

、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、

nway_filename* files = (nway_filename*)malloc(sizeof(nway_filename));
这是一个配置的网关文件的列表结构体
如果采用指针使用的话，则在使用结束时，调用destory_file_list(files);来释放

、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、

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
	添加一个新的网关配置文件
	、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
	
	int icount = get_gateway_count("c:\\xml");
	获取网关文件的数量
	、、、、、、、、、、、、、、、、、、、、、、、、、、、、、
	
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
	
	获取并遍历所有的网关配置的内容
  
