# coding: gbk
__author__ = 'lihao,18621575908'
import google.protobuf
from flask import Flask, jsonify
from flask import request
from flask import abort
from flask.ext.httpauth import HTTPBasicAuth
from flask import make_response
from socket import *
from ctypes import *
import gateway_op_pb2
import head_pb2
import struct
import time
import pickle
host = 'localhost'
port = 8098
ADDR = (host,port)

app = Flask(__name__)
auth = HTTPBasicAuth()

def get_value(json,section):
    if json and section in json:
        return json[section]
    return ''

def check_data(theString):
    hd = head_pb2.MsgHead()
    head_str = theString[:7]
    hd.ParseFromString(head_str)
    print hd.nway_length
    if (hd.nway_length + 7 > len(theString)):
        return False
    return True
def recv_data(theSocket):
    total_data=[]
    while True:
        data = theSocket.recv(1024)
        total_data.append(data)
        if (check_data(''.join(total_data)) == True):
            print 'check data true'
            break
        if not data:
            print 'not any data'
            break

    return ''.join(total_data)

def erase_gateway(gateway_name):
    #delete a gateway
    req_ng = gateway_op_pb2.erase_nway_gateway_req()     #gateway_op_pb2.nway_gateway()

    req_ng.gateway.gateway_name = gateway_name
    s_req_ng = req_ng.SerializeToString()

    req_hd = head_pb2.MsgHead()
    req_hd.command = 10039
    req_hd.nway_length = req_ng.ByteSize()
    req_hd.cmd_flag = 0
    s_req_head = req_hd.SerializeToString()

    s_send =  s_req_head +s_req_ng

    tcpCliSock = socket(AF_INET,SOCK_STREAM)
    tcpCliSock.connect(ADDR)
    print repr(s_send)
    print repr(s_req_ng)
    print repr(s_req_head)
    tcpCliSock.send(s_send)
    erase_data = recv_data(tcpCliSock)
    tcpCliSock.close()
    reshd = head_pb2.MsgHead()
    reshd.ParseFromString(erase_data)
    print reshd.command

    res_ng = gateway_op_pb2.erase_nway_gateway_rsp()
    res_ng.ParseFromString(erase_data[7:])
    return_list ={
        'status':res_ng.status
    }
    return return_list

def add_gateway(gateway_name,username, realm, from_user, from_domain, password, extension ,\
        proxy, expire_seconds, register, register_transport, retry_seconds, caller_id_in_from, \
    contact_params, ping, filename, register_proxy):

    req_ng = gateway_op_pb2.add_nway_gateway_req()
    req_ng.gateway.gateway_name = gateway_name
    req_ng.gateway.username = username
    req_ng.gateway.realm = realm
    req_ng.gateway.from_user =from_user
    req_ng.gateway.from_domain = from_domain
    req_ng.gateway.password = password
    req_ng.gateway.extension = extension
    req_ng.gateway.proxy = proxy
    req_ng.gateway.expire_seconds = expire_seconds
    req_ng.gateway.register = register
    req_ng.gateway.register_transport = register_transport
    req_ng.gateway.retry_seconds = retry_seconds
    req_ng.gateway.caller_id_in_from = caller_id_in_from
    req_ng.gateway.contact_params = contact_params
    req_ng.gateway.ping = ping
    req_ng.gateway.filename = filename
    req_ng.gateway.register_proxy = register_proxy

    req_hd = head_pb2.MsgHead()
    req_hd.command = 10035
    req_hd.nway_length = req_ng.ByteSize()
    req_hd.cmd_flag = 0
    s_req_hd = req_hd.SerializeToString()
    s_req_ng = req_ng.SerializeToString()
    s_req_data = s_req_hd + s_req_ng

    tcpCliSock = socket(AF_INET,SOCK_STREAM)
    tcpCliSock.connect(ADDR)

    tcpCliSock.send(s_req_data)
    rsp_data = recv_data(tcpCliSock)
    tcpCliSock.close()
    rsp_hd = head_pb2.MsgHead()
    rsp_hd.ParseFromString(rsp_data)
    print rsp_hd.command

    rsp_ng = gateway_op_pb2.add_nway_gateway_rsp()
    rsp_ng.ParseFromString(rsp_data[7:])
    return_list ={
        'status':rsp_ng.status
    }
    return return_list

