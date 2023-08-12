#include "friendfunc.hpp"
#include "Menu.hpp"

using namespace std;

void AddGroup();//添加群聊
void CreateGroup();//创建群聊
int GroupList();//展示群聊列表
void AboutGroup();//群聊相关内容
void MemberList(string groupuid);//群成员列表
void DeleteGroup(string groupuid);//退出群聊
void ApplyList(string groupuid);//申请加群列表
//void AgreeAddMember();//同意加群
//void RefuseAddMember();//拒绝加群
void DeleteMember(string groupuid);//移除群成员
void AddManager(string groupuid);//增加管理员
void DeleteManager(string groupuid);//取消管理员身份
void DissolveGroup(string groupuid);//解散群聊
void ChatGroup(string groupuid);


void AddGroup()
{
    string groupuid;//群聊的uid
    cout<<"您想加入的群聊的uid为:"<<endl;
    cin>>groupuid;

    UserCommand command1(Curcommand.m_uid,"","",ADDGROUP,{groupuid});
    int ret=mysocket.SendMsg(command1.To_Json());
    if(ret==0||ret==-1)
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);

    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);

    }else if(recv=="handled")
    {
        cout<<"您已在该群聊中,无需反复加群"<<endl;
        return;

    }else if(recv=="none")
    {
        cout<<"未找到该群,请确认群聊uid是否正确"<<endl;
        return;

    }else if(recv=="ok"){
        cout<<"加群申请已发送,请耐心等待回复"<<endl;
        return;

    }else if(recv=="had"){
        cout<<"已发送过加群申请,请耐心等待回复"<<endl;
        return;

    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}

void CreateGroup()
{
    int flag=FriendList();
    if(flag==1)
    {
        cout<<"请选择一个您想拉入群聊的好友"<<endl;
        string frienduid;
        cin>>frienduid;

        UserCommand command1(Curcommand.m_uid,"","",CREATEGROUP,{frienduid});
        int ret=mysocket.SendMsg(command1.To_Json());
        if(ret==0||ret==-1)
        {
            cout<<"服务器端已关闭"<<endl;
            exit(0);

        }

        //接收回复
        string recv=mysocket.RecvMsg();
        if(recv=="close")
        {
            cout<<"服务器端已关闭"<<endl;
            exit(0);
        }else if(recv=="nofind")
        {
            cout<<"您暂时还没有该好友"<<endl;
            return;
        }else{
            cout<<"群聊创建成功,群号为:"<<recv<<endl;
            return;
        }
    }else{
        return;
    }
    
}

int GroupList()
{
    UserCommand command1(Curcommand.m_uid,"","",GROUPLIST,{""});//展示群聊列表
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    system("clear");

    string Group;
    while ((Group = mysocket.RecvMsg()) != "end")
    {
        if (Group == "none")
        {
            system("clear");
            cout << "您当前还没有加入群聊" << endl;
            return 0;
        }
        else if (Group == "close")
        {
            cout << "服务器端已关闭" << endl;
            exit(0);
        }
        else
        {
            cout << Group << endl;
        }
    }

    cout << "群聊列表展示完毕" << endl;
    cout<<"------------------------------------------------------------"<<endl;

    return 1;
}

void AboutGroup()
{
    while(1)
    {
        Group_menu1();//群聊详细功能菜单

        int option;//选项

        cout << "请输入您的选项：" << endl;
        cin >> option;

        if (cin.eof()) // 检查是否到达文件结尾即有ctrl+d信号的出现
        {
            cout << "Reached the end of the input" << endl;
            return ;
        }

        system("clear");

        cin.clear(); // 清除输入流的错误状态
        cin.sync();  // 清空输入缓冲区

        int flag=GroupList();//先展示群聊列表
        if(flag==1)
        {
            string groupuid;
            cout<<"您想查看详细信息的群聊uid为:"<<endl;
            cin>>groupuid;

            if(option!=9)
            {
                switch (option)
                {
                    case 1:
                        MemberList(groupuid);
                        break;
                    case 2:
                        DeleteGroup(groupuid);
                        break;
                    case 3:
                        ApplyList(groupuid);
                        break;
                    /*case 4:
                        AgreeAddMember();
                        break;
                    case 5:
                        RefuseAddMember();
                        break;*/
                    case 4:
                        DeleteMember(groupuid);
                        break;
                    case 5:
                        AddManager(groupuid);
                        break;
                    case 6:
                        DeleteManager(groupuid);
                        break;
                    case 7:
                        DissolveGroup(groupuid);
                        break;
                    case 8:
                        ChatGroup(groupuid);
                        break;
                    default:
                    system("clear");
                        cout<<"输入错误,请重新输入"<<endl;
                        continue;
                }
            }else{
                break;
            }
        }else{
            break;
        }
    }
}

