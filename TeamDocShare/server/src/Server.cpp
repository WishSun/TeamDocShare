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
#include <errno.h>

/* 最大描述符*/
#define MAX_FD 65536

/* 最大监听事件数*/
#define MAX_EVENT_NUMBER 10000

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
void Server::EpollAdd(int fd, bool noneblock)
{
    Common::EpollAddCore(m_epollFd, fd, noneblock);
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

struct UserRequest
{
    Protocol  prot;
    int haveRecv;
    int needRecv;
    UserRequest(): haveRecv(0),needRecv(-1){}
};

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

    /* 创建线程池对象*/
    m_pMyThreadpool = MyThreadPool::CreateMyThreadPool(m_epollFd);

    epoll_event  events[ MAX_EVENT_NUMBER ];
    UserRequest *pUsersReqest = new UserRequest[ MAX_FD ];


    while( true )
    {
        int num = epoll_wait( m_epollFd, events, MAX_EVENT_NUMBER, -1 ); 
        if( num < 0 )
        {
            if( errno != EINTR )
            {
                perror("epoll_wait");
                exit(1);
            }
        }

        for( int i = 0; i < num; i++ )
        {
            int sockfd = events[i].data.fd;

            /* 有新连接到来*/
            if( sockfd == m_listenFd )
            {
                printf("有新连接到来\n\n");
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof( client_address );
                int connfd = accept( m_listenFd, (struct sockaddr*)&client_address, &client_addrlength );
                if( connfd < 0 )
                {
                    perror("accept");
                    continue;
                }

                /* 将新连接添加到epoll集合，并将其设置为非阻塞*/
                EpollAdd( connfd, true );
            }

            /* 客户端有数据到来*/
            else if( events[i].events & EPOLLIN )
            {
                int sockfd = events[i].data.fd;
                if( pUsersReqest[ sockfd ].needRecv == -1 )
                {
                    pUsersReqest[ sockfd ].haveRecv = 0;
                    pUsersReqest[ sockfd ].needRecv = sizeof(Protocol);
                }

                while( true )
                {
                    int ret = read( sockfd, (char *)&pUsersReqest[ sockfd ].prot + pUsersReqest[ sockfd ].haveRecv,
                                   pUsersReqest[ sockfd ].needRecv );

                    /* 如果读取数据出错*/
                    if( (ret == -1 && errno != EAGAIN && errno != EWOULDBLOCK) || ret == 0)
                    {
                        close( sockfd );
                        EpollDel( sockfd );
                        pUsersReqest[ sockfd ].haveRecv = 0;
                        pUsersReqest[ sockfd ].needRecv = -1;
                        break;
                    }

                    pUsersReqest[ sockfd ].haveRecv += ret;
                    pUsersReqest[ sockfd ].needRecv -= ret;

                    /* 请求包已接收完毕*/
                    if( pUsersReqest[ sockfd ].haveRecv == sizeof(Protocol) )
                    {
                        pUsersReqest[ sockfd ].haveRecv = 0;
                        pUsersReqest[ sockfd ].needRecv = -1;

                        /* 将描述符从epoll集合中去除，等任务执行完毕后再将其放回*/
                        EpollDel( sockfd );

                        /* 将协议包作为新任务添加到线程池*/
                        pUsersReqest[ sockfd ].prot.m_sockFd = sockfd;
                        m_pMyThreadpool->AppendTaskToPool( pUsersReqest[ sockfd ].prot );
                        break;
                    }

                    if( ret == -1 && ( errno == EAGAIN || errno == EWOULDBLOCK ) )
                    {
                        break;
                    }
                }
            }

            /* 出现异常情况*/
            else if( events[i].events & ( EPOLLRDHUP | EPOLLHUP | EPOLLERR ) )
            {
                EpollDel( events[i].data.fd );
                close( events[i].data.fd );
            }
        }
    }

    delete m_pMyThreadpool;
    m_pMyThreadpool = NULL;

    delete[] pUsersReqest;
    pUsersReqest = NULL;
}

