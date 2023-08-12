#include "friend.hpp"


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


void CreateGroup(TcpSocket mysocket,UserCommand command)//创建群聊
{
    //判断好友列表里有没有该好友
    if(!redis.hexists(command.m_uid+"的好友列表",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }

    //有的话进行下一步，随机到未被注册到的群聊uid
    while(1)
    {
        //随机数这样好像不太行，写完之后可以换一种生成随机数的方法
        random_device rd;
        mt19937 gen(rd()); // 使用 Mersenne Twister 引擎作为随机数生成器
        uniform_int_distribution<int> dis(10000000, 99999999); // 定义一个均匀分布的随机整数分布

        string groupuid = to_string(dis(gen));
        cout << "生成的随机群 uid 为：" << groupuid << endl;

        if(redis.sismember("群聊集合",groupuid))
        {
            continue;
        }else{
            redis.saddvalue("群聊集合",groupuid);
            //补充群聊详细信息
            redis.hsetValue(groupuid+"的基本信息","群号",groupuid);
            redis.hsetValue(groupuid+"的基本信息","群名称",groupuid);
            redis.hsetValue(groupuid+"的基本信息","群介绍","无");
            redis.hsetValue(groupuid+"群成员列表",command.m_uid,"群主");
            redis.hsetValue(command.m_uid+"的群聊列表",groupuid,"群主");
            redis.rpushValue(groupuid+"的群聊消息","这是一个新的群聊,欢迎大家的加入");

            string num = redis.gethash(command.m_option[0] + "的未读消息", "通知消息");
            redis.hsetValue(command.m_option[0] + "的未读消息", "通知消息", to_string(stoi(num)+1));
            redis.rpushValue(command.m_option[0]+"的通知消息",command.m_uid+"邀请您加入群聊"+groupuid);

            redis.hsetValue(groupuid+"群成员列表",command.m_option[0],"群成员");
            redis.hsetValue(command.m_option[0]+"的群聊列表",groupuid,"群成员");

            if(redis.sismember("在线用户列表",command.m_option[0]))
            {
                string friend_fd=redis.gethash(command.m_option[0],"通知套接字");
                TcpSocket friendsocket(stoi(friend_fd));
                friendsocket.SendMsg(L_BLUE+command.m_uid+"邀请您加入新的群聊"+NONE);
            }
        }
        mysocket.SendMsg(groupuid);
        return;
    }
    

}

void GroupList(TcpSocket mysocket,UserCommand command)
{
    if(!redis.exists(command.m_uid+"的群聊列表"))
    {
        mysocket.SendMsg("none");
        return;
    }

    vector<string> grouplist=redis.getFriendList(command.m_uid,"的群聊列表");

    for(const string& groupid:grouplist){
        mysocket.SendMsg(groupid);
    }

    mysocket.SendMsg("end");
}

void AddGroup(TcpSocket mysocket,UserCommand command)//功能好着呢
{
    if(redis.hexists(command.m_option[0]+"群成员列表",command.m_uid))
    {
        mysocket.SendMsg("handled");
        return;
    }else if(!redis.sismember("群聊集合",command.m_option[0]))
    {
        mysocket.SendMsg("none");
        return;
    }else if(redis.hexists(command.m_option[0]+"的申请加群列表",command.m_uid)){
        mysocket.SendMsg("had");
    }else
    {
        //在申请加群列表里添加信息
        string msg="申请加入群聊";
        redis.hsetValue(command.m_option[0]+"的申请加群列表",command.m_uid,msg);
        //int num=redis.getListCount(command.m_option[0],"的群成员列表");

        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员")
            {
                string apply=command.m_uid+"申请加入群聊"+command.m_option[0];
                string num=redis.gethash(memberid+"的未读消息","群聊消息");
                redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.rpushValue(memberid+"群聊消息",apply);

                if(redis.sismember("在线用户列表",memberid))
                {
                    string member_fd=redis.gethash(memberid,"通知套接字");
                    TcpSocket membersocket(stoi(member_fd));
                    membersocket.SendMsg(L_BLUE+apply+NONE);
                }
            }
        }

        mysocket.SendMsg("ok");
        return;
    }
}

void MemberList(TcpSocket mysocket,UserCommand command)//bug已解决
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");
        return;
    }
        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            //cout<<memberid<<endl;
            mysocket.SendMsg(memberid);
        }
        mysocket.SendMsg("end");
    
}

void DeleteGroup(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_option[0]))
    {
        mysocket.SendMsg("none");
        return;
    }

    //将群聊从用户的群聊列表里删除
    redis.removeMember(command.m_uid+"的群聊列表",command.m_option[0]);
    redis.removeMember(command.m_option[0]+"群成员列表",command.m_uid);
    mysocket.SendMsg("ok");
}

