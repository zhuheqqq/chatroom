#include "Menu.hpp"
#include "Sign.cpp"
#include<chrono>
#include "../Server/wrap.hpp"
#include "../Classes/TcpSocket.hpp"
#include "../Classes/UserCommand.hpp"
#include "../Server/option.hpp"
 
extern TcpSocket mysocket;
UserCommand Curcommand;


void *recvfunc(void *arg);//线程处理函数
void Log_out();//注销函数
int FriendManage();//好友管理函数
void GroupManage();//群聊管理函数
int FriendList();//展示好友列表
int Add_Friend();//添加好友
int Delete_Friend();//删除好友
int AgreeAddFriend();//同意好友申请
int RefuseAddFriend();//拒绝好友申请
int Block_Friend();//屏蔽好友
int Restore_Friend();//恢复好友会话
int View_OnlineStatus();//查看好友在线状态
int ChatWithFriend();//聊天
void UnreadMessage();//查看未读消息
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


int main()
{
    //UserCommand Curcommand;


    mysocket.ConnectToHost("127.0.0.1", 9999);

    int ret = Login();
    if (ret == 1)
    {
        while (1)
        {
            Func_menu();

            int option; // 选项

            cout << "请输入您的选项：" << endl;
            cin >> option;

            if (cin.eof()) // 检查是否到达文件结尾即有ctrl+d信号的出现
            {
                cout << "Reached the end of the input" << endl;
                return 0;
            }

            system("clear");

            cin.clear(); // 清除输入流的错误状态
            cin.sync();  // 清空输入缓冲区

            if (option == 1)
            {
                // 跳转到好友管理界面
                FriendManage();
            }
            else if (option == 2)
            {
                // 跳转到群聊管理界面
                GroupManage();
            }
            else if (option == 3)
            {
                // 注销账户，主要是将redis中uid集合中的该uid删除
                Log_out();
                exit(0);
            }
            else
            {
                system("clear");
                cout << "输入错误,请重新输入:" << endl;
                continue;
            }
        }
    }
}

int FriendManage()
{
    while(1)
    {
        Friend_menu();
        int option;

        cout << "请输入您的选项：" << endl;
        cin >> option;

        if (cin.eof()) // 检查是否到达文件结尾即有ctrl+d信号的出现
        {
            cout << "Reached the end of the input" << endl;
            return 0;
        }

        system("clear");

        cin.clear(); // 清除输入流的错误状态
        cin.sync();  // 清空输入缓冲区

        if(option!=11)
        {
                switch (option)
            {
                case 1:
                    FriendList();
                    break;
                case 2:
                    Add_Friend();
                    break;
                case 3:
                    Delete_Friend();
                    break;
                case 4:
                    AgreeAddFriend();
                    break;
                case 5:
                    RefuseAddFriend();
                    break;
                case 6:
                    Block_Friend();
                    break;
                case 7:
                    Restore_Friend();
                    break;
                case 8:
                    View_OnlineStatus();
                    break;
                case 9:
                    ChatWithFriend();
                    break;
                case 10:
                    UnreadMessage();
                    break;
                default:
                system("clear");
                    cout<<"输入错误,请重新输入"<<endl;
                    continue;
            }
        }else{
            break;
        }
        
    }
    
    return 0;
}

void GroupManage()
{
    while(1)
    {
        Group_menu();//群聊功能菜单

        int option; // 选项

        cout << "请输入您的选项：" << endl;
        cin >> option;

        if (cin.eof()) // 检查是否到达文件结尾即有ctrl+d信号的出现
        {
            cout << "Reached the end of the input" << endl;
            return;
        }

        cin.clear(); // 清除输入流的错误状态
        cin.sync();  // 清空输入缓冲区

        if(option==1)
        {
            //查看群聊列表
            GroupList();
            //break;

        }else if(option==2)
        {
            //创建群聊
            CreateGroup();
            //break;

        }else if(option==3)
        {
            //添加群聊
            AddGroup();
            //break;

        }else if(option==4)
        {
            //群聊相关
            AboutGroup();
            //break;

        }else if(option==5)
        {
            system("clear");
            break;

        }else{
            system("clear");
            cout<<"输入错误,请重新输入"<<endl;
        }

    }
}

