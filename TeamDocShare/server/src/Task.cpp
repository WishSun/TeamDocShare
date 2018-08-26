/*************************************************************************
	> File Name: Task.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时19分53秒
 ************************************************************************/

#include "./Task.h"
#include <unistd.h>
#include <errno.h>
#include <string.h>


/*----------------------------private----------------------------*/

/* 处理用户上传文件任务*/
bool Task::HandleUploadFile()
{
    
}


/* 处理用户上传文件任务*/
bool Task::HandleDownloadFile()
{
    
}


/* 处理用户登录任务
 * 返回值: 传输过程中出错返回false
 *         没出错返回true
 */
bool Task::HandleUserLogin()
{
    if( m_needSend == -1 )   
    {
        UserInfo user;
        strcpy(user.m_userName, m_prot.m_userName);
        strcpy(user.m_userPwd, m_prot.m_filePath);  /* 登录时，使用m_filePath来存放密码*/

        /* 如果验证成功, 则将其群组ID填充到协议包，发送给客户端*/
        if( m_pUM->FindUser(&user) )
        {
            m_prot.m_PType = PTYPE_TRUE;
            m_prot.m_groupID = user.m_groupID;
        }
        else
        {
            m_prot.m_PType = PTYPE_ERROR;
        }

        m_needSend = sizeof(m_prot);
        m_haveSend = 0;
    }
    if( m_needSend > 0 )
    {
        int ret = write(m_sockFd, (char *)&m_prot + m_haveSend, m_needSend);
        if( ret == -1 )
        {
            if( errno != EAGAIN && errno != EWOULDBLOCK )
            {
                return false;
            }
        }

        m_needSend -= ret;
        m_haveSend += ret;
    }

    if( m_needSend == 0 )
    {
        m_isFinish = true;
    }
    else
    {
        m_isFinish = false;
    }

    return true;
}


/* 处理用户注册任务*/
bool Task::HandleUserRegister()
{
    
}


/* 处理获取文件列表任务*/
bool Task::HandleGetFileList()
{
    
}


/* 处理获取群组列表任务*/
bool Task::HandleGetGroupList()
{
    
}


/* 处理获取用户自己所属群组信息任务*/
bool Task::HandleGetMyGroupInfo()
{
    
}


/* 处理用户更改所属群组任务*/
bool Task::HandleChangeGroup()
{
    
}


/* 处理用户创建新群组任务*/
bool Task::HandleCreateGroup()
{
    
}


/*----------------------------public----------------------------*/

/* 任务类构造函数*/
Task::Task(const Protocol& prot)
    :m_prot(prot), m_isFinish(false), m_fileFd(-1), m_sockFd(prot.m_sockFd), 
    m_needSend(-1), m_needRecv(-1)
{
    m_pUM = UserManage::CreateUserManage();
    m_pGM = GroupManage::CreateGroupManage();
}


/* 任务类析构函数*/
Task::~Task()
{

}


/* 执行任务*/
bool Task::ExecuteTask()
{
    switch( m_prot.m_PType )
    {
        case PTYPE_UPLOAD_FILE:      /* 上传文件*/
        {
            return HandleUploadFile();
            break;
        }
        case PTYPE_DOWNLOAD_FILE:    /* 下载文件*/
        {
            return HandleDownloadFile();
            break;
        }
        case PTYPE_LOGIN:            /* 登录*/
        {
            return HandleUserLogin();
            break;
        }
        case PTYPE_REGISTER:         /* 注册*/
        {
            return HandleUserRegister();
            break;
        }
        case PTYPE_GET_FILE_LIST:    /* 获取团队文件列表*/
        {
            return HandleGetFileList();
            break;
        }
        case PTYPE_GET_GROUP_LIST:   /* 获取所有组信息列表*/
        {
            return HandleGetGroupList();
            break;
        }
        case PTYPE_GET_MY_GROUP_INFO:/* 获取用户自己的组信息*/
        {
            return HandleGetMyGroupInfo();
            break;
        }
        case PTYPE_CHANGE_GROUP:     /* 更改所属组*/
        {
            return HandleChangeGroup();
            break;
        }
        case PTYPE_CREATE_GROUP:     /* 创建新组*/
        {
            return HandleCreateGroup();
            break;
        }
        default: return false;
    }
}


/* 判断任务是否已完成*/
bool Task::TaskIsFinish()
{
    return m_isFinish;
}


/* 将完成任务的套接字描述符重新添加到epoll监听集合*/
void Task::AddTaskFdToEpoll( int epollFd )
{
    Common::EpollAddCore(epollFd, m_sockFd, true);   
}
