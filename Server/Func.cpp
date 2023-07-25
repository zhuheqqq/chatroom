//#include "../Classes/TcpSocket.hpp"
#include "../Classes/TcpSocket.cpp"
#include "../Classes/UserCommand.hpp"
#include "option.hpp"
#include "wrap.hpp"
#include "../Classes/Redis.hpp"
#include<hiredis/hiredis.h>
#include<string>
#include<random>

using namespace std;
Redis redis;
//TcpSocket mysocket;

struct Argc_func{
public:
    Argc_func(TcpSocket mysocket, string command_string) : mysocket(mysocket), command_string(command_string) {}
    TcpSocket mysocket;
    string command_string;
};

void Sign_up(TcpSocket mysocket,UserCommand command);
void Log_in(TcpSocket mysocket,UserCommand command);

void task(void *arg)
{
    Argc_func *argc_func = static_cast<Argc_func*>(arg);
    UserCommand command;     // Command类存客户端的命令内容
    TcpSocket mysocket = argc_func->mysocket;      // TcpSocket类用于通信
    command.From_Json(argc_func->command_string); 

    switch(command.m_flag){
        case SIGNUP:
            Sign_up(mysocket,command);
            break;
        case LOGIN:
            Log_in(mysocket,command);
            break;
    }

    return;
}

void Sign_up(TcpSocket mysocket,UserCommand command)//注册选项
{
    while(1)
    {
        //string uid=to_string((rand()+111111)%100000);

        random_device rd;
        mt19937 gen(rd()); // 使用 Mersenne Twister 引擎作为随机数生成器
        uniform_int_distribution<int> dis(0, 99999); // 定义一个均匀分布的随机整数分布

        string uid = to_string(dis(gen));
        cout << "生成的随机 uid 为：" << uid << endl;

        if(redis.sismember("用户uid集合",uid)){//随机到一个未注册的uid
            //cout << "生成的随机 uid 为：" << uid << endl;
            continue;
        }else{
            redis.saddvalue("用户uid集合", uid);
            redis.hsetValue(uid, "账号", uid);
            redis.hsetValue(uid, "密码", command.m_option[0]);
            redis.hsetValue(uid, "在线状态", "-1");
            redis.hsetValue(uid, "性别", "未知");
            redis.hsetValue(uid, "其他信息", "无");
            //redis.hsetValue(uid, "通知套接字", "-1");
            redis.hsetValue(uid, "聊天对象", "无");
            redis.hsetValue(uid + "的未读消息", "通知消息", "0");

            mysocket.SendMsg(uid);
            cout<<"用户"<<uid<<"注册成功"<<endl;
            return;
        }
    }
}

void Log_in(TcpSocket mysocket,UserCommand command)
{
    //从数据库跳去对应的数据进行核对并回复结果
    if(!redis.sismember("用户uid集合",command.m_uid)){//帐号不存在返回错误
        mysocket.SendMsg("nonexisent");
    }else{
        //如果帐号存在进行密码比对
        string pwd=redis.gethash(command.m_uid,"密码");
        cout<<pwd<<endl;
        if(pwd!=command.m_option[0])
        {
            mysocket.SendMsg("discorrect");

        }else{
            //密码正确，可以登陆改变其在线状态
            redis.hsetValue(command.m_uid,"在线状态",to_string(mysocket.getfd()));
            redis.hsetValue("fd-uid表",to_string(mysocket.getfd()),command.m_uid);
            redis.hsetValue(command.m_uid,"聊天对象","0");
           // redis.hsetValue(command.m_uid,"通知套接字","-1");
           mysocket.SendMsg("ok");
           cout<<"用户"<<command.m_uid<<"登陆成功"<<endl;
        }
    }

    return;
}

int main()
{
    redis.createUidSet();

    int lfd=0,cfd=0;
    char *buf;

    struct sockaddr_in saddr,caddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(9999);
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);

    //创建套接字
    lfd=Socket(AF_INET,SOCK_STREAM,0);
    
    //绑定地址
    Bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    
    //设置上限数
    Listen(lfd,128);

   
    
    while(1)
    {
        //阻塞等待连接
        socklen_t len=sizeof(caddr);
        cfd=Accept(lfd,(struct sockaddr*)&caddr,&len);

        int ret=recvMsg(cfd,&buf);
        if(ret<=0)
        {
            cerr<<"error receiving data ."<<endl;
            close(cfd);
            continue;
        }

        buf[ret]='\0';
        string command_string=buf;
        
        cout<<"Recrived request:"<<command_string<<endl;

        //创建任务，处理客户端请求
        Argc_func argc_func(TcpSocket(cfd),command_string);
        task(&argc_func);

        close(cfd);

    }
    

    
    
}