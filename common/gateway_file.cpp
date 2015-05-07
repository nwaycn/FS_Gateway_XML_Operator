#include "gateway_file.h"
#include "nwayfile.h"
#include "../libs/tinyxml/tinyxml.h"
int get_gateway_count( const char* fs_conf_path )
{
	return get_file_count(fs_conf_path);
}
void inline set_att_for_param(TiXmlElement* parent,const char* param1,const char* param2)
{
	if (strlen(param2) > 0)
	{
		TiXmlElement* tmp_ele= new TiXmlElement(GATEWAY_PARAM);
		tmp_ele->SetAttribute(GATEWAY_NAME, param1);
		tmp_ele->SetAttribute(GATEWAY_VALUE, param2);
		parent->LinkEndChild(tmp_ele);
	}
	
}
int read_gateway_from_file( const char* fs_conf_gateway_file,nway_gateway& gw )
{
	int res_status = 0;
	//read from xml file
	TiXmlDocument doc(fs_conf_gateway_file);
	doc.LoadFile();
	if ( doc.Error() && doc.ErrorId() == TiXmlBase::TIXML_ERROR_OPENING_FILE ) {
		printf( "WARNING: File %s not found.\n", fs_conf_gateway_file);
		res_status = -1;
	}
	else
	{
		TiXmlElement* include_ele = doc.FirstChildElement(GATEWAY_INCLUDE);
		TiXmlElement* gateway_ele = include_ele->FirstChildElement(GATEWAY_GATEWAY);
		//validate gateway element
		if (gateway_ele)
		{
			if (gateway_ele->Attribute(GATEWAY_NAME) != NULL)
			{
				gw.set_gateway_name(gateway_ele->Attribute(GATEWAY_NAME))  ;//gateway name
				TiXmlNode* pNode  = NULL;
				for (pNode = gateway_ele->FirstChildElement(); pNode; pNode = pNode->NextSiblingElement())
				{
					TiXmlElement* ptmp_ele = pNode->ToElement();
					TiXmlAttribute * pAttr = ptmp_ele->FirstAttribute();
					TiXmlAttribute * pNextAttr =NULL;
					char szTmpName[100]={0};
					char szTmpValue[100]={0};
					if (pAttr)
					{
						do
						{                           
							const char* name = pAttr->Name();
							if (name && stricmp(name,GATEWAY_NAME)==0)
							{
								strncpy(szTmpName,pAttr->Value(),100);
							}
							else if (name && stricmp(name,GATEWAY_VALUE)==0)
							{
								strncpy(szTmpValue,pAttr->Value(),100);
							}
							
						}while(pAttr = pAttr->Next());
						if (strlen(szTmpValue)>0)
						{
							if (stricmp(szTmpName,"username") == 0)
							{
								gw.set_username(szTmpValue);
							}
							else if (stricmp(szTmpName,"realm") == 0)
							{
								gw.set_realm(szTmpValue);
							}
							else if (stricmp(szTmpName,"from-user") == 0)
							{
								gw.set_from_user(szTmpValue);
							}
							else if (stricmp(szTmpName,"from-domain") == 0)
							{
								gw.set_from_domain(szTmpValue);
							}
							else if (stricmp(szTmpName,"password") == 0)
							{
								gw.set_password(szTmpValue);
							}
							else if (stricmp(szTmpName,"extension") == 0)
							{
								gw.set_extension(szTmpValue);
							}
							else if (stricmp(szTmpName,"proxy") == 0)
							{
								gw.set_proxy(szTmpValue);
							}
							else if (stricmp(szTmpName,"register-proxy") == 0)
							{
								gw.set_register_proxy( szTmpValue);
							}
							else if (stricmp(szTmpName,"expire-seconds") == 0)
							{
								gw.set_expire_seconds(szTmpValue);
							}
							else if (stricmp(szTmpName,"register") == 0)
							{
								gw.set_register_(szTmpValue);
							}
							else if (stricmp(szTmpName,"register-transport") == 0)
							{
								gw.set_register_transport(szTmpValue);
							}
							else if (stricmp(szTmpName,"retry-seconds") == 0)
							{
								gw.set_retry_seconds(szTmpValue);
							}
							else if (stricmp(szTmpName,"caller-id-in-from") == 0)
							{
								gw.set_caller_id_in_from(szTmpValue);
							}
							else if (stricmp(szTmpName,"contact-params") == 0)
							{
								gw.set_contact_params(szTmpValue);
							}
							else if (stricmp(szTmpName,"ping") == 0)
							{
								gw.set_ping(szTmpValue);
							}
						}
						

					}
					
				}
			}
			else
				res_status = -2;
		}
		else
			res_status = -1;

	}
	 
	return res_status;
}

