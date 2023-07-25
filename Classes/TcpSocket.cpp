#include<nlohmann/json.hpp>
#include "TcpSocket.hpp"
#include "../Server/wrap.hpp"

TcpSocket::TcpSocket()
{
    m_fd=Socket(AF_INET,SOCK_STREAM,0);
}

TcpSocket::TcpSocket(int fd):m_fd(fd)//要用初始化列表不能在函数体内赋值
{
}

// TcpSocket::TcpSocket(string msg)
// {
//     if(msg=="recv")
//     {
//         fd=Socket(AF_INET,SOCK_STREAM,0);
//         recv_fd=Socket(AF_INET,SOCK_STREAM,0);
//     }
// }

TcpSocket::~TcpSocket(){}//析构函数

int TcpSocket::readn(char *buf,int size)
{
    char *pt=buf;
    int cnt=size;

    while(cnt>0)
    {
        int len=recv(m_fd,pt,cnt,0);
        if(len==-1)
        {
            close(m_fd);
            perr_exit("read error");
        
        }else if(len==0)//表示连接结束
        {
            cout<<"连接结束"<<endl;
            close(m_fd);
            return size-cnt;
        }

        pt+=len;
        cnt-=len;
    }

    return size-cnt;

}

int TcpSocket::writen(const char *msg,int size)
{
    const char *buf=msg;
    int cnt=size;
    while(cnt>0)
    {
        int len=send(m_fd,buf,cnt,0);//发送字节数大小
        if(len==-1)//发生错误
        {
            close(m_fd);
            perr_exit("writen error");

        }else if(len==0)
        {
            continue;
        }

        buf+=len;
        cnt-=len;//计算剩余待发送量
    }
    return size;//返回成功发送的字节数
}

int TcpSocket::SendMsg(string msg)
{
    char *data=new char[msg.size()+4];//开辟地址空间
    int biglen=htonl(msg.size());//字节序转换

    memcpy(data,&biglen,4);//将数据头先复制到data中
    memcpy(data+4,msg.data(),msg.size());//将后续字符串复制到data中

    int ret=writen(data,msg.size()+4);//发送数据

    delete[]data;//释放内存空间

    return ret;
}

string TcpSocket::RecvMsg()
{
    //接收数据头
    int len=0;
    int ret=readn((char *)&len,4);
    if(ret==0){
        return "close1";
    }
    len=ntohl(len);

    char *buf=new char[len+1];

    ret=readn(buf,len);
    if(ret!=len)
    {
        return to_string(len);
    }else if(ret==0)
    {
        close(m_fd);
        return "close";
    }

    buf[len]='\0';
    string msg(buf);
    delete[]buf;

    return msg;
}


int TcpSocket::ConnectToHost(string ip,unsigned short port)
{
    //连接服务器ip port
    struct sockaddr_in saddr;
    saddr.sin_family=AF_INET;
    saddr.sin_port=htons(port);
    inet_pton(AF_INET,ip.data(),&saddr.sin_addr.s_addr);
    int ret=Connect(m_fd,(struct sockaddr*)&saddr,sizeof(saddr));

    return ret;

}


