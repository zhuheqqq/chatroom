/*处理命令的属性和序列化（将对象转换为 JSON 字符串）*/
#pragma once
#include<iostream>
#include<string>
#include<vector>
#include<nlohmann/json.hpp>

using namespace std;
using json=nlohmann::json;

struct UserCommand{
public:
    UserCommand()=default;
    ~UserCommand()=default;
    UserCommand(string uid,string question,string recvuid,int flag,vector<string> option)
    : m_uid(uid),m_question(question),m_recvuid(recvuid), m_flag(flag), m_option(option)
    {

    }

    string m_uid;//用户ID，默认为空字符串
    string m_recvuid;//接收者的uid
    string m_question;//用户密保问题的答案
    int m_flag;//用户发送操作者的类别
    vector<string> m_option;//命令的具体内容，动态数组

    void From_Json(string js)//从Json字符串中解析数据并填充到UserCommand结构体中
    {
        try {
            json jn = json::parse(js);
            jn.at("uid").get_to(m_uid);
            jn.at("question").get_to(m_question);
            jn.at("recvuid").get_to(m_recvuid);
            jn.at("flag").get_to(m_flag);
            jn.at("option").get_to(m_option);
        } catch (const exception& e) {
            cerr << "JSON parsing error: " << e.what() << endl;
        }
    }

    string  To_Json()
    {
        json jn=json{
            {"uid",m_uid},
            {"question",m_question},
            {"recvuid",m_recvuid},
            {"flag",m_flag},
            {"option",m_option},
        };

        return jn.dump();//json格式转为json字符串格式
    }

};