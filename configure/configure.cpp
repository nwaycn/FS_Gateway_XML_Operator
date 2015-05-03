#include "configure.h"
#include "stdio.h"
#include "../libs/tinyxml/tinyxml.h"

int load_config( char* fs_conf_path, short* port )
{
	int res_status = 0;
	TiXmlDocument doc(NWAY_GATEWAY_OPERATE_CONFIG_FILE);
	doc.LoadFile();
	if ( doc.Error() && doc.ErrorId() == TiXmlBase::TIXML_ERROR_OPENING_FILE ) {
		printf( "WARNING: File %s not found.\n", NWAY_GATEWAY_OPERATE_CONFIG_FILE);
		res_status = -1;
	}
	else
	{
		//TiXmlHandle docH( &doc );
		TiXmlElement* root = doc.FirstChildElement(NWAY_GATEWAY_OPERATE_CONFIG_ROOT);
		TiXmlElement* fs_conf_ele = root->FirstChildElement(NWAY_GATEWAY_OPERATE_CONFIG_FS_CONF_PATH);
		//TiXmlText* el_fs_conf_path = fs_conf_ele->Value();
		if (fs_conf_ele && fs_conf_ele->GetText())
		{
			strcpy(fs_conf_path, fs_conf_ele->GetText());
		}
		TiXmlElement* port_ele = root->FirstChildElement(NWAY_GATEWAY_OPERATE_CONFIG_PORT);
		if (port_ele && port_ele->GetText())
		{
			char szTmp[30];
			strcpy(szTmp, port_ele->GetText());
			*port = atoi(szTmp);

		}
	}
	return res_status;
}

int save_sample_config( const char* fs_conf_path, short port )
{
	int res_status = 0;
	char szTmp[30];
	TiXmlDocument* doc = new TiXmlDocument();
	if (!doc)
	{
		printf("WARNING: document new failed\n");
		res_status = -1;
		return res_status;
	}
	TiXmlDeclaration *pDeclaration = new TiXmlDeclaration("1.0","","");
	if (!pDeclaration)
	{
		printf("WARNING: Declaration new failed\n");
		res_status = -1;
		return res_status;
	}
	doc->LinkEndChild(pDeclaration);
	TiXmlElement* pRoot = new TiXmlElement(NWAY_GATEWAY_OPERATE_CONFIG_ROOT);
	doc->LinkEndChild(pRoot);

	//////////////////////////////////////////////////////////////////////////
	TiXmlElement* pFS_conf_path = new TiXmlElement(NWAY_GATEWAY_OPERATE_CONFIG_FS_CONF_PATH);

	TiXmlText* pFS_conf_path_txt = new TiXmlText(fs_conf_path);
	pFS_conf_path->LinkEndChild(pFS_conf_path_txt);

	//////////////////////////////////////////////////////////////////////////
	TiXmlElement* pPort = new TiXmlElement(NWAY_GATEWAY_OPERATE_CONFIG_PORT);
	if (port<1000 || port > 65535)
	{
		port = 8098;
	}
	sprintf(szTmp,"%d",port);
	TiXmlText* pPort_txt = new TiXmlText(szTmp);
	pPort->LinkEndChild(pPort_txt);
	//////////////////////////////////////////////////////////////////////////
	pRoot->LinkEndChild(pFS_conf_path);
	pRoot->LinkEndChild(pPort);
	doc->SaveFile(NWAY_GATEWAY_OPERATE_CONFIG_FILE);
	if (doc)
	{
		delete doc;
		doc = NULL;
	}
	return res_status;
}

