#include <iostream>
#include <string>
#include <hiredis/hiredis.h>

class Redis {
public:
    Redis(const std::string& host = "127.0.0.1", int port = 6379) {
        context = redisConnect(host.c_str(), port);
        if (context == nullptr || context->err) {
            if (context) {
                std::cerr << "Error connecting to Redis: " << context->errstr << std::endl;
                redisFree(context);
            } else {
                std::cerr << "Could not allocate redis context" << std::endl;
            }
            throw std::runtime_error("Failed to connect to Redis server");
        }
    }

    ~Redis() {
        redisFree(context);
    }

    bool saddvalue(const std::string& key, const std::string& value) {
        redisReply* reply = (redisReply*)redisCommand(context, "SADD %s %s", key.c_str(), value.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

    bool hsetValue(const std::string& key, const std::string& field, const std::string& value) {
        redisReply* reply = (redisReply*)redisCommand(context, "HSET %s %s %s", key.c_str(), field.c_str(), value.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

    bool getHSetValue(const std::string& key, const std::string& field, std::string& value) {
        redisReply* reply = (redisReply*)redisCommand(context, "HGET %s %s", key.c_str(), field.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_STRING) {
            value = reply->str;
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

     bool sismember(const std::string& key, const std::string& member) {
        redisReply* reply = (redisReply*)redisCommand(context, "SISMEMBER %s %s", key.c_str(), member.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            bool exists = (reply->integer == 1);
            cout<<"\n"<<exists<<endl;
            freeReplyObject(reply);
            return exists;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

private:
    redisContext* context;
};