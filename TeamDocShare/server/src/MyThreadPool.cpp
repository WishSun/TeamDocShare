/*************************************************************************
	> File Name: MyThreadPool.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月24日 星期五 21时06分35秒
 ************************************************************************/
#include "./MyThreadPool.h"
#include <sys/stat.h>
#include <unistd.h>


/*-----------------------------private-----------------------------*/

MyThreadPool* MyThreadPool::m_pMyThreadpool = NULL;

MyThreadPool::MyThreadPool(int epollFd)
{
    m_pBigThrPool = ThreadPool::CreateBigThreadPool(epollFd);
    m_pSmaThrPool = ThreadPool::CreateSmallThreadPool(epollFd);
}

/*-----------------------------public-----------------------------*/

MyThreadPool* MyThreadPool::CreateMyThreadPool(int epollFd)
{
    if( m_pMyThreadpool == NULL )
    {
        m_pMyThreadpool = new MyThreadPool(epollFd);
    }
    return m_pMyThreadpool;
}

/* 将新任务添加到合适的线程池*/
bool MyThreadPool::AppendTaskToPool( Protocol& prot )
{
    /* 构建任务对象*/   
    Task *pNewTask = NULL;

    /* 根据不同的任务，添加到不同的线程池*/
    switch( prot.m_rqs_PType )
    {
        case PTYPE_LOGIN:                /* 登录*/
        case PTYPE_REGISTER:             /* 注册*/
        case PTYPE_GET_FILE_LIST:        /* 获取团队文件列表*/
        case PTYPE_GET_GROUP_LIST:       /* 获取所有组信息列表*/
        case PTYPE_GET_MY_GROUP_INFO:    /* 获取用户自己的组信息*/
        case PTYPE_CHANGE_GROUP:         /* 更改所属组*/
        case PTYPE_CREATE_GROUP:         /* 创建新组*/
            {
                pNewTask = new Task(prot);
                return m_pSmaThrPool->AppendTask(pNewTask);
            }

        case PTYPE_UPLOAD_FILE:          /* 上传文件*/
            {
                pNewTask = new Task(prot);

                /* 大于50MB的为大任务*/
                if( prot.m_contentLength > 50*1024*1024 )
                {
                    return m_pBigThrPool->AppendTask(pNewTask);
                }
                else
                {
                    return m_pSmaThrPool->AppendTask(pNewTask);
                }
            }
        case PTYPE_DOWNLOAD_FILE:        /* 下载文件*/
            {
                /* 文件不存在*/
                if(access(prot.m_filePath, F_OK) != 0)
                {
                    prot.m_rsp_PType = PTYPE_ERROR;
                    prot.m_contentLength = 0;
                }

                else
                {
                    /* 获取文件属性*/
                    struct stat st;
                    stat(prot.m_filePath, &st);
                    prot.m_contentLength = st.st_size;
                    prot.m_fileMode = st.st_mode;
                }
               
                pNewTask = new Task(prot);

                /* 大于50MB的为大任务*/
                if( prot.m_contentLength > 50*1024*1024 )
                {
                    return m_pBigThrPool->AppendTask(pNewTask);
                }
                else
                {
                    return m_pSmaThrPool->AppendTask(pNewTask);
                }
            }
        default: return false;
    }
}
