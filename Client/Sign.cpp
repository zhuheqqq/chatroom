#include<iostream>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<cctype>//isdigit的头文件
#include<cstdlib>
#include "../Classes/UserCommand.hpp"
#include "../Classes/TcpSocket.cpp"
#include "../Server/option.hpp"
#include "../Server/wrap.hpp"
#include "Menu.hpp"
//#include "client.cpp"

extern string IP;

using namespace std;
extern UserCommand Curcommand;
TcpSocket mysocket("recv");


struct RecvArg{
    string uid;
    int recv_fd=-1;
    RecvArg(string myuid,int recv_fd):uid(myuid),recv_fd(recv_fd){}
};

/*void *recvfunc(void *)
{
    
}*/

void setup();
void Sign_menu();
int Login();
int Log_in(TcpSocket mysocket);
int Sign_up(TcpSocket mysocket);
int Pass_re(TcpSocket mysocket);//找回密码函数
string get_uid();
void Func_menu();

void *recvfunc(void *arg)//线程处理函数
{
    RecvArg *recv_arg=static_cast<RecvArg*>(arg);//类型转换
    TcpSocket recvsocket(recv_arg->recv_fd);
    recvsocket.ConnectToHost(IP,PORT);
    UserCommand command(recv_arg->uid,"","",RECV,{""});//表示接收实时通知
    //cout<<"5"<<endl;
    int ret=recvsocket.SendMsg(command.To_Json());
    //cout<<"4"<<endl;
    if(ret==-1||ret==0)
    {
        cout<<"服务器端已关闭"<<endl;
        delete recv_arg;
        exit(0);
    }
    //cout<<"3"<<endl;

    while(1)//循环接收消息
    {
        //cout<<"2"<<endl;
        string recv=recvsocket.RecvMsg(); 
        if(recv=="close")
        {
            cout<<"服务器端已关闭"<<endl;
            delete recv_arg;
            exit(0);
        }
        //cout<<"1"<<endl;
        // 使用 ANSI 转义序列将光标移动到终端最底部并输出消息
        cout <<  recv << endl;
        //cout<<"0"<<endl;

    }

    return nullptr;
}

//信号处理函数
void setup()
{
    signal(SIGINT,SIG_IGN);//忽略ctrl+c信号
    signal(SIGQUIT,SIG_IGN);//忽略ctrl+\信号
    signal(SIGTSTP, SIG_IGN);//忽略ctrl+z信号
}

//获取用户的uid
string get_uid()
{
    string uid;
    int flag=1,c;
    
    cout<<"您的uid帐号为:"<<endl;
    cin.ignore();//忽略缓冲区剩余内容
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
    int flag=0;
    
    while(1)
    {
        Sign_menu();

        string option;
        cout<<"请输入您的选项:"<<endl;
        cin>>option;
       
        if(cin.eof())//检查是否到达文件结尾即有ctrl+d信号的出现
        {
            cout << "Reached the end of the input" << endl;
            //cin.ignore();
            //continue;
            
            return 0;

        }

        cin.clear();//清除输入流的错误状态
        cin.sync();//清空输入缓冲区


        if(option=="1")
        {
            Sign_up(mysocket);

        }else if(option=="2")
        {
            flag=Log_in(mysocket);
            if(flag==1){
                break;
            }
            
        }else if(option=="3")
        {
            //和密保问题作对比
            Pass_re(mysocket);

        }else if(option=="4")
        {
            exit(0);

        }else{
            cout<<"输入错误,请重新输入"<<endl;
            //continue;
        }
    }

    return 1;
}

