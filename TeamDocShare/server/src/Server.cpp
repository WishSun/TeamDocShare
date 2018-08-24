/*************************************************************************
	> File Name: Server.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时19分22秒
 ************************************************************************/

#include "./Server.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <time.h>

#define MAX_EVENT_NUMBER 10000


/*----------------------非成员函数---------------------*/

/* 设置描述符超时时间*/
void setSocketTimeout( int fd )
{
    struct timeval timeout = {3, 0};

    /* 设置发送超时*/
    setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(struct timeval));
    /* 设置接收超时*/
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(struct timeval));
}

/*-----------------------private-----------------------*/

Server* Server::m_pServer = NULL;

/* 构造函数*/
Server::Server(char *ip, int port)
{
    strcpy(m_IP, ip);
    m_port = port;
}


/* 创建监听socket并绑定地址和端口*/
bool Server::CreateSocket()
{
    if( (m_listenFd = socket( AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror("socket");
        return false;
    }

    struct sockaddr_in  address;
    address.sin_family = AF_INET;
    inet_pton( AF_INET, m_IP, &address.sin_addr );
    address.sin_port = htons( m_port );

    int op = 1;
    setsockopt( m_listenFd, SOL_SOCKET, SO_REUSEADDR, &op, sizeof(op) );

    if( bind( m_listenFd, (struct sockaddr*)&address, sizeof( address ) ) < 0 )
    {
        perror("bind");
        return false;
    }

    if( listen( m_listenFd, 5 ) < 0 )
    {
        perror("listen");
        return false;
    }

    return true;
}


/* 创建epoll事件监听集合，并将监听socket添加到epoll事件监听集合*/
bool Server::CreateEpoll()
{
    if( (m_epollFd = epoll_create( 5 )) < 0 )
    {
        perror("epoll_create");
        return false;
    }

    EpollAdd( m_listenFd, false );
    return true;
}


/* 将指定描述符添加到epoll事件监听集合*/
void Server::EpollAdd(int fd, bool timeout)
{
    epoll_event event;
    event.data.fd = fd;

    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;

    epoll_ctl( m_epollFd, EPOLL_CTL_ADD, fd, &event );

    if( timeout )
    {
        /* 设置描述符超时时间*/
        setSocketTimeout( fd );
    }
}


/* 将指定描述符从epoll监听集合中删除*/
void Server::EpollDel(int fd)
{
    epoll_ctl( m_epollFd, EPOLL_CTL_DEL, fd, 0 );
}


/*-----------------------public-----------------------*/

/* 创建服务器类单例对象并返回*/
Server* Server::CreateServer(char *ip, int port)
{
    if( m_pServer == NULL )
    {
        m_pServer = new Server(ip, port);
    }

    return m_pServer;
}


/* 析构函数*/
Server::~Server()
{

}


 /* 运行服务器程序*/
void Server::Run()
{
    if( ! CreateSocket() )
    {
        exit(1);
    }
    if( ! CreateEpoll() )
    {
        exit(1);
    }

    epoll_event  events[ MAX_EVENT_NUMBER ];

    while( true )
    {
        printf("开始监听...\n");
        int num = epoll_wait( m_epollFd, events, MAX_EVENT_NUMBER, -1 ); 
        if( num < 0 )
        {
            perror("epoll_wait");
            exit(1);
        }

        for( int i = 0; i < num; i++ )
        {
            int sockfd = events[i].data.fd;

            /* 有新连接到来*/
            if( sockfd == m_listenFd )
            {
                printf("有新连接到来\n\n");
            }
            /* 客户端有数据到来*/
            else if( events[i].events & EPOLLIN )
            {
                
            }
            else if( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) )
            {
                
            }
        }
    }
}

