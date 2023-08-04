#include <iostream>
#include <string>
#include<vector>
#include <hiredis/hiredis.h>

using namespace std;

class Redis {
public:
    Redis(const string& host = "127.0.0.1", int port = 6379) {
        context = redisConnect(host.c_str(), port);// 使用提供的主机和端口连接到Redis服务器。
        if (context == nullptr || context->err) {
            if (context) {// 如果连接已建立但出现错误，则输出错误消息
                cerr << "Error connecting to Redis: " << context->errstr << endl;
                redisFree(context);
            } else {// 如果无法建立连接（上下文为空），则输出错误消息。
                cerr << "Could not allocate redis context" << endl;
            }
            throw runtime_error("Failed to connect to Redis server");
        }
    }

    ~Redis() {
        redisFree(context);
    }

    bool saddvalue(const string& key, const string& value) {//增加键
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
    bool hsetValue(const string& key, const string& field, const string& value) {//
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
    bool getHSetValue(const string& key, const string& field, string& value) {
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

    // 函数用于检查成员是否存在于Redis集合中（使用SISMEMBER命令）,用于集合
     bool sismember(const string& key, const string& member) {
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
    string value;
    if (getHSetValue(key, field, value)) {
        return value;
    } else {
        // 如果字段不存在或发生了错误，返回一个空字符串。
        return "";
    }
}

//获得列表里的键的数量
 int getListCount(const string& userID, const string& listType) {
        string listKey = userID + listType;

        // 使用 HLEN 命令获取指定哈希表的字段数量，即列表中元素的数量
        redisReply* reply = (redisReply*)redisCommand(context, "HLEN %s", listKey.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            int listCount = static_cast<int>(reply->integer);
            freeReplyObject(reply);
            return listCount;
        }
        if (reply) {
            freeReplyObject(reply);
        }
        return 0; // 返回0表示列表为空或出现错误
    }

//获取指定的好友列表或者屏蔽列表
//获取哈希字段名
vector<string> getFriendList(const string& userID,const string& listType) {
    string friendListKey = userID + listType;
    vector<string> friendList;

    // 使用 HKEYS 命令获取指定哈希表的所有字段（即好友列表的键）
    redisReply* reply = (redisReply*)redisCommand(context, "HKEYS %s", friendListKey.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_ARRAY) {
        for (size_t i = 0; i < reply->elements; ++i) {
            friendList.push_back(reply->element[i]->str);
        }
        freeReplyObject(reply);
    }
    return friendList;
}

// 删除哈希表中指定的字段
bool removeMember(const string& key,const string& field) {
    // 使用 HDEL 命令从列表中移除好友
    redisReply* reply = (redisReply*)redisCommand(context, "HDEL %s %s", key.c_str(), field.c_str());
    if (reply != nullptr&&reply->type==REDIS_REPLY_INTEGER) {
        bool success = (reply->integer > 0); // 判断整数值是否大于0，表示删除成功
        freeReplyObject(reply);
        return success;
    }
    return false;
}

//删除集合中特定元素
bool sremValue(const string& key, const string& member) {
    // 发送 SREM 命令到 Redis 并存储回复。
    redisReply* reply = (redisReply*)redisCommand(context, "SREM %s %s", key.c_str(), member.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
        bool success = (reply->integer > 0); // 判断整数值是否大于0，表示删除成功
        freeReplyObject(reply);
        return success;
    }
    return false; // 返回 false 表示删除失败或成员不存在
}

//删除一个键
bool delKey(const string& key) {
        // 发送 DEL 命令到 Redis 并存储回复。
        redisReply* reply = (redisReply*)redisCommand(context, "DEL %s", key.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            bool success = (reply->integer > 0); // 判断整数值是否大于0，表示删除成功
            freeReplyObject(reply);
            return success;
        }
        return false; // 返回 false 表示删除失败或键不存在
    }

//用于检查哈希表中是否存在指定的字段
bool hexists(const string& key, const string& field) {
    // 使用 HGET 命令获取指定字段的值
    string value;
    if (getHSetValue(key, field, value)) {
        // 如果字段存在，返回 true
        return true;
    } else {
        // 如果字段不存在或发生了错误，返回 false
        return false;
    }
}

// 使用 LPUSH 命令将值插入列表头部
    bool lpushValue(const string& key, const string& value) {
        // 发送 LPUSH 命令到 Redis 并存储回复。
        redisReply* reply = (redisReply*)redisCommand(context, "LPUSH %s %s", key.c_str(), value.c_str());
        if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER) {
            // 判断回复的整数值是否大于等于 0，表示插入成功。
            bool success = (reply->integer >= 0);
            freeReplyObject(reply);
            return success;
        }
        return false; // 返回 false 表示插入失败
    }

    string lindexValue(const string& key, int index) {
    // 使用 LINDEX 命令获取列表中指定索引的元素
    redisReply* reply = (redisReply*)redisCommand(context, "LINDEX %s %d", key.c_str(), index);
    if (reply != nullptr && reply->type == REDIS_REPLY_STRING) {
        string value = reply->str;
        freeReplyObject(reply);
        return value;
    }
    return ""; // 返回空字符串表示获取失败
}

// 获取列表中的所有元素
    vector<string> lrangeAll(const std::string& key) {
        vector<string> result;

        // 使用 Redis 的 LRANGE 命令获取列表所有元素
        redisReply* reply = (redisReply*)redisCommand(context, "LRANGE %s 0 -1", key.c_str());
        if (reply == nullptr) {
            cerr << "Error: Failed to execute LRANGE command for key: " << key << endl;
            return result;
        }

        if (reply->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < reply->elements; ++i) {
                if (reply->element[i]->type == REDIS_REPLY_STRING) {
                    result.push_back(reply->element[i]->str);
                }
            }
        }

        freeReplyObject(reply);
        return result;
    }

private:
    redisContext* context; // 指向Redis连接的上下文指针。
};