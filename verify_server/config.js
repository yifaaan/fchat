const fs = require('fs');

let config = JSON.parse(fs.readFileSync('config.json', 'utf8'));
let email_user = config.email.user;
let email_pass = config.email.pass;

let mysql_host = config.mysql.host;
let mysql_port = config.mysql.port;
let mysql_user = config.mysql.user;
let mysql_passwd = config.mysql.passwd;
let mysql_database = config.mysql.database;

let redis_host = config.redis.host;
let redis_port = config.redis.port;
let redis_passwd = config.redis.passwd;

let code_prefix = "code_";

module.exports = {
    email_user,
    email_pass,
    mysql_host,
    mysql_port,
    mysql_user,
    mysql_passwd,
    mysql_database,
    redis_host,
    redis_port,
    redis_passwd,
    code_prefix
}