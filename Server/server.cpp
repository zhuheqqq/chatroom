#include "group.hpp"
struct Argc_func {
public:
    Argc_func(TcpSocket mysocket, string command_string)
        : mysocket(mysocket), command_string(command_string){}

    TcpSocket mysocket;
    string command_string;
};

void Sign_up(TcpSocket mysocket,UserCommand command);//注册
void Log_in(TcpSocket mysocket,UserCommand command);//登陆
void Log_out(TcpSocket mysocket,UserCommand command);//注销
void Pass_re(TcpSocket mysocket,UserCommand command);//找回密码
void ChangePwd(TcpSocket mysocket,UserCommand command);//修改密码
void FriendList(TcpSocket mysocket,UserCommand command);//展示好友列表
void Add_Friend(TcpSocket mysocket,UserCommand command);//添加好友
void Delete_Friend(TcpSocket mysocket,UserCommand command);//删除好友
void AgreeAddFriend(TcpSocket mysocket,UserCommand command);//同意好友请求
void RefuseAddFriend(TcpSocket mysocket,UserCommand command);//拒绝好友请求
void Block_Friend(TcpSocket mysocket,UserCommand command);//屏蔽好友
void Restore_Friend(TcpSocket mysocket,UserCommand command);//恢复会话
void ViewOnlineStatus(TcpSocket mysocket,UserCommand command);//查看好友在线状态
void UnreadMessage(TcpSocket mysocket,UserCommand command);//未读消息
void ChatWithFriend(TcpSocket mysocket,UserCommand command);//私聊请求
void FriendSendMsg(TcpSocket mysocket,UserCommand command);//发送消息
void ExitChat(TcpSocket mysocket,UserCommand command);//停止聊天
void AddGroup(TcpSocket mysocket,UserCommand command);//加群
void CreateGroup(TcpSocket mysocket,UserCommand command);//创建群聊
void GroupList(TcpSocket mysocket,UserCommand command);//群聊列表
void MemberList(TcpSocket mysocket,UserCommand command);//成员列表
void DeleteGroup(TcpSocket mysocket,UserCommand command);//退出群聊
void DeleteMember(TcpSocket mysocket,UserCommand command);//删除成员
void AddManager(TcpSocket mysocket,UserCommand command);//添加管理员
void DeleteManager(TcpSocket mysocket,UserCommand command);//删除管理员
void DissolveGroup(TcpSocket mysocket,UserCommand command);//解散群聊
void ApplyList(TcpSocket mysocket,UserCommand command);//申请加群列表
void AgreeAddMember(TcpSocket mysocket,UserCommand command);//同意加群
void RefuseAddMember(TcpSocket mysocket,UserCommand command);//拒绝加群
void ChatGroup(TcpSocket mysocket,UserCommand command);
void GroupSendMsg(TcpSocket mysocket,UserCommand command);
void ExitChatGroup(TcpSocket mysocket,UserCommand command);


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
        case PASSWORDRECOVERY:
            Pass_re(mysocket,command);
            break;
        case CHANGEPASSWORD:
            ChangePwd(mysocket,command);
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
            Block_Friend(mysocket,command);
            break;
        case RESTOREFRIEND:
            Restore_Friend(mysocket,command);
            break;
        case REFUSEADDFRIEND:
            RefuseAddFriend(mysocket,command);
            break;
        case VIEWONLINESTATUS:
            ViewOnlineStatus(mysocket,command);
            break;
        case UNREADMESSAGE:
            UnreadMessage(mysocket,command);
            break;
        case CHATWITHFRIEND:
            ChatWithFriend(mysocket,command);
            break;
        case SENDMSG:
            FriendSendMsg(mysocket,command);
            break;
        case EXITCHAT:
            ExitChat(mysocket,command);
            break;
        case ADDGROUP:
            AddGroup(mysocket,command);
            break;
        case CREATEGROUP:
            CreateGroup(mysocket,command);
            break;
        case GROUPLIST:
            GroupList(mysocket,command);
            break;
        case MEMBERLIST:
            MemberList(mysocket,command);
            break;
        case DELETEGROUP:
            DeleteGroup(mysocket,command);
            break;
        case DELETEMEMBER:
            DeleteMember(mysocket,command);
            break;
        case ADDMANAGER:
            AddManager(mysocket,command);
            break;
        case DELETEMANAGER:
            DeleteManager(mysocket,command);
            break;
        case DISSOLVEGROUP:
            DissolveGroup(mysocket,command);
            break;
        case APPLYLIST:
            ApplyList(mysocket,command);
            break;
        case AGREEADDMEMBER:
            AgreeAddMember(mysocket,command);
            break;
        case REFUSEADDMEMBER:
            RefuseAddMember(mysocket,command);
            break;
        case CHATGROUP:
            ChatGroup(mysocket,command);
            break;
        case CHATSENDMSG:
            GroupSendMsg(mysocket,command);
            break;
    }

    return;
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

                //redis.hsetValue("fd-uid表",to_string(curfd),"-1");
                //cout<<"客户端连接成功，套接字为:"<<curfd<<endl;
            }else{

                TcpSocket mysocket(curfd);
                
                //处理通信的文件描述符
                //接收数据
                char *buf;
               // memset(buf,0,sizeof(buf));用就出现段错误

                int ret=recvMsg(curfd,&buf);
                //cout<<ret<<endl;
                if(ret<=0)
                {
                    cerr<<"error receiving data ."<<endl;
                    string uid =redis.gethash("fd-uid表",to_string(curfd)); // 获取客户端的用户ID
                    redis.sremValue("在线用户列表",uid);
                    redis.hsetValue(uid,"通知套接字","-1");
                    redis.hsetValue("fd-uid表",to_string(curfd),"-1");
                    close(curfd);
                    continue;
                }

                buf[ret]='\0';
                //cout<<buf<<endl;
                string command_string=buf;

                UserCommand command;
                command.From_Json(command_string);
                cout<<"Recrived request:"<<command_string<<endl;
                //创建任务，处理客户端请求

                
                if(command.m_flag==RECV)
                {
                    redis.hsetValue(command.m_uid, "通知套接字", to_string(curfd));
                    //redis.hsetValue("fd-uid表", to_string(curfd), command.m_uid+"(通)");
                }else{
                    Argc_func argc_func(TcpSocket(curfd),command_string);
                    task(&argc_func);
                }
                
            }
        }
        
    } 
    
}

