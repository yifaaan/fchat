const grpc = require('@grpc/grpc-js');
const message_proto = require('./proto');
const config_module = require('./config');
const { SendMail } = require('./email');
const const_module = require('./const');
const { v4: uuidv4 } = require('uuid');

async function GetVerifyCode(call, callback) {
    console.log("register email is ", call.request.email);
    try {
        unique_id = uuidv4();
        console.log("unique_id is ", unique_id);
        let text_str = "verify code is " + unique_id + " , please use it in 5 minutes";
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