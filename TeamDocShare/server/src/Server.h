/*************************************************************************
	> File Name: Server.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时19分13秒
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H

#include "../../common/inc/common.h"
#include "./ThreadPool.h"
#include "./MyThreadPool.h"


/* 服务器监听类*/
class Server
{
private:
    char    m_IP[ IP_LENGTH ];  /* 服务器监听地址*/
    int     m_port;             /* 服务器监听端口*/
    int     m_listenFd;         /* 服务器监听套接字描述符*/
    int     m_epollFd;          /* I/O多路复用epoll事件监听集合句柄*/

    MyThreadPool *m_pMyThreadpool;      /* 线程池对象集合*/
    static Server *m_pServer;           /* 单例服务器监听类对象*/

private:
    Server(char *ip, int port);       /* 构造函数*/
    bool CreateSocket();    /* 创建监听socket并绑定地址和端口*/
    bool CreateEpoll();     /* 创建epoll事件监听集合，并将监听socket添加到epoll事件监听集合*/
    void EpollAdd(int fd, bool timeout);  /* 将指定描述符添加到epoll事件监听集合*/
    void EpollDel(int fd);  /* 将指定描述符从epoll监听集合中删除*/
public:
    static Server* CreateServer(char *ip, int port);    /* 创建服务器类单例对象并返回*/
    ~Server();                        /* 析构函数*/
    void Run();                       /* 运行服务器程序*/
};
#endif
