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

ThreadPool* ThreadPool::m_pBigTaskPool = NULL;
ThreadPool* ThreadPool::m_pSmallTaskPool = NULL;

/* 线程池构造函数*/
ThreadPool::ThreadPool( int epollFd, bool isBig ) 
{
    m_isBig = isBig;

    if( m_isBig )
    {
        for(int i = 0; i < BIG_THR_NUM; i++)
        {
            m_pBigThreads.push_back(new TaskThread(epollFd, BIG_THR_LIST_LENGTH));
        }
    }
    else
    {
        for(int i = 0; i < SMA_THR_NUM; i++)
        {
            m_pSmaThreads.push_back(new TaskThread(epollFd, SMA_THR_LIST_LENGTH));
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
    /* 挑出负载最轻的一个工作线程*/
    int minLoad = 0;
    if( m_isBig )
    {
        for(int i = 1; i < BIG_THR_NUM; i++)       
        {
            if( m_pBigThreads[i]->GetFreeNum() < m_pBigThreads[ minLoad ]->GetFreeNum() )
            {
                minLoad = i;
            }
        }

        /* 所有工作线程都已满载*/
        if( m_pBigThreads[ minLoad ]->GetFreeNum() >= BIG_THR_LIST_LENGTH )
        {
            return false;
        }

        m_pBigThreads[ minLoad ]->AddTaskToFree(pTask);
        return true;
    }
    else
    {
        for(int i = 1; i < SMA_THR_NUM; i++)
        {
            if( m_pSmaThreads[i]->GetFreeNum() < m_pSmaThreads[ minLoad ]->GetFreeNum() )
            {
                minLoad = i;
            }
        }

        /* 所有工作线程都已满载*/
        if( m_pSmaThreads[ minLoad ]->GetFreeNum() <= 0 )
        {
            return false;
        }

        m_pSmaThreads[ minLoad ]->AddTaskToFree(pTask);
        return true;
    }
}

