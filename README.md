# FS_Gateway_XML_Operator
用于将FreeSWITCH的Gateway接口化，采用c/s模型传输数据，数据打包用google::protobuf,c端为动态库，可以扩展为restful,webservice,或嵌入到第三方app中应用

how to build:

windows中：

1. github中下载protobuf,https://github.com/google/protobuf，并编译它生成lib库

2. 在vs2010中打开sln文件，在属性中把protobuf的include和lib路径加到工程目录中

3. 其余部分已包含在了工程中，可以直接编译



当前版本支持直接调用库来处理FreeSWITCH的conf/sip_profile/external下的文件，头文件如下：

        include "configure.h"
        include "../common/nwayfile.h"
        include "../common/gateway_file.h"

如果没有配置过路径或其它的配置项，则先配置一个默认的配置文件：

        save_sample_config("/usr/local/freeswitch/conf/sip_profile/external",9099);
/usr/local/freeswitch/conf/sip_profile/external 

为配置路径
9099为监听网络请求连接的端口，tcp的

、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、、

        nway_filename* files = (nway_filename*)malloc(sizeof(nway_filename));
这是一个配置的网关文件的列表结构体
如果采用指针使用的话，则在使用结束时，调用

        destory_file_list(files);
        
        来释放

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
  
  
  
  客户端：
  
  现阶段采用python flask实现的一个简易的restful接口
  
  比如要重新加载freeswitch的网关，则调用类似：
  
  http://localhost:8090/api/v1.0/reload_gateways 
  
  可以提交不提交则运行的fs_cli不带相关参数，参数有三个：
  
  shost ，sport， spassword，
  
  分别为freeswitch的服务器的host,freeswitch的esl port及密码
  
  
  获取网关列表，调用：
  
  http://localhost:8090/api/v1.0/get_gateways
  
  必须提交的参数：
  
  start_pos，number_per_page，
  
  类似分页，分别为开始取的文件序列，每页获取到的网关数量
  
  
  添加网关，调用：
  
  http://localhost:8090/api/v1.0/add_gateway
  
  参数清单如下：
  
  	gateway_name,username, realm, from_user, from_domain, password, extension ,
        proxy, expire_seconds, register, register_transport, retry_seconds, caller_id_in_from, 
    	contact_params, ping, filename, register_proxy



  修改网关，调用：
  
  http://localhost:8090/api/v1.0/edit_gateway
  
  参数清单如下：
  
  	gateway_name,username, realm, from_user, from_domain, password, extension ,
        proxy, expire_seconds, register, register_transport, retry_seconds, caller_id_in_from, 
    	contact_params, ping, filename, register_proxy

  
  删除网关，调用：
  
  http://localhost:8090/api/v1.0/del_gateway
  
  参数为：
  
  gateway_name
  
  
  在里边用了一个默认的验证检测，用户名为: ok, 密码为：python ,只有通过验证才能进行相关的操作
  
