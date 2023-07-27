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

    // 函数用于检查成员是否存在于Redis集合中（使用SISMEMBER命令）
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

// 创建 "用户uid集合" 的键空间并将其数据类型设置为集合（Set）
    void createUidSet() {
        const string key = "用户uid集合";
        const string dummyValue = "dummy"; // 添加一个虚拟的值，因为 SADD 需要至少一个元素

        // 使用 SADD 命令添加虚拟值到集合中，如果集合不存在则会创建
        redisReply* reply = (redisReply*)redisCommand(context, "SADD %s %s", key.c_str(), dummyValue.c_str());
        if (reply != nullptr) {
            freeReplyObject(reply);
        }
    }

//创建好友列表以及往好友列表里添加好友
void addFriendToFriendList(const string& userID, const string& friendID, const string& friendInfo) {
    string friendListKey = userID + "的好友列表";

    // 使用 HSETNX 命令将好友信息添加到用户的好友列表哈希表中
    redisReply* reply = (redisReply*)redisCommand(context, "HSETNX %s %s %s", friendListKey.c_str(), friendID.c_str(), friendInfo.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
        cout << "成功将用户 " << friendID << " 添加为好友！" << endl;
    } else {
        cout << "用户 " << friendID << " 已是好友，无需重复添加！" << endl;
    }
    if (reply) {
        freeReplyObject(reply);
    }
}

//创建屏蔽列表并往屏蔽列表里添加好友
void addToBlockedList(const string& userID, const string& blockedID) {
    string blockedListKey = userID + "的屏蔽列表";

    // 使用 HSETNX 命令将屏蔽信息添加到用户的屏蔽列表哈希表中
    redisReply* reply = (redisReply*)redisCommand(context, "HSETNX %s %s", blockedListKey.c_str(), blockedID.c_str());
    if (reply != nullptr && reply->type == REDIS_REPLY_INTEGER && reply->integer == 1) {
        cout << "成功将用户 " << blockedID << " 加入屏蔽列表！" << endl;
    } else {
        cout << "用户 " << blockedID << " 已在屏蔽列表中，无需重复添加！" << endl;
    }
    if (reply) {
        freeReplyObject(reply);
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
vector<string> Redis::getFriendList(const string& userID,const string& listType) {
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

private:
    redisContext* context; // 指向Redis连接的上下文指针。
};