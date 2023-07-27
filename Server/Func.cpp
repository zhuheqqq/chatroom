//#include "../Classes/TcpSocket.hpp"
#include "../Classes/TcpSocket.cpp"
#include "../Classes/UserCommand.hpp"
#include "option.hpp"
#include "wrap.hpp"
#include "../Classes/Redis.hpp"
#include<hiredis/hiredis.h>
#include<string>
#include<random>
#include<fcntl.h>
#include<sys/epoll.h>

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
void Add_Friend(TcpSocket mysocket,UserCommand command);
void FriendList(TcpSocket mysocket,UserCommand command);
void Block_Friend(TcpSocket mysocket,UserCommand command);
void Restore_Friend(TcpSocket mysocket,UserCommand command);

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
        case FRIENDLIST:
            FriendList(mysocket,command);
            break;    
        case ADDFRIEND:
            Add_Friend(mysocket,command);
            break;
        case BLOCKFRIEND:
            Block_Friend(mysocket,command);
            break;
        case RESTOREFRIEND:
            Restore_Friend(mysocket,command);
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

void Log_in(TcpSocket mysocket,UserCommand command)//登陆选项
{
    //从数据库跳去对应的数据进行核对并回复结果
    if(!redis.sismember("用户uid集合",command.m_uid)){//帐号不存在返回错误
        mysocket.SendMsg("nonexisent");
    }else{
        //如果帐号存在进行密码比对
        string pwd=redis.gethash(command.m_uid,"密码");
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

void FriendList(TcpSocket mysocket,UserCommand command)
{
    // 好友数量不为0，就遍历好友列表，根据在线状态发送要展示的内容
    vector<string> friendList = redis.getFriendList(command.m_uid,"的好友列表");

    for (const string& friendID : friendList) {
        string friendMark = redis.gethash(command.m_uid, friendID);
        //考虑TCP心跳监测
        string isOnline = redis.gethash(friendID, "在线状态");//无法获取实时的在线状态需要修改

        if (!redis.sismember(command.m_uid + "的屏蔽列表", friendID)) {
            if (isOnline != "-1") {
                mysocket.SendMsg(L_GREEN + friendMark + NONE + "(" + friendID + ")");
            } else {
                mysocket.SendMsg(L_WHITE + friendMark + NONE + "(" + friendID + ")");
            }
        }
    }

    mysocket.SendMsg("end");//展示完毕

}

void Add_Friend(TcpSocket mysocket,UserCommand command)//没写完
{
    if(!redis.sismember("用户uid集合",command.m_recvuid))//如果没有找到该用户返回错误
    {
        mysocket.SendMsg("none");
        return;
    }
    //遍历好友列表，判断帐号是否已经是自己的好友
    int num=redis.getListCount(command.m_uid,"的好友列表");//获得好友数量
    if(num!=0){
        vector<string> friendlist=redis.getFriendList(command.m_uid,"的好友列表");

        auto it=find(friendlist.begin(),friendlist.end(),command.m_recvuid);
        if(it!=friendlist.end())
        {
            mysocket.SendMsg("exist");//好友已存在
        }
    }
}

void Block_Friend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.sismember(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");//不存在该好友
        return;
    }
    //如果好友已经被屏蔽
    int num=redis.getListCount(command.m_uid,"的屏蔽列表");//屏蔽列表中好友的数量
    if(num!=0)
    {
        vector<string> blocklist=redis.getFriendList(command.m_uid,"的屏蔽列表");

        auto it=find(blocklist.begin(),blocklist.end(),command.m_option[0]);
        if(it!=blocklist.end())
        {
            mysocket.SendMsg("handled");
        }
    }else{//正常屏蔽好友
        redis.addToBlockedList(command.m_uid,command.m_option[0]);
        mysocket.SendMsg("ok");
        return;
    }
}

void Restore_Friend(TcpSocket mysocket,UserCommand command)//没写完，缺解除屏蔽
{
    if(!redis.sismember(command.m_uid+"的屏蔽列表",command.m_option[0]))
    {
        mysocket.SendMsg("no");//该好友未被屏蔽
        return;
    }
    if(!redis.sismember(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");//不存在该好友
        return;
    }
}

int main()
{
    redis.createUidSet();

    int lfd=0,cfd=0,efd=0; 
    char *buf;

    struct sockaddr_in saddr,caddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(6666);
    saddr.sin_addr.s_addr=htonl(INADDR_ANY);

    //创建套接字
    lfd=Socket(AF_INET,SOCK_STREAM,0);

    //设置端口复用
    int opt = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    //绑定地址
    Bind(lfd,(struct sockaddr *)&saddr,sizeof(saddr));
    
    //设置上限数
    Listen(lfd,128);

    struct epoll_event event;
    struct epoll_event resevent[1024];
    int size = sizeof(resevent) / sizeof(struct epoll_event);
    

    efd=epoll_create(100);//创建红黑树
    event.events=EPOLLIN|EPOLLET;//ET边缘触发

    event.data.fd=lfd;
    int ret=epoll_ctl(efd,EPOLL_CTL_ADD,lfd,&event);
    if(ret==-1)
    {
        perr_exit("epoll_ctl");
    }
       
    while(1)
    {
        int num=epoll_wait(efd,resevent,size,-1);
        for(int i=0;i<num;++i)
        {
            //取出当前文件描述符
            int curfd=resevent[i].data.fd;
            //判断文件描述符是否用于监听
            if(curfd==lfd)
            {
                //建立新的连接
                int cfd=Accept(curfd,NULL,NULL);

                int flag = fcntl(cfd, F_GETFL);
                flag |= O_NONBLOCK;
                fcntl(cfd, F_SETFL, flag);

                // 新得到的文件描述符添加到epoll模型中, 下一轮循环的时候就可以被检测了
                event.events = EPOLLIN|EPOLLET;    // 读缓冲区是否有数据
                event.data.fd = cfd;
                ret = epoll_ctl(efd, EPOLL_CTL_ADD, cfd, &event);
                if(ret == -1)
                {
                    perror("epoll_ctl-accept");
                    exit(0);
                }
            }else{
                //处理通信的文件描述符
                //接收数据
                char *buf;
               // memset(buf,0,sizeof(buf));用就出现段错误

                int ret=recvMsg(curfd,&buf);
                if(ret<=0)
                {
                    cerr<<"error receiving data ."<<endl;
                    close(curfd);
                    continue;
                }

                buf[ret]='\0';
                string command_string=buf;
                
                cout<<"Recrived request:"<<command_string<<endl;
                //创建任务，处理客户端请求
                Argc_func argc_func(TcpSocket(curfd),command_string);
                task(&argc_func);
            }
        }

    }  
    
}