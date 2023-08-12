#include "Menu.hpp"
#include "Sign.cpp"
//#include "friendfunc.hpp"
#include "groupfunc.hpp"
#include<chrono>
#include "../Server/wrap.hpp"
#include "../Classes/TcpSocket.hpp"
#include "../Classes/UserCommand.hpp"
#include "../Server/option.hpp"
 
extern TcpSocket mysocket;
UserCommand Curcommand;


void *recvfunc(void *arg);//线程处理函数

int main()
{
    //UserCommand Curcommand;

    setup();

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
            }else if (option == 2)
            {
                // 跳转到群聊管理界面
                GroupManage();
            }else if (option == 3)
            {
                // 注销账户，主要是将redis中uid集合中的该uid删除
                Log_out();
                exit(0);
            }else if(option==4)
            {
                break;
            }else
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





