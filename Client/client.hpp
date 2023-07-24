#include "Menu.hpp"
#include "Sign.cpp"
#include "../Server/wrap.hpp"
#include "../Classes/TcpSocket.hpp"
#include "../Classes/UserCommand.hpp"

void Log_out(TcpSocket mysocket)//注销功能函数
{
    mysocket.SendMsg("Log_out");
}