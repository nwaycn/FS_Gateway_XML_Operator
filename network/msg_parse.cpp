#include "msg_parse.h"
#include "../protobuf/src/head.pb.h"
#include "../protobuf/src/gateway_op.pb.h"
#include "../common/gateway_file.h"
#include "../common/nwayfile.h"
#include "connections.h"
#include "command.h"

using namespace nway_fs_opterator;
int exch_msg_parse( iop_base_t* base, int id,const char* data,int len , const char* fs_conf_path)
{
	int res_status = 0;
    char* buf =(char*) malloc(len);
	//strncpy(buf,data,len);
	int nheadlen =  get_head_len();
	memcpy(buf,data,len);
	
	//必须另外拷贝，否则可能会被新的缓冲数据存坏
	char* headbuf = (char*)malloc(nheadlen);
	memcpy(headbuf,buf,nheadlen);
	MsgHead h;

	h.ParseFromArray(headbuf,nheadlen);
	free(headbuf);
	int allLen = h.ByteSize();
	
	int recvLen = h.nway_length() + nheadlen;
	switch (h.command())
	{
	case GXOP_CMD_GET_GATEWAYS_REQ:
		{
			get_nway_gateways_req req;
			get_nway_gateways_rsp rsp;
			char* mybuf =(char*) malloc(h.nway_length());
			memcpy(mybuf,buf+ nheadlen,h.nway_length());
			req.ParseFromArray(mybuf, h.nway_length());
			free(mybuf);
			//需要获取获得哪些文件，并生成相应的gateway数据
			nway_filename nf;
			int nres = get_file_list(fs_conf_path, &nf,req.start_pos(),req.number_per_page());
			nway_filename* curr_file = nf.next;
			nway_op_status st = success;
			if (nres != 0)
			{
				st = failed;
			}
			rsp.set_status(st);
			while ( curr_file)
			{
				
				if (curr_file->szFullPath)
				{
					nway_gateway* ng = rsp.add_gateways();
					read_gateway_from_file(curr_file->szFullPath, *ng);
				}
				curr_file = curr_file->next;
				
			}
			MsgHead rsph;
			rsph.set_nway_length(rsp.ByteSize());
			rsph.set_command(GXOP_CMD_GET_GATEWAYS_RSP);
			rsph.set_cmd_flag(id);
			string send_data;
			rsph.AppendToString(&send_data);
			rsp.AppendToString(&send_data);
			iop_buf_send(base,id,send_data.c_str(),send_data.length());
			printf("send rsp length:%d\n",send_data.length());
		}
		break;
	//case GXOP_CMD_GET_GATEWAY_REQ:
	//	{
	//		//暂不处理
	//	}
	//	break;
	case GXOP_CMD_GET_GATEWAY_ADD_REQ:
		{
			add_nway_gateway_req req;
			add_nway_gateway_rsp rsp;
			req.ParseFromArray(buf+ nheadlen, h.nway_length());
			int nres = add_gateway(fs_conf_path,req.gateway());
			nway_op_status st = success;
			if (nres != 0)
			{
				st = failed;
			}
			rsp.set_status(st);
			rsp.set_gateway_name(req.gateway().gateway_name());
			rsp.set_res_text("");
			MsgHead rsph;
			rsph.set_nway_length(rsp.ByteSize());
			rsph.set_command(GXOP_CMD_GET_GATEWAY_ADD_RSP);
			rsph.set_cmd_flag(id);
			string send_data;
			rsph.AppendToString(&send_data);
			rsp.AppendToString(&send_data);
			iop_buf_send(base,id,send_data.c_str(),send_data.length());

		}
		break;
	case GXOP_CMD_GET_GATEWAY_EDIT_REQ:
		{
			edit_nway_gateway_req req;
			edit_nway_gateway_rsp rsp;
			req.ParseFromArray(buf+ nheadlen, h.nway_length());
			int nres = save_gateway(fs_conf_path,req.gateway());
			nway_op_status st = success;
			if (nres != 0)
			{
				st = failed;
			}
			rsp.set_status(st);
			rsp.set_gateway_name(req.gateway().gateway_name());
			rsp.set_res_text("");
			MsgHead rsph;
			rsph.set_nway_length(rsp.ByteSize());
			rsph.set_command(GXOP_CMD_GET_GATEWAY_EDIT_RSP);
			rsph.set_cmd_flag(id);
			string send_data;
			rsph.AppendToString(&send_data);
			rsp.AppendToString(&send_data);
			iop_buf_send(base,id,send_data.c_str(),send_data.length());
		}
		break;
	case GXOP_CMD_GET_GATEWAY_ERASE_REQ:
		{
			erase_nway_gateway_req req;
			erase_nway_gateway_rsp rsp;
			/*char sztmp[2048];
			memcpy(sztmp,buf+ nheadlen,h.nway_length());
			req.ParseFromArray(sztmp,h.nway_length());*/
			req.ParseFromArray(buf+ nheadlen, h.nway_length());
			int nres = erase_gateway(fs_conf_path,req.gateway());
			nway_op_status st = success;
			if (nres != 0)
			{
				st = failed;
			}
			rsp.set_status(st);
			rsp.set_gateway_name(req.gateway().gateway_name());
			rsp.set_res_text("");
			MsgHead rsph;
			rsph.set_nway_length(rsp.ByteSize());
			rsph.set_command(GXOP_CMD_GET_GATEWAY_ERASE_RSP);
			rsph.set_cmd_flag(id);
			string send_data;
			rsph.AppendToString(&send_data);
			rsp.AppendToString(&send_data);
			iop_buf_send(base,id,send_data.c_str(),send_data.length());
		}
		break;
	case GXOP_CMD_GET_GATEWAY_RELOAD_REQ:
		{
			reload_gateway_req req;
			reload_gateway_rsp rsp;
			char* mybuf =(char*) malloc(h.nway_length());
			memcpy(mybuf,buf+ nheadlen,h.nway_length());
			req.ParseFromArray(mybuf, h.nway_length());
			free(mybuf);
			int nres = nway_reload_gateway(req.host().c_str(),req.port().c_str(),req.password().c_str());
			nway_op_status st = success;
			if (nres != 0)
			{
				st = failed;
			}
			rsp.set_status(st);
			MsgHead rsph;
			rsph.set_nway_length(rsp.ByteSize());
			rsph.set_command(GXOP_CMD_GET_GATEWAY_RELOAD_RSP);
			rsph.set_cmd_flag(id);
			string send_data;
			rsph.AppendToString(&send_data);
			rsp.AppendToString(&send_data);
			iop_buf_send(base,id,send_data.c_str(),send_data.length());
		}
		break;
	default:
		{
			MsgHead rsph;
			rsph.set_nway_length(0);
			rsph.set_command(GXOP_CMD_GET_GATEWAY_UNKOWN_RSP);
			rsph.set_cmd_flag(id);
			string send_data;
			rsph.AppendToString(&send_data);

			iop_buf_send(base,id,send_data.c_str(),send_data.length());
		}
		
	}
	free(buf);
	return res_status;
}
int get_head_len()
{
	MsgHead h;
	//h.ByteSize();
	h.set_cmd_flag(0);
	h.set_command(0);
	h.set_nway_length(0);
	//printf("bytesize:%d\n", h.ByteSize());
	return h.ByteSize()+1;
}