void Log_out() // 注销功能函数
{
    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",LOGOUT,{""});
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
    }else if(recv=="ok")
    {
        cout<<"您已成功注销账户,感谢您的使用"<<endl;
        return;
    }else{
        cout<<"其他错误"<<endl;
        return;
    }
}

int FriendList()
{
    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",FRIENDLIST,{""});//展示好友列表
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string Friend;
    while ((Friend = mysocket.RecvMsg()) != "end")
    {
        if (Friend == "none")
        {
            cout << "您当前还没有好友" << endl;
            return 0;
        }
        else if (Friend == "close")
        {
            cout << "服务器端已关闭" << endl;
            exit(0);
        }
        else
        {
            cout << Friend << endl;
        }
    }

    cout << "好友列表展示完毕,绿色表示好友在线" << endl;
    cout<<"------------------------------------------------------------"<<endl;

    return 1;
}

int Add_Friend()
{
    string recv_uid;//接收者的uid,或者说要添加好友的uid
    string option;//发送的验证消息
    cout<<"您想添加的好友的uid为:"<<endl; 
    cin>>recv_uid;

    cout<<"您想发送的验证消息为"<<endl;
    cin.ignore();
    getline(cin,option);

    //cout<< ":"<< Curcommand.m_uid<<endl;
    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,recv_uid,ADDFRIEND,{option});//将发送者和接收者的uid以及发送者发送的验证消息打包
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    string recv = mysocket.RecvMsg();
    if (recv == "close")
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    else if (recv == "ok")
    {
        system("clear");
        cout << "好友添加申请已发送,等待对方通过" << endl;
        return 1;
    }
    else if (recv == "exist")
    {
        system("clear");
        cout << "该用户已经是您的好友,无需反复添加" << endl;
        return 0;
    }else if(recv=="handle")
    {
        system("clear");
        cout<<"您已经向该好友发送过好友申请,请耐心等待回复"<<endl;
        return 1;
    }
    else if (recv == "apply")
    {
        system("clear");
        cout << "您的系统消息中存在对方发送的好友申请,请先回复" << endl;
        return 0;
    }
    else if(recv=="none")
    {
        cout << "该用户不存在" << endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Delete_Friend()
{
    FriendList();
    string deleteuid;//想删除的好友的uid

    cout<<"请输入您想删除的好友的uid:"<<endl;
    cin>>deleteuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",DELETEFRIEND,{deleteuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv = mysocket.RecvMsg();
    if (recv == "close")
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    else if (recv == "ok")
    {
        system("clear");
        cout << "您已成功删除该好友" << endl;
        return 1;
    }
    else if (recv == "none")
    {
        //system("clear");
        cout << "未找到该好友" << endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int AgreeAddFriend()
{
    string agreeuid;//同意好友申请的好友的uid
    cout<<"您想同意加好友申请的好友uid为:"<<endl;
    cin>>agreeuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",AGREEADDFRIEND,{agreeuid});
    int ret = mysocket.SendMsg(command1.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    string recv = mysocket.RecvMsg();
    if (recv == "close")
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }
    else if (recv == "ok")
    {
        system("clear");
        cout << "已通过" << agreeuid << "的好友申请" << endl;
        return 1;
    }else if(recv=="nofind")
    {
        cout<<"不存在该用户的好友申请"<<endl;
        return 1;
    }/*else if (recv == "handle")
    {
        cout << "已处理过" << Curcommand.m_uid << "的好友申请,无需重复处理" << endl;
        return 1;
    }*/else
    {
        cout << "其他错误" << endl;
        return 0;
    }
}

int RefuseAddFriend()
{
    FriendList();
    string refuseuid;//拒绝好友申请的好友的uid
    cout<<"您想拒绝加好友申请的uid为:"<<endl;
    cin>>refuseuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",REFUSEADDFRIEND,{refuseuid});
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
    }else if(recv=="nofind")
    {
        cout<<"不存在该用户的好友申请"<<endl;
        return 1;
    }else if(recv=="ok")
    {
        system("clear");
        cout<<"已拒绝"<<refuseuid<<"的好友申请"<<endl;
        return 1;
    }else{
        cout<<recv<<endl;
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Block_Friend()//屏蔽好友
{
    FriendList();
    string blockuid;//屏蔽好友
    cout<<"您想屏蔽该好友uid为:"<<endl;
    cin>>blockuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",BLOCKFRIEND,{blockuid});
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
    }else if(recv=="ok")
    {
        cout<<"您已屏蔽该好友"<<endl;
        return 1;

    }else if(recv=="none")
    {
        cout<<"未找到该好友"<<endl;
        return 0;
    }else if(recv=="handled")
    {
        system("clear");
        cout<<"您已屏蔽过该好友,无需重复操作"<<endl;
        return 0;
    }else
    {
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Restore_Friend()
{
    string restoreuid;//恢复好友会话
    cout<<"您想恢复会话的好友uid为:"<<endl;
    cin>>restoreuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",RESTOREFRIEND,{restoreuid});
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
    }else if(recv=="ok")
    {
        cout<<"已恢复与该好友的会话"<<endl;
        return 1;
    }else if(recv=="none")
    {
        cout<<"未找到该好友"<<endl;
        return 0;
    }else if(recv=="no")
    {
        system("clear");
        cout<<"该好友未被屏蔽"<<endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int View_OnlineStatus()//已实现
{
    string viewuid;
    cout<<"您想查看在线状态的好友uid为:"<<endl;
    cin>>viewuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",VIEWONLINESTATUS,{viewuid});
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
        cout<<"该用户不是您的好友"<<endl;
        return 0;
    }else if(recv=="ok")
    {
        cout<<"用户"<<viewuid<<"现在在线,快去找ta聊天吧"<<endl;
        return 1;
    }else if(recv=="no")
    {
        cout<<"用户"<<viewuid<<"现在不在线,有事请留言"<<endl;
        return 1;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int ChatWithFriend()
{
    FriendList();
    //发送私聊请求
    string recvuid;
    cout<<"您想聊天好友的uid为:"<<endl;
    cin>>recvuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,recvuid,CHATWITHFRIEND,{""});
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
    }/*else if(recv=="none")
    {
        cout<<"您还没有好友,请先添加好友"<<endl;
        return 0;
    }*/else if(recv=="nofind")
    {
        cout<<"在好友列表中没有找到该好友"<<endl;
        return 0;
    }else if(recv=="ok")
    {
        //成功与好友建立联系，打印历史聊天记录
        string historymsg;
        while(1)
        {
            historymsg=mysocket.RecvMsg();
            if(historymsg=="close")
            {
                cout<<"服务器端已关闭"<<endl;
                exit(0);
            }else if(historymsg=="历史聊天记录展示完毕")//未打印
            {
                break;
            }else{
                cout<<historymsg<<endl;
            }
        }

        //获取新的聊天会话
        string newmsg;
        while(1)
        {
            //cin.ignore();//忽略当前缓冲区中的内容
            //cin.sync();
            getline(cin,newmsg);
            if(newmsg=="exit")//代表用户想退出聊天
            {
                //退出聊天
                UserCommand command_exit(Curcommand.m_uid,"",recvuid,EXITCHAT,{""});
                int ret=mysocket.SendMsg(command_exit.To_Json());
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
                }else if(recv=="ok")
                {
                    cout<<"已成功退出聊天"<<endl;
                    return 1;
                }else{
                    cout<<"其他错误"<<endl;
                    return 0;
                }

                break;
            }

            //包装消息
            UserCommand command_msg(Curcommand.m_uid,"",recvuid,SENDMSG,{newmsg});
            int ret = mysocket.SendMsg(command_msg.To_Json());
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
                return 0;
            }else if(recv=="ok")
            {
                continue;
            }
        }
    }
    return 1;

}

void UnreadMessage()
{
    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",UNREADMESSAGE,{""});//展示未读消息
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
        cout<<"您暂时还没有未读消息"<<endl;
        return;
    }else
    {
        cout<<recv<<endl;
    }
    
}

void AddGroup()
{
    string groupuid;//群聊的uid
    cout<<"您想加入的群聊的uid为:"<<endl;
    cin>>groupuid;

    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",ADDGROUP,{groupuid});
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
    UserCommand command1(Curcommand.m_uid,Curcommand.m_nickname,"",GROUPLIST,{""});//展示群聊列表
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

            if(option!=8)
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
            cout<<"已成功解散此群聊"<<endl;
            return;
        }else{
            cout<<"其他错误"<<endl;
            return;
        }

    }


}