int save_gateway( const char* fs_conf_path,const nway_gateway& gw )
{
	int res_status=0;
	char szPath[255] = {0};
#ifdef WIN32
	sprintf(szPath,"%s\\%s.xml\0",fs_conf_path,gw.gateway_name().c_str());
#else
	sprintf(szPath,"%s/%s.xml\0", fs_conf_path, gw.gateway_name().c_str());
#endif
	if (!file_exists(szPath))
	{
		return -2;
		//file has  existed
	}
	TiXmlDocument* doc = new TiXmlDocument(szPath);
	if (!doc)
	{
		printf("WARNING: document new failed\n");
		res_status = -1;
		return res_status;
	}
	//this file not xml Declaration
	TiXmlElement* include_ele = new TiXmlElement(GATEWAY_INCLUDE);
	doc->LinkEndChild(include_ele);
	//gateway element
	TiXmlElement* gateway_ele= new TiXmlElement(GATEWAY_GATEWAY);
	gateway_ele->SetAttribute(GATEWAY_NAME,gw.gateway_name().c_str());
	include_ele->LinkEndChild(gateway_ele);
	//////////////////////////////////////////////////////////////////////////
	//params

	set_att_for_param(gateway_ele,"username",gw.username().c_str());

	set_att_for_param(gateway_ele,"realm",gw.realm().c_str());

	set_att_for_param(gateway_ele,"from-user",gw.from_user().c_str());

	set_att_for_param(gateway_ele,"from-domain",gw.from_domain().c_str());

	set_att_for_param(gateway_ele, "password", gw.password().c_str());
	set_att_for_param(gateway_ele, "extension", gw.extension().c_str());
	set_att_for_param(gateway_ele, "proxy", gw.proxy().c_str());
	set_att_for_param(gateway_ele, "register-proxy",gw.register_proxy().c_str());
	set_att_for_param(gateway_ele, "expire-seconds", gw.expire_seconds().c_str());
	set_att_for_param(gateway_ele, "register", gw.register_().c_str());
	set_att_for_param(gateway_ele, "register-transport", gw.register_transport().c_str());
	set_att_for_param(gateway_ele, "retry-seconds", gw.retry_seconds().c_str());
	set_att_for_param(gateway_ele, "caller-id-in-from", gw.caller_id_in_from().c_str());
	set_att_for_param(gateway_ele, "contact-params", gw.contact_params().c_str());
	set_att_for_param(gateway_ele, "ping", gw.ping().c_str());
	doc->SaveFile();
	if (doc)
	{
		delete doc;
		doc = NULL;
	}
	return res_status;

}

int erase_gateway( const char* fs_conf_path, const nway_gateway& gw )
{
	int res_status = 0;
	char szPath[255] = {0};
#ifdef WIN32
	sprintf(szPath,"%s\\%s.xml\0",fs_conf_path,gw.gateway_name().c_str());
#else
	sprintf(szPath,"%s/%s.xml\0", fs_conf_path, gw.gateway_name().c_str());
#endif
	if (!file_exists(szPath))
	{
		return -2;
		//file has  existed
	}

	return nway_remove(szPath);
}

int add_gateway( const char* fs_conf_path,const nway_gateway& gw )
{
	int res_status=0;
	//file name and path
	char szPath[255] = {0};
#ifdef WIN32
	sprintf(szPath,"%s\\%s.xml\0",fs_conf_path,gw.gateway_name().c_str());
#else
	sprintf(szPath,"%s/%s.xml\0", fs_conf_path, gw.gateway_name().c_str());
#endif
	//check the file exist?
	if (file_exists(szPath))
	{
		return -2;
		//file has  existed
	}
	TiXmlDocument* doc = new TiXmlDocument();
	if (!doc)
	{
		printf("WARNING: document new failed\n");
		res_status = -1;
		return res_status;
	}
	//this file not xml Declaration
	TiXmlElement* include_ele = new TiXmlElement(GATEWAY_INCLUDE);
	doc->LinkEndChild(include_ele);
	//gateway element
	TiXmlElement* gateway_ele= new TiXmlElement(GATEWAY_GATEWAY);
	gateway_ele->SetAttribute(GATEWAY_NAME,gw.gateway_name().c_str());
	include_ele->LinkEndChild(gateway_ele);
	//////////////////////////////////////////////////////////////////////////
	//params
	 
	set_att_for_param(gateway_ele,"username",gw.username().c_str());
	 
	set_att_for_param(gateway_ele,"realm",gw.realm().c_str());
	 
	set_att_for_param(gateway_ele,"from-user",gw.from_user().c_str());
	 
	set_att_for_param(gateway_ele,"from-domain",gw.from_domain().c_str());
    
	set_att_for_param(gateway_ele, "password", gw.password().c_str());
	set_att_for_param(gateway_ele, "extension", gw.extension().c_str());
	set_att_for_param(gateway_ele, "proxy", gw.proxy().c_str());
	set_att_for_param(gateway_ele, "register-proxy",gw.register_proxy().c_str());
	set_att_for_param(gateway_ele, "expire-seconds", gw.expire_seconds().c_str());
	set_att_for_param(gateway_ele, "register", gw.register_().c_str());
	set_att_for_param(gateway_ele, "register-transport", gw.register_transport().c_str());
	set_att_for_param(gateway_ele, "retry-seconds", gw.retry_seconds().c_str());
	set_att_for_param(gateway_ele, "caller-id-in-from", gw.caller_id_in_from().c_str());
	set_att_for_param(gateway_ele, "contact-params", gw.contact_params().c_str());
	set_att_for_param(gateway_ele, "ping", gw.ping().c_str());
	doc->SaveFile(szPath);
	if (doc)
	{
		delete doc;
		doc = NULL;
	}
	return res_status;
}