def modify_gateway(gateway_name,username, realm, from_user, from_domain, password, extension ,\
        proxy, expire_seconds, register, register_transport, retry_seconds, caller_id_in_from, \
    contact_params, ping, filename, register_proxy):
    req_ng = gateway_op_pb2.edit_nway_gateway_req
    req_ng.gateway.gateway_name = gateway_name
    req_ng.gateway.username = username
    req_ng.gateway.realm = realm
    req_ng.gateway.from_user =from_user
    req_ng.gateway.from_domain = from_domain
    req_ng.gateway.password = password
    req_ng.gateway.extension = extension
    req_ng.gateway.proxy = proxy
    req_ng.gateway.expire_seconds = expire_seconds
    req_ng.gateway.register = register
    req_ng.gateway.register_transport = register_transport
    req_ng.gateway.retry_seconds = retry_seconds
    req_ng.gateway.caller_id_in_from = caller_id_in_from
    req_ng.gateway.contact_params = contact_params
    req_ng.gateway.ping = ping
    req_ng.gateway.filename = filename
    req_ng.gateway.register_proxy = register_proxy

    req_hd = head_pb2.MsgHead()
    req_hd.command = 10037
    req_hd.nway_length = req_ng.ByteSize()
    req_hd.cmd_flag = 0
    s_req_hd = req_hd.SerializeToString()
    s_req_ng = req_ng.SerializeToString()
    s_req_data = s_req_hd + s_req_ng

    tcpCliSock = socket(AF_INET,SOCK_STREAM)
    tcpCliSock.connect(ADDR)

    tcpCliSock.send(s_req_data)
    rsp_data = recv_data(tcpCliSock)
    tcpCliSock.close()
    rsp_hd = head_pb2.MsgHead()
    rsp_hd.ParseFromString(rsp_data)
    print rsp_hd.command
    rsp_ng = gateway_op_pb2.edit_nway_gateway_rsp()
    rsp_ng.ParseFromString(rsp_data[7:])
    return_list ={
        'status':rsp_ng.status
    }
    return return_list

def reload_gateway(host, port, password):
    req_ng = gateway_op_pb2.reload_gateway_req()
    req_ng.host = host
    req_ng.port = port
    req_ng.password = password

    req_hd = head_pb2.MsgHead()
    req_hd.command = 10043
    req_hd.nway_length = req_ng.ByteSize()
    req_hd.cmd_flag = 0
    s_req_hd = req_hd.SerializeToString()
    s_req_ng = req_ng.SerializeToString()
    s_req_data = s_req_hd + s_req_ng
    tcpCliSock = socket(AF_INET,SOCK_STREAM)
    tcpCliSock.connect(ADDR)

    tcpCliSock.send(s_req_data)
    rsp_data = recv_data(tcpCliSock)
    tcpCliSock.close()
    rsp_hd =head_pb2.MsgHead()
    rsp_ng = gateway_op_pb2.reload_gateway_rsp()

    rsp_ng.ParseFromString(rsp_data[7:])
    return_list ={
        'status':rsp_ng.status
    }
    return return_list

def get_gateways(start_pos,number_per_page):
    req_ng = gateway_op_pb2.get_nway_gateways_req()
    req_ng.start_pos = 1
    req_ng.number_per_page = 10
    req_ng.falg = '1'
    s_req_ng = req_ng.SerializeToString()

    req_gg_hd = head_pb2.MsgHead()
    req_gg_hd.command = 10031
    req_gg_hd.nway_length = req_ng.ByteSize()
    req_gg_hd.cmd_flag = 0
    s_req_hd = req_gg_hd.SerializeToString()

    s_req_data = s_req_hd +s_req_ng
    print s_req_data
    print repr(s_req_data)
    print repr(s_req_ng)
    print repr(s_req_hd)
    print req_gg_hd.command
    print req_gg_hd.nway_length

    tcpCliSock = socket(AF_INET,SOCK_STREAM)
    tcpCliSock.connect(ADDR)

    tcpCliSock.send(s_req_data)
    rsp_data = recv_data(tcpCliSock)
    tcpCliSock.close()
    """
    check data validate
    """
    test_hd = head_pb2.MsgHead()
    test_hd.ParseFromString(s_req_hd)

    print test_hd.command
    test_req = gateway_op_pb2.get_nway_gateways_req()
    test_req.ParseFromString(s_req_ng)
    print test_req.start_pos
    print test_req.number_per_page
    rsp_hd = head_pb2.MsgHead()
    rsp_hd.ParseFromString(rsp_data)

    rsp_ng = gateway_op_pb2.get_nway_gateways_rsp()
    rsp_ng.ParseFromString(rsp_data[7:])
    mylist = []
    if (rsp_ng.status == 1):
        for my_ng in rsp_ng.gateways:
            my_detail = {
                'gateway_name':my_ng.gateway_name,
                'username':my_ng.username,
                'realm':my_ng.realm,
                'from_user':my_ng.from_user,
                'from_domain':my_ng.from_domain,
                'password':my_ng.password,
                'extension':my_ng.extension,
                'proxy':my_ng.proxy,
                'expire_seconds':my_ng.expire_seconds,
                'register':my_ng.register,
                'register_transport':my_ng.register_transport,
                'retry_seconds':my_ng.retry_seconds,
                'caller_id_in_from':my_ng.caller_id_in_from,
                'contact_params':my_ng.contact_params,
                'ping':my_ng.ping,
                'register_proxy':my_ng.register_proxy
            }
            mylist.append(my_detail)
    return_list = {
        'status':rsp_ng.status,
        'gateways':mylist
    }
    return return_list

