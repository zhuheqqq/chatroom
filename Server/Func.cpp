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
void Log_out(TcpSocket mysocket,UserCommand command);
void FriendList(TcpSocket mysocket,UserCommand command);
void Add_Friend(TcpSocket mysocket,UserCommand command);
void Delete_Friend(TcpSocket mysocket,UserCommand command);
void AgreeAddFriend(TcpSocket mysocket,UserCommand command);
void RefuseAddFriend(TcpSocket mysocket,UserCommand command);
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
        case LOGOUT:
            Log_out(mysocket,command);
            break;
        case FRIENDLIST:
            FriendList(mysocket,command);
            break;    
        case ADDFRIEND:
            Add_Friend(mysocket,command);
            break;
        case DELETEFRIEND:
            Delete_Friend(mysocket,command);
            break;
        case AGREEADDFRIEND:
            AgreeAddFriend(mysocket,command);
            break;
        case BLOCKFRIEND:
            RefuseAddFriend(mysocket,command);
            break;
        case REFUSEADDFRIEND:
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
            redis.hsetValue(uid, "昵称", command.m_nickname);
            redis.hsetValue(uid, "密码", command.m_option[0]);
            redis.hsetValue(uid, "在线状态", "-1");
            redis.hsetValue(uid, "性别", "未知");
            redis.hsetValue(uid, "其他信息", "无");
            redis.hsetValue(uid, "通知套接字", "-1");
            redis.hsetValue(uid, "聊天对象", "无");
            redis.hsetValue(uid + "的未读消息", "通知消息", "0");
            redis.hsetValue(uid + "的未读消息", "好友申请", "0");

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

//销毁账户
void Log_out(TcpSocket mysocket,UserCommand command)//功能已实现
{
    // 获取该用户的好友列表
    vector<string> friendList = redis.getFriendList(command.m_uid, "的好友列表");

    for (const string& friendID : friendList) {
        // 更新其他用户的好友列表，将该用户从好友列表中移除
        redis.removeMember(friendID+"的好友列表", command.m_uid);
    }

    //从好友列表中移除该用户的好友信息
    redis.delKey(command.m_uid+"的好友列表");

    //删除uid当中存储的特殊的信息
    redis.delKey(command.m_uid);//这一步成功执行
    redis.delKey(command.m_uid+"的未读消息");//这一步成功执行

    //这一步已实现，要删除集合中特定元素的值
    redis.sremValue("用户uid集合", command.m_uid); //从用户uid集合中移除
    mysocket.SendMsg("ok");

}


//没有实现
//如果好友列表可以展示在线状态，那么就没有必要有查看好友在线状态的选项
void FriendList(TcpSocket mysocket,UserCommand command)
{
    // 好友数量不为0，就遍历好友列表，根据在线状态发送要展示的内容
    vector<string> friendList = redis.getFriendList(command.m_uid,"的好友列表");

    for (const string& friendID : friendList) {
        string friendMark = redis.gethash(command.m_uid, friendID);
        //考虑TCP心跳监测
        string isOnline = redis.gethash(friendID, "在线状态");//无法获取实时的在线状态需要修改*/

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

//加好友太麻烦了，先判断自己的系统消息里是否有对方的好友申请，再判断自己是否已经发送过好友申请（我觉得这个可以不要)

void Add_Friend(TcpSocket mysocket,UserCommand command)//没写完
{
    if(!redis.sismember("用户uid集合",command.m_recvuid))//如果没有找到该用户返回错误
    {
        mysocket.SendMsg("none");//该用户不存在
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
            return;
        }
    }
    //如果自己的系统消息里有对方发来的未处理的好友申请，就不能向对方发送好友请求
    if(redis.hexists(command.m_uid+"收到的好友申请",command.m_recvuid))
    {
        //string msg=redis.gethash(command.m_uid+"的通知消息",command.m_recvuid);
        //string flag(msg.end()-11,msg.end());
        //if(flag=="(未处理)")
        //{
            mysocket.SendMsg("apply");
            return;
        //}
        
    }
    //如果已经向对方发送过好友申请未被处理，则不能再次发送申请
    if(redis.hexists(command.m_recvuid+"收到的好友申请",command.m_uid))
    {
        mysocket.SendMsg("handle");
        return;
    }

    //如果以上情况都没有发生，就在对方的好友申请里更新自己的好友申请
    //string wait="(未处理)";
    string apply="来自"+command.m_uid+"的好友申请:"+command.m_option[0];
    redis.hsetValue(command.m_recvuid+"收到的好友申请",command.m_uid,apply);
    //对方未读消息加1
    string nums=redis.gethash(command.m_recvuid+"的未读消息","好友申请");
    redis.hsetValue(command.m_recvuid+"的未读消息","好友申请",to_string(stoi(nums)+1));

    mysocket.SendMsg("ok");
}

void Delete_Friend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");//不存在该好友
        return;
    }else if(redis.removeMember(command.m_uid+"的好友列表",command.m_option[0]))
    {
        redis.removeMember(command.m_option[0]+"的好友列表",command.m_uid);
        mysocket.SendMsg("ok");//成功将该好友从好友列表中删除
        return;
    }
}

