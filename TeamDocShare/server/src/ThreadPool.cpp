/*************************************************************************
	> File Name: ThreadPool.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分01秒
 ************************************************************************/

#include "./ThreadPool.h"

/* 大小任务线程数*/
#define BIG_THR_NUM 5
#define SMA_THR_NUM 3

/* 大小任务线程的任务链表最大长度*/
#define BIG_THR_LIST_LENGTH  100
#define SMA_THR_LIST_LENGTH  500

/*----------------------------private----------------------------*/

ThreadPool *ThreadPool::m_pBigTaskPool = NULL;
ThreadPool *ThreadPool::m_pSmallTaskPool = NULL;

/* 线程池构造函数*/
ThreadPool::ThreadPool( int epollFd, bool isBig )
{
    m_pUM = UserManage::CreateUserManage();
    m_pGM = GroupManage::CreateGroupManage();

    if( isBig )
    {
        m_pSmallTaskPool = NULL;

        for(int i = 0; i < BIG_THR_NUM; i++)
        {
            m_pBigThreads.push_back(new TaskThread(epollFd, BIG_THR_LIST_LENGTH, m_pUM, m_pGM));
        }
    }
    else
    {
        m_pBigTaskPool = NULL;

        for(int i = 0; i < SMA_THR_NUM; i++)
        {
            m_pBigThreads.push_back(new TaskThread(epollFd, SMA_THR_LIST_LENGTH, m_pUM, m_pGM));
        }
    }
}


/*----------------------------private----------------------------*/

/* 线程池析构函数*/
ThreadPool::~ThreadPool()
{
    
}


/* 创建大任务线程池单例对象*/
ThreadPool* ThreadPool::CreateBigThreadPool(int epollFd)
{
    if( m_pBigTaskPool == NULL )
    {
        m_pBigTaskPool = new ThreadPool(epollFd, true);
    }

    return m_pBigTaskPool;
}


/* 创建小任务线程池单例对象*/
ThreadPool* ThreadPool::CreateSmallThreadPool(int epollFd)
{
    if( m_pSmallTaskPool == NULL )
    {
        m_pSmallTaskPool = new ThreadPool(epollFd, false);
    }

    return m_pSmallTaskPool;
}


/* 想线程池中添加任务*/
bool ThreadPool::AppendTask(Task *pTask)
{
    
}

