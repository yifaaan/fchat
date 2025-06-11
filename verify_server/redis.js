const config = require('./config.json');
const Redis = require('ioredis');

// create redis client
const redis_cli = new Redis({
    host: config.redis.host,
    port: config.redis.port,
    password: config.redis.passwd,
});

redis_cli.on('error', (err) => {
    console.log("redis client connection error: ", err);
    redis_cli.quit();
});

async function Get(key) {
    try {
        const value = await redis_cli.get(key);
        if (value == null) {
            console.log("redis key not found: ", key);
            return null;
        }
        console.log("redis get success key: ", key, " value: ", value);
        return value;
    } catch (err) {
        console.log("redis get error: ", err);
        return null;
    }
}

async function Exists(key) {
    try {
        const value = await redis_cli.exists(key);
        if (value == 0) {
            console.log("redis key not found: ", key);
            return false;
        }
        console.log("redis key found: ", key);
        return true;
    } catch (err) {
        console.log("redis exists error: ", err);
        return false;
    }
}

async function Set(key, value, expire) {
    try {
        const res = await redis_cli.set(key, value, 'EX', expire);
        console.log("redis set success key: ", key, " value: ", value);
        return res;
    } catch (err) {
        console.log("redis set error: ", err);
        return false;
    }
}

function Quit() {
    redis_cli.quit();
}

module.exports = { Get, Exists, Set, Quit };