int Sign_up(TcpSocket mysocket)//注册
{
    string pwd,pwd2;//密码
    string security_question;//密保问题

    while(1){
        cin.ignore();//忽略缓冲区多余字符 

        //cout<<"请输入昵称:"<<endl;
        //getline(cin,nickname);  

        //cin.ignore();//忽略缓冲区多余字符 
        
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

    cout<<"请回答下列问题，答案将作为您的密保问题"<<endl;
    cout<<"您最喜欢的书叫什么名字？"<<endl;
    getline(cin,security_question);


    UserCommand command("",security_question,"",SIGNUP,{pwd});//假设SIGNUP为让服务器随机生成一个uid
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
    Curcommand.m_uid=uid;
    Curcommand.m_question=security_question;
    cout<<L_RED<<"您注册的uid为:"<<uid<<",这是您身份的唯一标识,请牢记"<<NONE<<endl;

    cout<<"请登陆"<<endl;

    return 0;
}   


int Log_in(TcpSocket mysocket)//登陆
{
    string uid,pwd;
    uid=get_uid();
    Curcommand.m_uid=uid;

    //
    cout<<"请输入您的密码:"<<endl;
    getline(cin,pwd);
    
    UserCommand command(uid,"","",LOGIN,{pwd});//LOGIN含义为让服务器端比对密码
    
    int ret=mysocket.SendMsg(command.To_Json());//命令类转换为json格式，再转换为字符串格式，最后由套接字发送
    if(ret==0||ret==-1)
    {
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();//接收返回的结果
    cout<<recv<<endl;
    if(recv=="close")//接收服务器端返回的字符打印提示信息
    {
        cout<<"服务器已关闭"<<endl;
        exit(0);
       
    }else if(recv=="discorrect")
    {
        cout<<"密码错误,请重新输入："<<endl;
        return 0;

    }else if(recv=="nonexisent")
    {
        cout<<"帐号不存在,请先注册"<<endl;
        return 0;

    }else if(recv=="ok")
    {
        system("clear");
        cout<<"登陆成功"<<endl;
        pthread_t tid;
        RecvArg *recv_arg=new RecvArg(uid,mysocket.getresvfd());
        Pthread_create(&tid,NULL,&recvfunc,static_cast<void*>(recv_arg));//处理实时通知

        ret=Pthread_detach(tid);

        return 1;
    }

   
    return 1;

}

int Pass_re(TcpSocket mysocket)
{
    string uid,pwd,answer;
    uid=get_uid();
    Curcommand.m_uid=uid;

    cout<<"您最喜欢的书叫什么名字？"<<endl;
    getline(cin,answer);

    UserCommand command(uid,"","",PASSWORDRECOVERY,{""});
    int ret=mysocket.SendMsg(command.To_Json());//命令类转换为json格式，再转换为字符串格式，最后由套接字发送
    if(ret==0||ret==-1)
    {
        cout<<"服务器已关闭"<<endl;
        exit(0);
    }

    string recv=mysocket.RecvMsg();//接收返回的结果
    if(recv=="close")//接收服务器端返回的字符打印提示信息
    {
        cout<<"服务器已关闭"<<endl;
        exit(0);
       
    }else if(recv!=answer)
    {
        cout<<"您输入的答案不正确,无法进行下面的操作"<<endl;
        return 0;
    }else if(recv==answer)
    {
        cout<<"请输入您想修改的密码:"<<endl;
        cin>>pwd;
        UserCommand command1(uid,"","",CHANGEPASSWORD,{pwd});
        int ret=mysocket.SendMsg(command1.To_Json());//命令类转换为json格式，再转换为字符串格式，最后由套接字发送
        if(ret==0||ret==-1)
        {
            cout<<"服务器已关闭"<<endl;
            exit(0);
        }

        string recv=mysocket.RecvMsg();//接收返回的结果
        if(recv=="close")//接收服务器端返回的字符打印提示信息
        {
            cout<<"服务器已关闭"<<endl;
            exit(0);
        
        }else if(recv=="ok")
        {
            cout<<"密码已成功修改,请重新登陆"<<endl;
            return 1;
        }else{
            cout<<"其他错误"<<endl;
            return 0;
        }
    }
    return 1;

}

