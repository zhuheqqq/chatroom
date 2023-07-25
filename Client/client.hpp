#include "Menu.hpp"
#include "Sign.cpp"
#include "../Server/wrap.hpp"
#include "../Classes/TcpSocket.hpp"
#include "../Classes/UserCommand.hpp"

void Log_out(TcpSocket mysocket)//注销功能函数
{
    mysocket.SendMsg("Log_out");
}

int FriendManage(TcpSocket mysocket,UserCommand command);
bool FriendList(TcpSocket mysocket,UserCommand command);
bool Add_Friend(TcpSocket mysocket,UserCommand command);
bool AgreeAddFriend(TcpSocket mysocket,UserCommand command);


int main()
{
    TcpSocket mysocket;
    UserCommand command;
    
    mysocket.ConnectToHost("127.0.0.1",9999);

    int ret=Login();
    if(ret==1)
    {
        while(1)
        {
            Func_menu();

            int option;//选项

            cout<<"请输入您的选项："<<endl;
            cin>>option;

            if(cin.eof())//检查是否到达文件结尾即有ctrl+d信号的出现
            {
                cout << "Reached the end of the input" << endl;
                return 0;
            }

            cin.clear();//清除输入流的错误状态
            cin.sync();//清空输入缓冲区

            if(option==1)
            {
                //跳转到好友管理界面
                FriendManage(mysocket,command);

            }else if(option==2)
            {
                //跳转到群聊管理界面
            }else if(option==3)
            {
                //注销账户，主要是将redis中uid集合中的该uid删除
            }else{
                cout<<"输入错误,请重新输入:"<<endl;
                continue;
            }
        }
    }
    
}

int  FriendManage(TcpSocket mysocket,UserCommand command)
{
    Friend_menu();
    int option;

    cout<<"请输入您的选项："<<endl;
    cin>>option;

    if(cin.eof())//检查是否到达文件结尾即有ctrl+d信号的出现
    {
        cout << "Reached the end of the input" << endl;
        return 0;
    }

    cin.clear();//清除输入流的错误状态
    cin.sync();//清空输入缓冲区

    switch (option)
    {
    case 2:
        Add_Friend(mysocket,command);
        break;
    case 4:
        AgreeAddFriend(mysocket,command);
    
    
    default:
        break;
    }

}

bool Add_Friend(TcpSocket mysocket,UserCommand command)
{
    int ret=mysocket.SendMsg(command.To_Json());
    if(ret==0|ret==-1)
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
        cout<<"好友添加申请已发送,等待对方通过"<<endl;
        return true;
    }else if(recv=="exist")
    {
        cout<<"该用户已经是您的好友,无需反复添加"<<endl;
        return false;
    }else if(recv=="apply")
    {
        cout<<"您的系统消息中存在对方发送的好友申请,请先回复"<<endl;
    }else if(recv=="cannot")
    {
        cout<<"您已经向该用户发送过好友申请,请等待回复"<<endl;
    }else{
        cout<<"该用户不存在"<<endl;
        return false;
    }
}

bool AgreeAddFriend(TcpSocket mysocket,UserCommand command)
{
    int ret=mysocket.SendMsg(command.To_Json());
    if(ret==0|ret==-1)
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
        cout<<"已通过"<<command.m_uid<<"的好友申请"<<endl;
        return true;
    }else if(recv=="refuse")
    {
        cout<<"已拒绝"<<command.m_uid<<"的好友申请"<<endl;
    }else{
        cout<<"其他错误"<<endl;
        return false;
    }
}

bool FriendList(TcpSocket mysocket,UserCommand command)
{
    int ret=mysocket.SendMsg(command.To_Json());
    if(ret==0|ret==-1)
    {
        cout<<"服务器端已关闭"<<endl;
        exit(0);
    }

    while(1)
    {
        string Friend=mysocket.RecvMsg();
        if(Friend=="end")
        {
            cout<<"好友列表展示完毕"<<endl;
            break;
        }else if(Friend=="none")
        {
            cout<<"您当前还没有好友"<<endl;
            return false;
        }else if(Friend=="close")
        {
            cout<<"服务器端已关闭"<<endl;
            exit(0);
        }else{
            cout<<Friend<<endl;
        }
    }

    return true;

}