void MemberList(string groupuid)//群成员列表
{
    UserCommand command1(Curcommand.m_uid,"","",MEMBERLIST,{groupuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string member=mysocket.RecvMsg();
    while(member!="end")
    {
        if(member=="close")
        {
            cout<<"服务器端已关闭"<<endl;
            exit(0);
        }else if(member=="none")
        {
            cout<<"您还没有该群聊"<<endl;
            return;
        }else{
            cout<<member<<endl;
        }

        member=mysocket.RecvMsg();
    }
    cout<<"群成员列表展示完毕"<<endl;
    cout<<"------------------------------------------------------------------"<<endl;
    return;
    
}

void DeleteGroup(string groupuid)
{
    UserCommand command1(Curcommand.m_uid,"","",DELETEGROUP,{groupuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="none")
    {
        cout<<"您不在此群聊中"<<endl;
        return;
    }else if(recv=="ok")
    {
        system("clear");
        cout<<"您已成功退出此群聊"<<endl;
        return;
    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}

void ApplyList(string groupuid)//申请加群列表
{
    UserCommand command1(Curcommand.m_uid,"","",APPLYLIST,{groupuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    while(recv!="end")
    {
        if(recv=="close")
        {
            cout<<"服务器端已关闭"<<endl;
            exit(0);
        }else if(recv=="no")
        {
            cout<<"您没有该群此操作权限"<<endl;
            return;
        }else
        {
            cout<<recv<<endl;
        }
        recv=mysocket.RecvMsg();
    }

    cout<<"申请加群列表展示完毕"<<endl;
    cout<<"------------------------------------------------------------------"<<endl;

    string agreeuid;

    cout<<"请输入您想同意加群的uid(输入exit表示停止操作):"<<endl;
    cin>>agreeuid;
    while(agreeuid!="exit")
    {
        UserCommand command1(Curcommand.m_uid,"",groupuid,AGREEADDMEMBER,{agreeuid});
        ret = mysocket.SendMsg(command1.To_Json());
        if (ret == 0||ret == -1)
        {
            cout << "服务器端已关闭" << endl;
            exit(0);
        }

        string agree_recv=mysocket.RecvMsg();
        if(agree_recv=="close")
        {
            cout<<"服务器端已关闭"<<endl;
            exit(0);
        }else if(agree_recv=="nofind")
        {
            cout<<"不存在该uid申请加群"<<endl;
        }else if(agree_recv=="ok")
        {
            cout<<"已成功通过该用户的加群申请"<<endl;
        }else{
            cout<<"其他错误"<<endl;
            return;
        }

        cout<<"请输入您想同意加群的uid(输入exit表示停止操作):"<<endl;
        cin>>agreeuid;
        
    }

        string refuseuid;
        cout<<"请输入您想拒绝加群的uid(输入exit表示停止操作):"<<endl;
        cin>>refuseuid;
        while(refuseuid!="exit")
        {
            UserCommand command1(Curcommand.m_uid,"",groupuid,REFUSEADDMEMBER,{refuseuid});
            ret = mysocket.SendMsg(command1.To_Json());
            if (ret == 0||ret == -1)
            {
                cout << "服务器端已关闭" << endl;
                exit(0);
            }

            string refuse_recv=mysocket.RecvMsg();
            if(refuse_recv=="close")
            {
                cout<<"服务器端已关闭"<<endl;
                exit(0);
            }else if(refuse_recv=="nofind")
            {
                cout<<"不存在该uid申请加群"<<endl;
            }else if(refuse_recv=="ok")
            {
                cout<<"已成功拒绝该用户的加群申请"<<endl;
            }else{
                cout<<"其他错误"<<endl;
                return;
            }

            cout<<"请输入您想拒绝加群的uid(输入exit表示停止操作):"<<endl;
            cin>>refuseuid;
        
    }

    return;
    
}

void DeleteMember(string groupuid)
{
    MemberList(groupuid);
    string deleteuid;
    cout<<"您想移除的群成员的uid为:"<<endl;
    cin>>deleteuid;

    UserCommand command1(Curcommand.m_uid,"",groupuid,DELETEMEMBER,{deleteuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="none")
    {
        cout<<"您不在此群聊中"<<endl;
        return;
    }else if(recv=="no")
    {
        cout<<"不好意思,您没有此操作权限"<<endl;
        return;
    }else if(recv=="ok")
    {
        cout<<"您已成功移除该群成员"<<endl;
        return;
    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}

void AddManager(string groupuid)
{
    MemberList(groupuid);
    string addmanager;
    cout<<"您想添加成管理员的群成员的uid为:"<<endl;
    cin>>addmanager;

    UserCommand command1(Curcommand.m_uid,"",groupuid,ADDMANAGER,{addmanager});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="none")
    {
        cout<<"您不在此群聊中"<<endl;
        return;
    }else if(recv=="handled")
    {
        cout<<"该成员已经是群管理员,无需反复添加"<<endl;
        return;
    }else if(recv=="no")
    {
        cout<<"不好意思,您没有此操作权限"<<endl;
        return;
    }else if(recv=="ok")
    {
        cout<<"您已成功添加"<<addmanager<<"为群管理员"<<endl;
        return;
    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}

void DeleteManager(string groupuid)
{
    MemberList(groupuid);
    string deletemanager;
    cout<<"您想移除的群管理的uid为:"<<endl;
    cin>>deletemanager;

    UserCommand command1(Curcommand.m_uid,"",groupuid,DELETEMANAGER,{deletemanager});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="none")
    {
       cout<<"您不在此群聊中"<<endl;
        return;
    }else if(recv=="handled")
    {
        cout<<"该成员不是管理员,无需取消管理员身份"<<endl;
        return;
    }else if(recv=="no")
    {
        cout<<"不好意思,您没有此操作权限"<<endl;
        return;
    }else if(recv=="ok")
    {
        cout<<"已成功取消该成员管理员身份"<<endl;
        return;
    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}

void DissolveGroup(string groupuid)
{
    cout<<"确定要解散群聊吗(yes/no)"<<endl;
    string choice;
    cin>>choice;

    if(choice=="yes")
    {
        UserCommand command1(Curcommand.m_uid,"","",DISSOLVEGROUP,{groupuid});
        int ret = mysocket.SendMsg(command1.To_Json());
        if (ret == 0||ret == -1)
        {
            cout << "服务器端已关闭" << endl;
            exit(0);
        }

        string recv=mysocket.RecvMsg();
        if(recv=="close")
        {
            cout<<"服务器端已关闭"<<endl;
            exit(0);
        }else if(recv=="no")
        {
            cout<<"不好意思,您没有此操作权限"<<endl;
            return;
        }else if(recv=="ok")
        {
            system("clear");
            cout<<L_RED<<"已成功解散此群聊"<<NONE<<endl;
            return;
        }else{
            cout<<"其他错误"<<endl;
            return;
        }

    }


}

void ChatGroup(string groupuid)
{
    UserCommand command1(Curcommand.m_uid,"",groupuid,CHATGROUP,{""});
    int ret=mysocket.SendMsg(command1.To_Json());
    if(ret==0||ret==-1)
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();
    if(recv=="close")
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }else if(recv=="no")
    {
        cout<<"您还未加入该群,请先加入该群"<<endl;
    }else if(recv=="ok")
    {
        //打印群聊历史聊天记录
        string historymsg;
        while(1)
        {
            historymsg=mysocket.RecvMsg();
            if(historymsg=="close")
            {
                cout<<"服务器端已关闭"<<endl;
                exit(0);
            }else if(historymsg=="历史聊天记录打印完毕")
            {
                break;
            }else{
                cout<<historymsg<<endl;
            }
        }

        string newmsg;
        while(1)
        {
            getline(cin,newmsg);
            if(newmsg==":exit")
            {
                //退出聊天
                UserCommand command_exit(Curcommand.m_uid,"",groupuid,EXITCHATGROUP,{""});
                int ret=mysocket.SendMsg(command_exit.To_Json());
                if(ret==0||ret==-1)
                {
                    cout<<"服务器端已关闭"<<endl;
                    exit(0);

                }

                string recv=mysocket.RecvMsg();
                if(recv=="close")
                {
                    cout<<"服务器端已关闭"<<endl;
                    exit(0);
                }else if(recv=="ok")
                {
                    cout<<"已成功退出聊天"<<endl;
                    return;
                }else{
                    cout<<"其他错误"<<endl;
                    return;
                }

                break;
            }

            //包装消息
            UserCommand command_msg(Curcommand.m_uid,"",groupuid,CHATSENDMSG,{newmsg});
            int ret=mysocket.SendMsg(command_msg.To_Json());
            if(ret==0||ret==-1)
            {
                cout<<"服务器端已关闭"<<endl;
                exit(0);
            }

            string recv=mysocket.RecvMsg();
            if(recv=="close")
            {
                cout<<"服务器端已关闭"<<endl;
                exit(0);
            }else if(recv=="ok")
            {
                continue;
            }
        }
    }
    return;
}