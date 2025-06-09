const node_mailer = require('nodemailer');
const config_module = require('./config');

let transport = node_mailer.createTransport({
    host: 'smtp.qq.com',
    port: 465,
    secure: true,
    auth: {
        user: config_module.email_user,
        pass: config_module.email_pass
    }
})

function SendMail(mail_options) {
    return new Promise((resolve, reject) => {
        transport.sendMail(mail_options, (error, info) => {
            if (error) {
                console.log(error);
                reject(error);
            } else {
                console.log("send mail success: " + info.response);
                resolve(info.response);
            }
        })
    })
}

module.exports = { SendMail }