void DeleteMember(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_recvuid+"群成员列表",command.m_uid))
    {
        mysocket.SendMsg("none");
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_uid)=="群成员")
    {
        mysocket.SendMsg("no");
        return;
    }

    redis.removeMember(command.m_option[0]+"的群聊列表",command.m_recvuid);
    redis.removeMember(command.m_recvuid+"群成员列表",command.m_option[0]);

    vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

    for(const string& memberid:memberlist)
    {
        if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员")
        {
            string apply=command.m_uid+"将"+command.m_option[0]+"移除群聊"+command.m_recvuid;
            string num=redis.gethash(memberid+"的未读消息","群聊消息");
            redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
            redis.rpushValue(memberid+"群聊消息",apply);

            if(redis.sismember("在线用户列表",memberid))
            {
                string member_fd=redis.gethash(memberid,"通知套接字");
                TcpSocket membersocket(stoi(member_fd));
                membersocket.SendMsg(L_BLUE+apply+NONE);
            }
        }
    }

    mysocket.SendMsg("ok");
    return;

}

void AddManager(TcpSocket mysocket,UserCommand command)//功能暂时好着呢
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_recvuid))
    {
        mysocket.SendMsg("none");
        return;
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_uid)!="群主")
    {
        mysocket.SendMsg("no");
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_option[0])=="群管理员")
    {
        mysocket.SendMsg("handled");
    }

    redis.hsetValue(command.m_recvuid+"群成员列表",command.m_option[0],"群管理员");

    if(redis.sismember("在线用户列表",command.m_option[0]))
    {
        string member_fd=redis.gethash(command.m_option[0],"通知套接字");
        TcpSocket membersocket(stoi(member_fd));
        membersocket.SendMsg(L_BLUE+command.m_uid+"将你添加为群聊"+command.m_recvuid+"的群管理员"+NONE);
    }

    mysocket.SendMsg("ok");
}

void DeleteManager(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_recvuid))
    {
        mysocket.SendMsg("none");
        return;
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_uid)!="群主")
    {
        mysocket.SendMsg("no");
    }else if(redis.gethash(command.m_recvuid+"群成员列表",command.m_option[0])!="群管理员")
    {
        mysocket.SendMsg("handled");
    }

    redis.hsetValue(command.m_recvuid+"群成员列表",command.m_option[0],"群成员");

    if(redis.sismember("在线用户列表",command.m_option[0]))
    {
        string member_fd=redis.gethash(command.m_option[0],"通知套接字");
        TcpSocket membersocket(stoi(member_fd));
        membersocket.SendMsg(L_BLUE+command.m_uid+"将你在"+command.m_recvuid+"的群管理员身份移除"+NONE);
    }

    mysocket.SendMsg("ok");
}

void DissolveGroup(TcpSocket mysocket,UserCommand command)
{
    if(redis.gethash(command.m_option[0]+"群成员列表",command.m_uid)!="群主")
    {
        mysocket.SendMsg("no");
        return;
    }

    redis.sremValue("群聊集合",command.m_option[0]);//将群聊从群聊集合中删除
    
    vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

    for(const string& memberid:memberlist)
    {
        
        redis.removeMember(memberid+"的群聊列表",command.m_option[0]);
        string apply=command.m_uid+"(群主)将"+command.m_option[0]+"群聊解散";
        string num=redis.gethash(memberid+"的未读消息","群聊消息");
        redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
        redis.rpushValue(memberid+"群聊消息",apply);

        //实时通知没有实现
        if(redis.sismember("在线用户列表",memberid)&&redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群主")
        {
            string member_fd=redis.gethash(memberid,"通知套接字");
            TcpSocket membersocket(stoi(member_fd));
            membersocket.SendMsg(L_BLUE+apply+NONE);
        }
        
    }
    
    //删除群聊信息
    redis.delKey(command.m_option[0]+"的基本信息");
    redis.delKey(command.m_option[0]+"的群聊消息");
    redis.delKey(command.m_option[0]+"群成员列表");


    mysocket.SendMsg("ok");

}

void ApplyList(TcpSocket mysocket,UserCommand command)
{
    if(redis.gethash(command.m_option[0]+"群成员列表",command.m_uid)!="群成员")
    {
        vector<string> memberlist=redis.getFriendList(command.m_option[0],"的申请加群列表");

        for(const string& memberid:memberlist)
        {
            mysocket.SendMsg(memberid);
        }
        mysocket.SendMsg("end");
    }else
    {
        mysocket.SendMsg("no");
    }
}

void AgreeAddMember(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_recvuid+"的申请加群列表",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }else{
        redis.removeMember(command.m_recvuid+"的申请加群列表",command.m_option[0]);
        redis.hsetValue(command.m_recvuid+"群成员列表",command.m_option[0],"群主");
        redis.hsetValue(command.m_option[0]+"的群聊列表",command.m_recvuid,"群成员");

        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员"&&command.m_uid!=memberid)
            {
                string apply=command.m_uid+"同意了"+command.m_option[0]+"的加群申请";
                string num=redis.gethash(memberid+"的未读消息","群聊消息");
                redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.rpushValue(memberid+"群聊消息",apply);

                if(redis.sismember("在线用户列表",memberid))
                {
                    string member_fd=redis.gethash(memberid,"通知套接字");
                    TcpSocket membersocket(stoi(member_fd));
                    membersocket.SendMsg(L_BLUE+apply+NONE);
                }
            }
        }
    }
    mysocket.SendMsg("ok");
    if(redis.sismember("在线用户列表",command.m_option[0]))
    {
        string member_fd=redis.gethash(command.m_option[0],"通知套接字");
        TcpSocket membersocket(stoi(member_fd));
        membersocket.SendMsg(L_BLUE+command.m_recvuid+":您的加群申请已被通过"+NONE);
    }
}

