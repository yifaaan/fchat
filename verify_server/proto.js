const path = require('path');
const grpc = require('@grpc/grpc-js');
const proto_loader = require('@grpc/proto-loader');

const PROTO_PATH = path.join(__dirname, 'message.proto');
const package_definition = proto_loader.loadSync(PROTO_PATH, { keepCase: true, longs: String, enums: String, defaults: true, oneofs: true });

const proto_discriptor = grpc.loadPackageDefinition(package_definition);
const message_proto = proto_discriptor.message;
module.exports = message_proto;