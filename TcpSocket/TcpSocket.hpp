#include<arpa/inet.h>
#include<string>
#include<cstring>
#include<sys/socket.h>
#include<iostream>

using namespace  std;

class TcpSocket
{
public:
    TcpSocket();
    TcpSocket(int fd);
    ~TcpSocket();
    int SendMsg(string msg);
    string RecvMsg();

    int readn(char * buf,int size);
    int writen(const char *msg,int size);


private:
    int fd=-1;
};