void RefuseAddMember(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_recvuid+"的申请加群列表",command.m_option[0]))
    {
        mysocket.SendMsg("nofind");
        return;
    }else{
        redis.removeMember(command.m_recvuid+"的申请加群列表",command.m_option[0]);
        vector<string> memberlist=redis.getFriendList(command.m_option[0],"群成员列表");

        for(const string& memberid:memberlist)
        {
            if(redis.gethash(command.m_option[0]+"群成员列表",memberid)!="群成员"&&command.m_uid!=memberid)
            {
                string apply=command.m_uid+"拒绝了"+command.m_option[0]+"的加群申请";
                string num=redis.gethash(memberid+"的未读消息","群聊消息");
                redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.rpushValue(memberid+"群聊消息",apply);

                if(redis.sismember("在线用户列表",memberid))
                {
                    string member_fd=redis.gethash(memberid,"通知套接字");
                    TcpSocket membersocket(stoi(member_fd));
                    membersocket.SendMsg(L_BLUE+apply+NONE);
                }
            }
        }
        mysocket.SendMsg("ok");
        if(redis.sismember("在线用户列表",command.m_option[0]))
        {
            string member_fd=redis.gethash(command.m_option[0],"通知套接字");
            TcpSocket membersocket(stoi(member_fd));
            membersocket.SendMsg(L_BLUE+command.m_recvuid+":您的加群申请被拒绝"+NONE);
        }
    }
}

void ChatGroup(TcpSocket mysocket,UserCommand command)
{
    if(!redis.hexists(command.m_uid+"的群聊列表",command.m_recvuid))
    {
        mysocket.SendMsg("no");
        exit(0);
    }
    mysocket.SendMsg("ok");

    //打印历史聊天记录
    vector<string>history=redis.lrangeAll(command.m_recvuid+"的群聊消息");
    for(const string& msg:history)
    {
        mysocket.SendMsg(msg);
    }

    //设置其聊天对象为群聊
    redis.hsetValue(command.m_uid,"聊天对象",command.m_recvuid);

    mysocket.SendMsg("历史聊天记录打印完毕");
}

void GroupSendMsg(TcpSocket mysocket,UserCommand command)
{
    //将新消息加入消息队列
    string newmsg=command.m_uid+":"+command.m_option[0];
    redis.rpushValue(command.m_recvuid+"的群聊消息",newmsg);

    //在发送者的页面展示消息
    string my_recvfd=redis.gethash(command.m_uid,"通知套接字");
    TcpSocket my_socket(stoi(my_recvfd));
    my_socket.SendMsg(L_WHITE + newmsg + NONE);

    string uid=command.m_uid;
    string msg1=uid+":"+command.m_option[0];

     vector<string> memberlist=redis.getFriendList(command.m_recvuid,"群成员列表");

        for(const string& memberid:memberlist)
        {
            if(!redis.sismember("在线用户列表",memberid)&&memberid!=uid)
            {
                string apply=command.m_recvuid+"群聊中有人发来了一条新消息";
                string num=redis.gethash(memberid+"的未读消息","群聊消息");
                redis.hsetValue(memberid+"的未读消息","群聊消息",to_string(stoi(num)+1));
                redis.rpushValue(memberid+"群聊消息",apply);

            }else if(redis.sismember("在线用户列表",memberid)&&(redis.gethash(memberid,"聊天对象")==command.m_recvuid)&&memberid!=uid)
            {
                string gr_recvfd=redis.gethash(memberid,"通知套接字");
                TcpSocket gr_socket(stoi(gr_recvfd));
                gr_socket.SendMsg(L_GREEN+msg1+NONE);

            }else if(redis.sismember("在线用户列表",memberid)&&memberid!=uid)
            {
                string apply=command.m_recvuid+"群聊中有人发来了一条新消息";
                string gr_recvfd=redis.gethash(memberid,"通知套接字");
                TcpSocket gr_socket(stoi(gr_recvfd));
                gr_socket.SendMsg(L_GREEN+apply+NONE);
            }
        }

        mysocket.SendMsg("ok");
        return;

}

void ExitChatGroup(TcpSocket mysocket,UserCommand command)
{
    redis.hsetValue(command.m_uid,"聊天对象","0");
    mysocket.SendMsg("ok");
    return ;
}