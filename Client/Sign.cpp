#include<iostream>
#include<sys/signal.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<cstdlib>
using namespace std;


//信号处理函数
void setup()
{
    signal(SIGINT,SIG_IGN);//忽略ctrl+c信号
    signal(SIGQUIT,SIG_IGN);//忽略ctrl+d信号
}

//注册登陆界面菜单
void Sign_menu()
{
    cout<<"-----------------------------------------------------------"<<endl;
    cout<<"-                                                         -"<<endl;
    cout<<"-                          1.注册                         -"<<endl;
    cout<<"-                          2.登陆                         -"<<endl;
    cout<<"-                          3.退出                         -"<<endl;
    cout<<"-                                                         -"<<endl;
    cout<<"-----------------------------------------------------------"<<endl;
}

int readn(char *buf,int size)
{
    int len=
}

//登陆模块
int Login()
{
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
            Sign_up();
        }else if(choice==2)
        {
            Log_in();
        }else if(choice==3)
        {
            exit(1);

        }else{
            cout<<"输入错误,请重新输入："<<endl;
            continue;
        }
    }
}