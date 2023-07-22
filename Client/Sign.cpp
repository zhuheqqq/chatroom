#include<iostream>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<cctype>//isdigit的头文件
#include<cstdlib>
#include "../Classes/UserCommand.hpp"
#include "../Classes/TcpSocket.cpp"
#include "../Server/func.hpp"
#include "../Server/wrap.hpp"
#include "Menu.hpp"

using namespace std;

void setup();
void Sign_menu();
int Login();
int Log_in(TcpSocket socket);
int Sign_up(TcpSocket socket);
string get_uid();
void Func_menu();



//信号处理函数
void setup()
{
    signal(SIGINT,SIG_IGN);//忽略ctrl+c信号
    signal(SIGQUIT,SIG_IGN);//忽略ctrl+d信号
}

//获取用户的uid
string get_uid()
{
    string uid;
    int flag=1,c;
    
    cout<<"您的uid帐号为:"<<endl;
    cin.ignore();//清空缓冲区
    getline(cin,uid);//获取输入的uid

    for(int c:uid)
    {
        if(!isdigit(c))
        {
            flag=0;
            break;
        }
    }

    while(flag==0)
    {
        cout<<"您输入的uid不全为数字,请重新输入:"<<endl;
        cin.ignore();
        getline(cin,uid);

        flag=1;
        for(int c:uid)
        {
            if(!isdigit(c))
            {
                flag=0;
                break;
            }
        }
    }

    return uid;
    
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

int Sign_up(TcpSocket mysocket)//注册
{
    string nickname;//昵称
    string pwd,pwd2;//密码

    while(1){
        cin.ignore();//忽略缓冲区多余字符        
        cout<<"请输入昵称:"<<endl;
        getline(cin,nickname);//读入昵称
        
       // cin.sync();//清空缓冲区,使用这个函数导致nickname无法输入
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
            system("clear");
            cout<<"两次密码不一致,请重新操作"<<endl;
            return -1;
        }else{
            break;
        }
        
    }
    UserCommand command("NULL",nickname,SIGNUP,{pwd});//假设1为让服务器随机生成一个uid
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

    return 0;
}   


int Log_in(TcpSocket mysocket)//登陆
{
    string uid,password;
    uid=get_uid();

    //cin.ignore();
    cout<<"请输入您的密码:"<<endl;
    getline(cin,password);

    UserCommand command(uid,"NULL",LOGIN,{password});//LOGIN含义为让服务器端比对密码
    int ret=mysocket.SendMsg(command.To_Json());//命令类转换为json格式，再转换为字符串格式，最后由套接字发送
    if(ret==0||ret==-1)
    {
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }

    string resv=mysocket.RecvMsg();//接收返回的结果
    if(resv=="close")//接收服务器端返回的字符打印提示信息
    {
        cout<<"服务器已关闭"<<endl;
    }else if(resv=="discorrect")
    {
        cout<<"密码错误"<<endl;

    }else if(resv=="nonexisent")
    {
        cout<<"帐号不存在,请先注册"<<endl;
        return 0;

    }else if(resv=="ok")
    {
        cout<<"登陆成功"<<endl;

        pthread_t tid;
       // Pthread_create(&tid,NULL,tfn,(void *)uid);//tfn功能为将用户uid添加到在线列表中

        //放一个登陆成功之后进行下一步选择的函数
        Func_menu();//功能菜单函数
    }

    return 0;

}

// int main()
// {
//     Login();
//     Func_menu();
// }