@auth.get_password
def get_password(username):
    if username == 'ok':
        return 'python'
    return None
@auth.error_handler
def unauthorized():
    return make_response(jsonify({'error': 'Unauthorized access'}), 401)

@app.route('/api/v1.0/del_gateway', methods=['POST'])
@auth.login_required
def del_gateway_r():
    if not request.json or not 'gateway_name' in request.json :
        abort(400)
    gatway_name = request.json['gateway_name']
    res = erase_gateway(gatway_name)

    return jsonify(res) ,201

@app.route('/api/v1.0/add_gateway', methods=['POST'])
@auth.login_required
def add_gateway_r():
    if not request.json or not 'gateway_name' in request.json :
        abort(400)
    gateway_name = request.json['gateway_name']
    username = get_value(request.json,'username')
    realm = get_value(request.json,'realm')      #request.json[]
    from_user = get_value(request.json,'from_user')
    from_domain = get_value(request.json,'from_domain')
    password = get_value(request.json,'password')
    extension = get_value(request.json,'extension')
    proxy = get_value(request.json,'proxy')
    expire_seconds = get_value(request.json,'expire_seconds')
    register = get_value(request.json,'register')
    register_transport = get_value(request.json,'register_transport')
    retry_seconds = get_value(request.json,'retry_seconds')
    caller_id_in_from = get_value(request.json,'caller_id_in_from')
    contact_params = get_value(request.json,'contact_params')
    ping = get_value(request.json,'ping')
    filename = get_value(request.json,'filename')
    register_proxy = get_value(request.json,'register_proxy')
    res = add_gateway(gateway_name,username, realm, from_user, from_domain, password, extension ,\
        proxy, expire_seconds, register, register_transport, retry_seconds, caller_id_in_from, \
    contact_params, ping, filename, register_proxy)
    return jsonify(res) ,201

@app.route('/api/v1.0/edit_gateway', methods=['POST'])
@auth.login_required
def edit_gateway_r():
    if not request.json or not 'gateway_name' in request.json :
        abort(400)
    gateway_name = request.json['gateway_name']
    username = get_value(request.json,'username')
    realm = get_value(request.json,'realm')      #request.json[]
    from_user = get_value(request.json,'from_user')
    from_domain = get_value(request.json,'from_domain')
    password = get_value(request.json,'password')
    extension = get_value(request.json,'extension')
    proxy = get_value(request.json,'proxy')
    expire_seconds = get_value(request.json,'expire_seconds')
    register = get_value(request.json,'register')
    register_transport = get_value(request.json,'register_transport')
    retry_seconds = get_value(request.json,'retry_seconds')
    caller_id_in_from = get_value(request.json,'caller_id_in_from')
    contact_params = get_value(request.json,'contact_params')
    ping = get_value(request.json,'ping')
    filename = get_value(request.json,'filename')
    register_proxy = get_value(request.json,'register_proxy')
    res = modify_gateway(gateway_name,username, realm, from_user, from_domain, password, extension ,\
        proxy, expire_seconds, register, register_transport, retry_seconds, caller_id_in_from, \
    contact_params, ping, filename, register_proxy)
    return jsonify(res) ,201

@app.route('/api/v1.0/get_gateways', methods=['GET'])
@auth.login_required
def get_gateways_r():
    if not request.json or not 'start_pos' in request.json or not 'number_per_page' in request.json:
        abort(400)
    start_pos = int(request.json['start_pos'])
    number_per_page = int (request.json['number_per_page'])
    res = get_gateways(start_pos, number_per_page)
    return jsonify(res) ,201

@app.route('/api/v1.0/reload_gateways', methods=['GET'])
@auth.login_required
def reload_gateways_r():
    #if not request.json or not 'host' in request.json or not 'port' in request.json:
     #   abort(400)
    host = ''
    port = ''
    password = ''
    host = get_value(request.json,'shost')
    port = get_value(request.json,'sport')
    password = get_value(request.json,'spassword')
    res = reload_gateway(host,port, password)
    return jsonify(res) ,201

if __name__=="__main__":
    #erase_gateway('lihao')
    #myreturn = get_gateways(1,10)
    #print myreturn
    app.run(debug=True, host='0.0.0.0', port=8090)






