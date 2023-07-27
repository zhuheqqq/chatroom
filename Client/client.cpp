#include "Menu.hpp"
#include "Sign.cpp"
#include "../Server/wrap.hpp"
#include "../Classes/TcpSocket.hpp"
#include "../Classes/UserCommand.hpp"
 
// TcpSocket mysocket;

void Log_out(TcpSocket mysocket) // 注销功能函数
{
    mysocket.SendMsg("Log_out");
}

int FriendManage(TcpSocket mysocket, UserCommand command);
int FriendList(TcpSocket mysocket, UserCommand command);
int Add_Friend(TcpSocket mysocket, UserCommand command);
int Delete_Friend(TcpSocket mysocket, UserCommand command);
int AgreeAddFriend(TcpSocket mysocket, UserCommand command);
int RefuseAddFriend(TcpSocket mysocket,UserCommand command);
int Block_Friend(TcpSocket mysocket, UserCommand command);
int Restore_Friend(TcpSocket mysocket,UserCommand command);
int View_OnlineStatus(TcpSocket mysocket,UserCommand command);
int ChatWithFriend(TcpSocket mysocket,UserCommand command);

int main()
{
    UserCommand command;

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

            cin.clear(); // 清除输入流的错误状态
            cin.sync();  // 清空输入缓冲区

            if (option == 1)
            {
                // 跳转到好友管理界面
                FriendManage(mysocket, command);
            }
            else if (option == 2)
            {
                // 跳转到群聊管理界面
            }
            else if (option == 3)
            {
                // 注销账户，主要是将redis中uid集合中的该uid删除
            }
            else
            {
                cout << "输入错误,请重新输入:" << endl;
                continue;
            }
        }
    }
}

int FriendManage(TcpSocket mysocket, UserCommand command)
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

    cin.clear(); // 清除输入流的错误状态
    cin.sync();  // 清空输入缓冲区

    switch (option)
    {
        case 1:
            FriendList(mysocket,command);
            break;
        case 2:
            Add_Friend(mysocket, command);
            break;
        case 3:
            Delete_Friend(mysocket,command);
            break;
        case 4:
            AgreeAddFriend(mysocket, command);
            break;
        case 5:
            RefuseAddFriend(mysocket, command);
            break;
        case 6:
            Block_Friend(mysocket, command);
            break;
        case 7:
            Restore_Friend(mysocket, command);
            break;
        case 8:
            View_OnlineStatus(mysocket, command);
            break;
        case 9:
            ChatWithFriend(mysocket, command);
            break;
        default:
            cout<<"输入错误,请重新输入："<<endl;
            break;
    }
    return 0;
}

int FriendList(TcpSocket mysocket, UserCommand command)
{
    UserCommand command1(command.m_uid,"",FRIENDLIST,{""});//展示好友列表
    int ret = mysocket.SendMsg(command.To_Json());
    if (ret == 0||ret == -1)
    {
        cout << "服务器端已关闭" << endl;
        exit(0);
    }

    while (1)
    {
        string Friend = mysocket.RecvMsg();
        if (Friend == "end")
        {
            cout << "好友列表展示完毕" << endl;
            break;
        }
        else if (Friend == "none")
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

    return 1;
}

int Add_Friend(TcpSocket mysocket, UserCommand command)
{
    string recv_uid;//接收者的uid,或者说要添加好友的uid
    string option;//发送的验证消息
    cout<<"您想添加的好友的uid为:"<<endl;
    cin>>recv_uid;

    cout<<"您想发送的验证消息为"<<endl;
    cin.ignore();
    getline(cin,option);

    
    UserCommand command1(command.m_uid,recv_uid,ADDFRIEND,{option});//将发送者和接收者的uid以及发送者发送的验证消息打包
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
        cout << "好友添加申请已发送,等待对方通过" << endl;
        return 1;
    }
    else if (recv == "exist")
    {
        cout << "该用户已经是您的好友,无需反复添加" << endl;
        return 0;
    }
    else if (recv == "apply")
    {
        cout << "您的系统消息中存在对方发送的好友申请,请先回复" << endl;
    }
    else if (recv == "handled")
    {
        cout << "您已经向该用户发送过好友申请,请等待回复" << endl;
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

int Delete_Friend(TcpSocket mysocket, UserCommand command)
{
    string deleteuid;//想删除的好友的uid

    cout<<"请输入您想删除的好友的uid:"<<endl;
    cin>>deleteuid;

    UserCommand command1(command.m_uid,"",DELETEFRIEND,{deleteuid});
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
        cout << "您已成功删除该好友" << endl;
        return 1;
    }
    else if (recv == "none")
    {
        cout << "未找到该好友" << endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int AgreeAddFriend(TcpSocket mysocket, UserCommand command)
{
    string agreeuid;//同意好友申请的好友的uid
    cout<<"您想同意加好友申请的好友uid为:"<<endl;
    cin>>agreeuid;

    UserCommand command1(command.m_uid,"",AGREEADDFRIEND,{agreeuid});
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
        cout << "已通过" << command.m_uid << "的好友申请" << endl;
        return 1;
    }else
    {
        cout << "其他错误" << endl;
        return 0;
    }
}

int RefuseAddFriend(TcpSocket mysocket,UserCommand command)
{
    string refuseuid;//拒绝好友申请的好友的uid
    cout<<"您想拒绝加好友申请的uid为:"<<endl;
    cin>>refuseuid;

    UserCommand command1(command.m_uid,"",REFUSEADDFRIEND,{refuseuid});
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
        cout<<"已拒绝"<<command.m_uid<<"的好友申请"<<endl;
        return 1;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Block_Friend(TcpSocket mysocket, UserCommand command)//屏蔽好友
{
    string blockuid;//屏蔽好友
    cout<<"您想屏蔽该好友uid为:"<<endl;
    cin>>blockuid;

    UserCommand command1(command.m_uid,"",BLOCKFRIEND,{blockuid});
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
        cout<<"您已屏蔽过该好友,无需重复操作"<<endl;
    }else
    {
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int Restore_Friend(TcpSocket mysocket,UserCommand command)
{
    string restoreuid;//恢复好友会话
    cout<<"您想屏蔽该好友uid为:"<<endl;
    cin>>restoreuid;

    UserCommand command1(command.m_uid,"",RESTOREFRIEND,{restoreuid});
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
        cout<<"该好友未被屏蔽"<<endl;
        return 0;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int View_OnlineStatus(TcpSocket mysocket,UserCommand command)
{
    string viewuid;//屏蔽好友
    cout<<"您想查看在线状态的好友uid为:"<<endl;
    cin>>viewuid;

    UserCommand command1(command.m_uid,"",VIEWONLINESTATUS,{viewuid});
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
        cout<<"用户"<<command.m_uid<<"现在在线,快去找ta聊天吧"<<endl;
        return 1;
    }else if(recv=="no")
    {
        cout<<"用户"<<command.m_uid<<"现在不在线,有事请留言"<<endl;
        return 1;
    }else{
        cout<<"其他错误"<<endl;
        return 0;
    }
}

int ChatWithFriend(TcpSocket mysocket,UserCommand command)
{

}



