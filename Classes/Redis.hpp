#include <iostream>
#include <string>
#include <hiredis/hiredis.h>

using namespace std;

class Redis {
public:
    Redis(const std::string& host = "127.0.0.1", int port = 6379) {
        context = redisConnect(host.c_str(), port);// 使用提供的主机和端口连接到Redis服务器。
        if (context == nullptr || context->err) {
            if (context) {// 如果连接已建立但出现错误，则输出错误消息
                std::cerr << "Error connecting to Redis: " << context->errstr << std::endl;
                redisFree(context);
            } else {// 如果无法建立连接（上下文为空），则输出错误消息。
                std::cerr << "Could not allocate redis context" << std::endl;
            }
            throw std::runtime_error("Failed to connect to Redis server");
        }
    }

    ~Redis() {
        redisFree(context);
    }

    bool saddvalue(const std::string& key, const std::string& value) {//增加键
    // 发送SADD命令到Redis并存储回复。
        redisReply* reply = (redisReply*)redisCommand(context, "SADD %s %s", key.c_str(), value.c_str());
        // 检查回复是否不为空，且回复类型是否为整数，且整数值为1。
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

    // 函数用于在Redis哈希中设置字段的值（使用HSET命令）
    bool hsetValue(const std::string& key, const std::string& field, const std::string& value) {//
        // 发送HSET命令到Redis并存储回复。
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

    // 函数用于从Redis哈希中获取字段的值（使用HGET命令）。
    bool getHSetValue(const std::string& key, const std::string& field, std::string& value) {
        redisReply* reply = (redisReply*)redisCommand(context, "HGET %s %s", key.c_str(), field.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_STRING) {
            value = reply->str;// 将回复中的值复制到'value'字符串中。
            freeReplyObject(reply);
            return true;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;// 返回false表示值未从哈希中获取
    }

    // 函数用于检查成员是否存在于Redis集合中（使用SISMEMBER命令）
     bool sismember(const std::string& key, const std::string& member) {
        redisReply* reply = (redisReply*)redisCommand(context, "SISMEMBER %s %s", key.c_str(), member.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            bool exists = (reply->integer == 1);// 检查整数值是否为1（成员存在于集合中）
            freeReplyObject(reply);
            return exists;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return false;
    }

    string gethash(const std::string& key, const std::string& field) {
    // 使用Redis的HGET命令获取哈希表中指定字段的值。
    std::string value;
    if (getHSetValue(key, field, value)) {
        return value;
    } else {
        // 如果字段不存在或发生了错误，返回一个空字符串。
        return "";
    }
}

// 创建 "用户uid集合" 的键空间并将其数据类型设置为集合（Set）
    void createUidSet() {
        const std::string key = "用户uid集合";
        const std::string dummyValue = "dummy"; // 添加一个虚拟的值，因为 SADD 需要至少一个元素

        // 使用 SADD 命令添加虚拟值到集合中，如果集合不存在则会创建
        redisReply* reply = (redisReply*)redisCommand(context, "SADD %s %s", key.c_str(), dummyValue.c_str());
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
    }

//构建好友列表哈希表的键，格式为“用户uid的好友列表”
    void addFriendToFriendList(const std::string& userID, const std::string& friendID, const std::string& friendInfo) {
    std::string friendListKey = userID + "的好友列表";

    // 使用HSET命令将好友信息添加到用户的好友列表哈希表中
    bool result = hsetValue(friendListKey, friendID, friendInfo);

    if (result) {
        std::cout << "好友添加成功！" << std::endl;
    } else {
        std::cout << "好友添加失败！" << std::endl;
    }
}

private:
    redisContext* context; // 指向Redis连接的上下文指针。
};