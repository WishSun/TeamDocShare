/*************************************************************************
	> File Name: Task.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时19分46秒
 ************************************************************************/

#ifndef _TASK_H
#define _TASK_H
#include "../../common/inc/common.h"
#include "./UserManage.h"
#include "./GroupManage.h"

class Task
{
private:
    enum handle_status
    {
        FIRST_COME = 0,     /* 任务首次到来状态*/
        HANDLEING,          /* 任务处理状态*/
        SEND_CONTINUE,      /* 发送让客户端继续发送数据状态*/
        SEND_RESPONSE       /* 发送响应状态*/
    };
    
    Protocol  m_prot;       /* 该任务对象应处理的协议包*/
    bool      m_isFinish;   /* 任务是否已完成*/

    int       m_fileFd;     /* 文件描述符*/
    int       m_sockFd;     /* 与客户端通信套接字*/

    int       m_needSend;   /* 需要读取的字节数*/
    int       m_haveSend;   /* 已经读取的字节数*/

    int       m_needRecv;   /* 需要发送的字节数*/
    int       m_haveRecv;   /* 已经发送的字节数*/
    
    handle_status m_handle_status; /* 任务处理状态*/

    GroupInfo m_groupInfo;  /* 组信息*/

    char      m_buf[ SENDRECV_LENGTH ];  /* 缓冲区*/
    int       m_bufNeedDeal;             /* 缓冲区需要处理的字节数*/
    int       m_bufHaveDeal;             /* 缓冲区已经处理的字节数*/
    
    UserManage  *m_pUM;  /* 用户管理类单例对象*/
    GroupManage *m_pGM;  /* 群组管理类单例对象*/

private:
    bool HandleUploadFile();         /* 处理用户上传文件任务*/
    bool HandleDownloadFile();       /* 处理用户上传文件任务*/
    bool HandleUserLogin();          /* 处理用户登录任务*/
    bool HandleUserRegister();       /* 处理用户注册任务*/
    bool HandleGetFileList();        /* 处理获取文件列表任务*/
    bool HandleGetGroupList();       /* 处理获取群组列表任务*/
    bool HandleGetMyGroupInfo();     /* 处理获取用户自己所属群组信息任务*/
    bool HandleChangeGroup();        /* 处理用户更改所属群组任务*/
    bool HandleCreateGroup();        /* 处理用户创建新群组任务*/

public:
    Task(const Protocol& prot);    /* 任务类构造函数*/
    ~Task();                       /* 任务类析构函数*/

    bool ExecuteTask();     /* 执行任务*/
    bool TaskIsFinish();    /* 判断任务是否已完成*/

    void AddTaskFdToEpoll( int epollFd ); /* 将完成任务的套接字描述符重新添加到epoll监听集合*/

    void CloseSocket();     /* 关闭套接字*/
};

#endif
