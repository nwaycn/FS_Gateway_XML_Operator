protoc --cpp_out=..\src gateway_op.proto
protoc --cpp_out=..\src head.proto

protoc --python_out=..\python head.proto