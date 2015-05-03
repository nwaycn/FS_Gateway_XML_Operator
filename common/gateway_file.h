/*
 * Copyright (c) 2011-2015 ShangHai Nway  Information Technology Co., Ltd. 
 * eamil:lihao@nway.com.cn
 * addr: china
 * license: GPL 2.0
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#ifndef ____NWAY_FREESWITCH_GATEWAY_FILE__
#define ____NWAY_FREESWITCH_GATEWAY_FILE__
#include "../protobuf/src/head.pb.h"
#include "../protobuf/src/gateway_op.pb.h"
#define GATEWAY_INCLUDE "include"
#define GATEWAY_GATEWAY "gateway"
#define GATEWAY_PARAM "param"
#define GATEWAY_NAME "name"
#define GATEWAY_VALUE "value"
using namespace nway_fs_opterator;
int get_gateway_count(const char* fs_conf_path);
int read_gateway_from_file(const char* fs_conf_gateway_file,nway_gateway& gw);
int save_gateway(const char* fs_conf_path,const nway_gateway& gw);
int erase_gateway(const char* fs_conf_path, const nway_gateway& gw);
int add_gateway(const char* fs_conf_path,const nway_gateway& gw);
#endif