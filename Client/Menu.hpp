#pragma once
#include<iostream>
#include<string>
using namespace  std;



//注册登陆界面菜单
void Sign_menu()
{
    cout<<"---------------------Welcom to chatroom--------------------"<<endl;
    cout<<"-                                                         -"<<endl;
    cout<<"-                        [1].注册                         -"<<endl;
    cout<<"-                        [2].登陆                         -"<<endl;
    cout<<"-                        [3].忘记密码                     -"<<endl;
    cout<<"-                        [4].退出                         -"<<endl;
    cout<<"-                                                         -"<<endl;
    cout<<"-----------------------------------------------------------"<<endl;
}

//功能函数菜单
void Func_menu()
{
    //system("clear");//清屛
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"-                   Welcome to chatroom                    -"<<endl;
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"-                                                          -"<<endl;
    cout<<"-                      [1].好友管理                        -"<<endl;
    cout<<"-                      [2].群聊管理                        -"<<endl;
    cout<<"-                      [3].注销账户                        -"<<endl;
    cout<<"-                      [4].退出                            -"<<endl;
    cout<<"-                                                          -"<<endl;
    cout<<"------------------------------------------------------------"<<endl;

}

//好友管理菜单
void Friend_menu()
{
    //system("clear");
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"-           Welcome to friend management interface         -"<<endl;
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"-                                                          -"<<endl;
    cout<<"-                      [1].查看好友列表                    -"<<endl;
    cout<<"-                      [2].添加好友                        -"<<endl;
    cout<<"-                      [3].删除好友                        -"<<endl;
    cout<<"-                      [4].同意好友申请                    -"<<endl;
    cout<<"-                      [5].拒绝好友申请                    -"<<endl;
    cout<<"-                      [6].屏蔽会话                        -"<<endl;
    cout<<"-                      [7].恢复会话                        -"<<endl;
    cout<<"-                      [8].查看好友在线状态                -"<<endl;
    cout<<"-                      [9].私聊                            -"<<endl;
    cout<<"-                      [10].查看未读消息                   -"<<endl;
    cout<<"-                      [11].退出                           -"<<endl;
    cout<<"-                                                          -"<<endl;
    cout<<"------------------------------------------------------------"<<endl;

}

//群聊管理菜单
void Group_menu()
{
    //system("clear");
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"-            Welcome to group management interface         -"<<endl;
    cout<<"------------------------------------------------------------"<<endl;
    cout<<"-                                                          -"<<endl;
    cout<<"-                      [1].查看群聊列表                    -"<<endl;
    cout<<"-                      [2].创建群聊                        -"<<endl;
    cout<<"-                      [3].添加群聊                        -"<<endl;
    cout<<"-                      [4].群聊相关                        -"<<endl;
    cout<<"-                      [5].退出                            -"<<endl;
    cout<<"-                                                          -"<<endl;
    cout<<"------------------------------------------------------------"<<endl;

}

void Group_menu1()
{
    cout<<"------------------------------------------------------------------"<<endl;
    cout<<"-               Welcome to group management interface            -"<<endl;
    cout<<"------------------------------------------------------------------"<<endl;
    cout<<"-                                                                -"<<endl;
    cout<<"-                      [1].查看成员列表                          -"<<endl;//功能ok
    cout<<"-                      [2].退出群聊                              -"<<endl;//ok
    cout<<"-                      [3].申请加群列表(管理员操作)              -"<<endl;//功能ok
    cout<<"-                      [4].移除群成员(管理员操作)                -"<<endl;//功能ok
    cout<<"-                      [5].添加管理员(群主操作)                  -"<<endl;//功能ok
    cout<<"-                      [6].取消管理员身份(群主操作)              -"<<endl;//功能ok
    cout<<"-                      [7].解散群聊(群主操作)                    -"<<endl;//有问题
    cout<<"-                      [8].群聊                                  -"<<endl;
    cout<<"-                      [9].退出                                  -"<<endl;
    cout<<"-                                                                -"<<endl;
    cout<<"------------------------------------------------------------------"<<endl;
}
