const grpc = require('@grpc/grpc-js');
const message_proto = require('./proto');
const config_module = require('./config');
const { SendMail } = require('./email');
const const_module = require('./const');
const { v4: uuidv4 } = require('uuid');
const redis_module = require('./redis');

async function GetVerifyCode(call, callback) {
    console.log("register email is ", call.request.email);
    try {
        let code = await redis_module.Get(config_module.code_prefix + call.request.email);
        console.log("code is ", code);
        if (code == null) {
            unique_id = uuidv4();
            if (unique_id.length > 4) {
                unique_id = unique_id.substring(0, 4);
            }
            let res = await redis_module.Set(config_module.code_prefix + call.request.email, unique_id, 300);
            code = unique_id;
            if (!res) {
                callback(null, { email: call.request.email, error: const_module.Errors.RedisErr });
                return;
            }
        }
        console.log("unique_id is ", code);
        let text_str = "verify code is " + code + " , please use it in 5 minutes";
        let mail_options = {
            from: config_module.email_user,
            to: call.request.email,
            subject: "verify code",
            text: text_str
        };
        let send_result = await SendMail(mail_options);
        console.log("send_result is ", send_result);
        callback(null, { email: call.request.email, error: const_module.Errors.Success })
    } catch (error) {
        console.log("error is ", error);
        callback(null, { email: call.request.email, error: const_module.Errors.Exception })
    }
}

function main() {
    var server = new grpc.Server();
    server.addService(message_proto.VerifyService.service, { GetVerifyCode: GetVerifyCode })
    server.bindAsync('127.0.0.1:50051', grpc.ServerCredentials.createInsecure(), (err, port) => {
        if (err) {
            return console.error(err);
        }
        console.log(`Server is running on port ${port}`);
    })
}

main();