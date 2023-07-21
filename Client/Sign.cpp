#include<iostream>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<cstdlib>
#include "TcpSocket.cpp"
#include "UserCommand.hpp";

using namespace std;

void setup();
void Sign_menu();
int Login();
int Log_in(TcpSocket socket);
int Sign_up(TcpSocket socket);



//信号处理函数
void setup()
{
    signal(SIGINT,SIG_IGN);//忽略ctrl+c信号
    signal(SIGQUIT,SIG_IGN);//忽略ctrl+d信号
}

//注册登陆界面菜单
void Sign_menu()
{
    cout<<"---------------------welcom to chatroom--------------------"<<endl;
    cout<<"-                                                         -"<<endl;
    cout<<"-                          1.注册                         -"<<endl;
    cout<<"-                          2.登陆                         -"<<endl;
    cout<<"-                          3.退出                         -"<<endl;
    cout<<"-                                                         -"<<endl;
    cout<<"-----------------------------------------------------------"<<endl;
}

//登陆模块
int Login()
{
    TcpSocket mysocket;
    
    while(1)
    {
        int choice;

        Sign_menu();

        cout<<"请输入您的选项："<<endl;
        cin>>choice;

        if(cin.eof())//检查是否到达文件结尾即有ctrl+d信号的出现
        {
            cout << "Reached the end of the input" << endl;

            return 0;
        }

        cin.clear();//清除输入流的错误状态
        cin.sync();//清空输入缓冲区

        if(choice==1)
        {
            Sign_up(mysocket);

        }else if(choice==2)
        {
            Log_in(mysocket);

        }else if(choice==3)
        {
            exit(0);

        }else{
            cout<<"输入错误,请重新输入："<<endl;
            continue;
        }
    }
}

int Sign_up(TcpSocket mysocket)
{
    string nickname;//昵称
    string pwd,pwd2;//密码

    while(1){
        cin.sync();//清空缓冲区        
        cout<<"请输入昵称:"<<endl;
        getline(cin,nickname);//读入昵称

        cin.sync();//清空缓冲区
        cout<<"请输入密码:"<<endl;
        getline(cin,pwd);
        while(pwd.size()==0){
            cout<<"请重新输入有效的密码:"<<endl;
            getline(cin,pwd);
        }

        cout<<"请确认您的密码:"<<endl;
        getline(cin,pwd2);
        if(pwd!=pwd2)
        {
            cout<<"两次密码不一致,请重新操作:"<<endl;
            return -1;
        }else{
            break;
        }
        
    }
    UserCommand command("NULL",nickname,"让服务器返回一个uid",{pwd});
    int ret=mysocket.SendMsg(command.To_Json());//命令类转换为json格式，再转换为字符串格式，最后由套接字发送
    if(ret==0||ret==-1)
    {
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }

    string uid=mysocket.RecvMsg();//收到生成的uid
    if(uid=="close"){
        cout<<"对端已关闭"<<endl;
        exit(0);
    }
    cout<<"您注册的uid为:"<<uid<<endl;
}   