void AgreeAddFriend(TcpSocket mysocket,UserCommand command)//同意好友申请
{
    //查看通知消息里有没有他的申请
    if(!redis.hexists(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }

    //如果已经处理过该好友申请
    /*string msg=redis.gethash(command.m_uid+"的通知消息",command.m_recvuid);
    string flag(msg.end()-11,msg.end());
    if(flag=="(已拒绝)"||flag=="(已通过)")
    {
        mysocket.SendMsg("handle");
        return;
    }*/
    //没有上述情况正常同意

    if(redis.removeMember(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        string nownum = redis.gethash(command.m_uid + "的未读消息", "好友申请");
        redis.hsetValue(command.m_uid + "的未读消息", "好友申请", (to_string(stoi(nownum) - 1)));
        //完善同意者信息
        string nickname=redis.gethash(command.m_option[0],"昵称");
        cout<<nickname<<endl;
        redis.hsetValue(command.m_uid+"的好友列表",command.m_option[0],nickname);
        redis.lpushValue(command.m_uid+"---"+command.m_option[0],"-------------------");

        //完善申请者信息
        //command.m_nickname不可以正确输出
        redis.hsetValue(command.m_option[0]+"的好友列表",command.m_uid,command.m_nickname);
        redis.lpushValue(command.m_option[0]+"---"+command.m_uid,"-------------------");

        redis.lpushValue(command.m_option[0]+"的通知消息",command.m_uid+"通过了您的好友申请");

        // 申请者未读消息中的通知消息数量+1
        string num1 = redis.gethash(command.m_option[0] + "的未读消息", "通知消息");
        redis.hsetValue(command.m_option[0] + "的未读消息", "通知消息", to_string(stoi(num1)+1));

        mysocket.SendMsg("ok");
        return;

    }
}


void RefuseAddFriend(TcpSocket mysocket,UserCommand command)//拒绝好友申请
{
    //查看通知消息里有没有他的申请
    if(!redis.hexists(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }
    if(redis.removeMember(command.m_uid+"收到的好友申请",command.m_option[0]))
    {
        //拒绝者未读消息好友申请
        string nownum = redis.gethash(command.m_uid + "的未读消息", "好友申请");
        redis.hsetValue(command.m_uid + "的未读消息", "好友申请", (to_string(stoi(nownum) - 1)));
        
        
        
        // 申请者未读消息中的通知消息数量+1
        string num1 = redis.gethash(command.m_option[0] + "的未读消息", "通知消息");
        redis.hsetValue(command.m_option[0] + "的未读消息", "通知消息", to_string(stoi(num1)+1));
        redis.lpushValue(command.m_option[0]+"的通知消息",command.m_uid+"拒绝了您的好友申请");

        mysocket.SendMsg("ok");
    }

}

void Block_Friend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
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

void Restore_Friend(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的屏蔽列表",command.m_option[0]))
    {
        mysocket.SendMsg("no");//该好友未被屏蔽
        return;
    }else if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");//不存在该好友
        return;
    }else{
        redis.removeMember(command.m_uid+"的屏蔽列表",command.m_option[0]);//好友成功解除屏蔽
        mysocket.SendMsg("ok");
        return;
    }
}

int main()
{

    int lfd=0,cfd=0,efd=0; 
    char *buf;

    struct sockaddr_in saddr,caddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(9999);
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