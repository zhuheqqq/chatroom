#ifndef __WRAP_H_
#define __WRAP_H_
#include<stddef.h>
#include<sys/types.h>
#include<ctype.h>
#include<string.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<errno.h>
#include<pthread.h>
#include<arpa/inet.h>
#include<iostream>
#include<strings.h>
#include<thread>


#define SERV_PORT 9000//设置端口号


void perr_exit(const char *s);
int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr);
int Bind(int fd,const struct sockaddr *sa,socklen_t salen);
int Connect(int fd,const struct sockaddr *sa,socklen_t salen);
int Listen(int fd,int backlog);
int Socket(int family,int type,int protocol);
ssize_t Read(int fd,void *ptr,size_t nbytes);
ssize_t Write(int fd,const void *ptr,size_t nbytes);
int Close(int fd);
ssize_t Readn(int fd,void *vptr,size_t n);
ssize_t Writen(int fd,const void *vptr,size_t n);
static ssize_t my_read(int fd,char *ptr);
ssize_t Readline(int fd,void *vptr,size_t maxlen);
int recvMsg(int cfd,char** msg);
int Pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void*(*start_routinue)(void *),
                   void *arg);
int Pthread_detach(pthread_t thread);



void perr_exit(const char *s)
{
    perror(s);
    exit(-1);
}

int Accept(int fd,struct sockaddr *sa,socklen_t *salenptr)
{
    int n;

again:
    if((n=accept(fd,sa,salenptr))<0)
    {
        if((errno==ECONNABORTED)||(errno==EINTR))
        {
            goto again;
        }else{
            perr_exit("accept error");
        }
    }

    return n;
}

int Bind(int fd,const struct sockaddr *sa,socklen_t salen)
{
    int n;

    if((n=bind(fd,sa,salen))<0)
    {
        perr_exit("bind_error");
    }
    return n;
}

int Connect(int fd,const struct sockaddr *sa,socklen_t salen)
{
    int n;

    if((n=connect(fd,sa,salen))<0)
    {
        perr_exit("connect error");
    }

    return n;
}

int Listen(int fd,int backlog)
{
    int n;

    if((n=listen(fd,backlog))<0)
    {
        perr_exit("listen error");
    }

    return n;
}

int Socket(int family,int type,int protocol)
{
    int n;

    if((n=socket(family,type,protocol))<0)
    {
        perr_exit("socket error");
    }

    return n;
}

int Close(int fd)
{
    int n;

    if((n=close(fd))==-1)
    {
        perr_exit("close error");
    }

    return n;
}

ssize_t Readn(int fd,void *vptr,size_t n)
{
    size_t nleft;//usigned int剩余未读取的字节数
    ssize_t nread;//int实际读到的字节数
    char *ptr;

    ptr=(char *)vptr;
    nleft=n;//n是未读取的字节数

    while(nleft>0)
    {
        if((nread=read(fd,ptr,nleft))<0)
        {
            if(errno==EINTR||EWOULDBLOCK)
            {
                nread=0;
            }else{
                return -1;
            }
        }else if(nread==0)
        {
            break;
        }

        nleft-=nread;
        ptr+=nread;
    }

    return n-nleft;
}

ssize_t Read(int fd,void *ptr,size_t nbytes)
{
    int n;

again:
    if((n=read(fd,ptr,nbytes))==-1)
    {
        if(errno==EINTR)
        {
            goto again;
        }else{
            return -1;
        }
    }

    return n;
}

ssize_t Write(int fd,const void *ptr,size_t nbytes)
{
    int n;
again:
    if((n=write(fd,ptr,nbytes))==-1)
    {
        if(errno==EINTR)
        {
            goto again;
        }else{
            return -1;
        }
    }
    return n;
}

ssize_t Writen(int fd,const void *vptr,size_t n)
{
    size_t nleft;
    ssize_t nwritten;
    const char *ptr;

    ptr=(char *)vptr;
    nleft=n;
    while(nleft>0)
    {
        if((nwritten=write(fd,ptr,nleft))<=0)
        {
            if(nwritten<0&&(errno==EINTR||EWOULDBLOCK))
            {
                nwritten=0;
            }else{
                return -1;
            }
            nleft-=nwritten;
            ptr+=nwritten;
        }
    }

    return n;
}

static ssize_t my_read(int fd,char *ptr)
{
    static int read_cnt;
    static char *read_ptr;
    static char read_buf[100];

    if(read_cnt<=0)
    {
again:
        if((read_cnt=read(fd,read_buf,sizeof(read_buf)))<0)
        {
            if(errno==EINTR)
            {
                goto again;
            }
            return -1;
        }else if(read_cnt==0)
        {
            return 0;
        }

        read_ptr=read_buf;
    }
    read_cnt--;
    *ptr=*read_ptr++;

    return 1;
}

ssize_t Readline(int fd,void *vptr,size_t maxlen)
{
    ssize_t n,rc;
    char c,*ptr;
    ptr=(char *)vptr;

    for(n=1;n<maxlen;n++)
    {
        if((rc=my_read(fd,&c))==1)
        {
            *ptr++=c;
            if(c=='\n')
            {
                break;
            }
        }else if(rc==0)
        {
            *ptr=0;
            return n-1;
        }else{
            return -1;
        }
    }
    *ptr=0;
    return n;
}

int Pthread_create(pthread_t *thread,const pthread_attr_t *attr,void *(*start_routine)(void *),void *arg)
                          {
                            int ret=pthread_create(thread,NULL,start_routine,arg);
                            if(ret!=0)
                            {
                                perror("pthread_create error");
                                exit(0);
                            }

                            return ret;
                          }


int Pthread_detach(pthread_t thread)
{
    int ret=pthread_detach(thread);
    if(ret!=0)
    {
        perror("pthread_detach error");
        exit(0);
    }

    return ret;
}

int recvMsg(int cfd,char** msg)//接受带数据头的数据包
{
    //接收数据头
    int len=0;
    Readn(cfd,(char *)&len,4);
    len=ntohl(len);
   // printf("数据块大小为%d\n",len);

    char *buf=(char *)malloc(len+1);//留出存储'\0'的位置
    int ret=Readn(cfd,buf,len);
    /*if(ret!=len)
    {
        printf("数据接收失败\n");
    }else if(ret==0){
        printf("对方断开连接\n");
        close(cfd);
    }*/

    buf[len]='\0';
    *msg=buf;

    return ret;//返回接收的字节数
}



#endif