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
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <malloc.h>


/*----------------------------private----------------------------*/

/* 处理用户上传文件任务*/
bool Task::HandleUploadFile()
{
    switch( m_handle_status )
    {
        /* 任务首次到来状态*/
        case FIRST_COME:
        {
            char filePath[ PATH_LENGTH ] = {0};
            char userFile[ PATH_LENGTH ] = {0};

            /* 查询数据库MD5值与文件路径映射有无记录，
             * 如果有且文件存在，则创建硬链接在本用户目录
             */
            if( m_pUM->GetFilePathInMD5(m_prot.m_fileMD5, filePath) )
            {
                sprintf(userFile, "./%s.share/%s", m_prot.m_userName, m_prot.m_filePath);
                link(filePath, userFile);

                /* 将用户名与文件路径映射信息装入数据库*/
                m_pUM->SetFilePathtoUserName(userFile, m_prot.m_userName);

                /* 状态迁移至发送响应状态*/
                m_prot.m_rsp_PType = PTYPE_TRUE;
                m_handle_status = SEND_RESPONSE;
            }
            else
            {
                sprintf(userFile, "./%s.share/%s", m_prot.m_userName, m_prot.m_filePath);

                m_fileFd = open(userFile, O_CREAT | O_WRONLY | O_TRUNC, m_prot.m_fileMode);
                m_needRecv = m_prot.m_contentLength;

                /* 状态迁移至发送继续响应状态*/
                m_prot.m_rsp_PType = PTYPE_CONTINUE;
                m_handle_status = SEND_CONTINUE;
                m_needSend = -1;
            }
            break;
        }

        /* 发送继续响应状态*/
        case SEND_CONTINUE:
        {
            if( m_needSend == -1 )
            {
                m_needSend = sizeof(m_prot);
                m_haveSend = 0;
            }

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

            m_isFinish = false;

            /* 让客户端继续发送文件数据的响应发送完毕，状态迁移至处理状态*/
            if( m_needSend == 0 )
            {
                m_handle_status = HANDLEING;
                m_needSend = -1;
            }
            break;
        }

        case HANDLEING:
        {
            /* 从套接字中读取文件数据, 只读一次*/
            int recvLen = m_needRecv > SENDRECV_LENGTH ? SENDRECV_LENGTH : m_needRecv;
            char buff[ SENDRECV_LENGTH ] = {0};
            int ret = read(m_sockFd, buff, recvLen);
            if( ret == 0 )
            {
                return false;
            }
            if( ret == -1 )
            {
                if( errno != EAGAIN && errno != EWOULDBLOCK )
                {
                    return false;
                }
                m_isFinish = false;
                break;
            }

            /* 将读到的数据写入文件*/
            if( ! Common::SendData(m_fileFd, buff, ret) )
            {
                return false;
            }

            m_needRecv -= ret;
            m_haveRecv += ret;

            /* 读取完毕*/
            if( m_needRecv == 0 )
            {
                close(m_fileFd);   

                /* 在服务器backup目录创建文件硬链接，然后将该硬链接路径存入md5映射*/
                char srcFile[ PATH_LENGTH ] = {0}, dstFile[ PATH_LENGTH ] = {0};
                sprintf(srcFile, "./%s.share/%s", m_prot.m_userName, m_prot.m_filePath);
                sprintf(dstFile, "./backup/%s_%s.backup", m_prot.m_userName, m_prot.m_filePath);
                link(srcFile, dstFile);
                m_pUM->SetFilePathtoMD5Map( m_prot.m_fileMD5, dstFile );

                /* 将用户名与文件路径映射信息装入数据库*/
                m_pUM->SetFilePathtoUserName(srcFile, m_prot.m_userName);

                /* 状态迁移至发送响应状态*/
                m_prot.m_rsp_PType = PTYPE_TRUE;
                m_handle_status = SEND_RESPONSE;
                m_needSend = -1;
                m_isFinish = false;
            }
            else
            {
                m_isFinish = false;
                break;
            }
        }           

        /* 发送响应状态*/
        case SEND_RESPONSE:
        {
            if( m_needSend == -1 )
            {
                m_needSend = sizeof(m_prot);
                m_haveSend = 0;
            }

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

            m_isFinish = (m_needSend == 0);
            break;
        }
        
        default: return false;
    }

    return true;
}


/* 处理用户下载文件任务*/
bool Task::HandleDownloadFile()
{
    /* 如果文件存在，需检验用户给的文件路径名，是不是他团队成员的文件, 不是则无访问权限*/
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
            m_prot.m_rsp_PType = PTYPE_TRUE;
            m_prot.m_groupID = user.m_groupID;
        }
        else
        {
            m_prot.m_rsp_PType = PTYPE_ERROR;
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
        /* 创建用户共享目录*/
        char userDir[ PATH_LENGTH ] = {0};
        sprintf(userDir, "./%s.share", m_prot.m_userName);
        mkdir(userDir, 0777);
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
    if( m_needSend == -1 )   
    {
        UserInfo user;
        strcpy(user.m_userName, m_prot.m_userName);
        strcpy(user.m_userPwd, m_prot.m_filePath);  

        if( m_pUM->AddNewUser(&user) )
        {
            m_prot.m_rsp_PType = PTYPE_TRUE;
            m_prot.m_groupID = -1;

            /* 创建用户共享目录*/
            char userDir[ PATH_LENGTH ] = {0};
            sprintf(userDir, "./%s.share", user.m_userName);
            mkdir(userDir, 0777);
        }
        else
        {
            m_prot.m_rsp_PType = PTYPE_ERROR;
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


/* 处理获取文件列表任务*/
bool Task::HandleGetFileList()
{
    static char *pList = NULL;
    static list<FileListInfo> fileList;

    switch( m_handle_status )
    {
        case FIRST_COME:
        {
            m_pUM->GetFileList(m_prot.m_userName, m_prot.m_groupID, fileList);

            int listLen = fileList.size() * sizeof(FileListInfo);
            pList = (char *)malloc( listLen );

            char *pL = pList;
            list<FileListInfo>::iterator iter = fileList.begin();

            FileListInfo f;
            while( iter != fileList.end() )
            {
                f = *iter;
                memcpy(pL, (char*)&f, sizeof(FileListInfo));
                pL += sizeof(FileListInfo);
                iter++;
            }

            fileList.clear();

            /* 构建继续请求响应包*/
            m_prot.m_contentLength = listLen;
            m_prot.m_rsp_PType = PTYPE_CONTINUE;

            /* 状态迁移*/
            m_handle_status = SEND_CONTINUE;
            m_needSend = sizeof(m_prot);
            m_haveSend = 0;

            m_isFinish = false;

            break;
        }
        case SEND_CONTINUE:
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

            /* 响应包发送完毕，状态迁移*/
            if( m_needSend == 0 )
            {
                m_handle_status = HANDLEING;
                m_needSend = m_prot.m_contentLength;
                m_haveSend = 0;
            }

            m_isFinish = false;
            break;
        }
        case HANDLEING:
        {
            int sendLen = m_needSend > SENDRECV_LENGTH ? SENDRECV_LENGTH : m_needSend;
            int ret = write(m_sockFd, pList + m_haveSend, sendLen);
            if( (ret == -1) && (errno != EAGAIN) && (errno != EWOULDBLOCK) )
            {
                perror("write");
                return false;
            }
            m_needSend -= ret;
            m_haveSend += ret;    

            if( m_needSend == 0 )
            {
                free(pList);
                pList = NULL;
                m_isFinish = true;
            }
            break;
        }
        default: return false;
    }

    return true;
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
    :m_prot(prot), m_isFinish(false), m_fileFd(-1), m_sockFd(prot.m_sockFd), m_needSend(-1), m_haveSend(0), m_needRecv(-1),m_haveRecv(0)
{
    m_handle_status = FIRST_COME;

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
    switch( m_prot.m_rqs_PType )
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


/* 关闭套接字*/
void Task::CloseSocket()
{
    close(m_sockFd);
}
