/*************************************************************************
	> File Name: TaskThread.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时20分20秒
 ************************************************************************/

#include "./TaskThread.h"


/*----------------------------public----------------------------*/

/* 交换两个链表的身份*/
void TaskThread::ExchangeList()
{
    m_mutex.lock();

    int temp = m_busyIdx;
    m_busyIdx = m_freeIdx;
    m_freeIdx = temp;

    m_mutex.unlock();
}

/*----------------------------public----------------------------*/

/* 任务线程构造函数*/
TaskThread::TaskThread( int epollFd, int maxLength )
    : m_epollFd(epollFd), m_maxLength(maxLength), m_freeIdx(0), m_busyIdx(1), m_count(50)
{
    /* 将this指针作为参数传给线程函数*/
    pthread_create(&m_tid, NULL, Run, this);

    /* 将线程设为分离状态*/
    pthread_detach( m_tid );
}


/* 任务线程析构函数*/
TaskThread::~TaskThread()
{
    
}


/* 获取当前空闲任务链表可插入任务结点数*/
int TaskThread::GetFreeNum()
{
    return m_maxLength - m_taskList[ m_freeIdx ].size();
}


/* 将一个新任务添加到空闲任务链表*/
void TaskThread::AddTaskToFree(Task *pTask)
{
    m_mutex.lock();

    /* 添加任务结点并发送唤醒信号*/
    m_taskList[ m_freeIdx ].push_back(pTask);
    m_cond.signal();

    m_mutex.unlock();
}


/* 任务线程的线程函数*/
void* TaskThread::Run(void *arg)
{
    TaskThread *pTThr = (TaskThread *)arg;

    int count = 1;
    
    /* 遍历迭代器*/
    list<Task*>::iterator iter = pTThr->m_taskList[ pTThr->m_busyIdx ].begin();
    /* 删除迭代器*/
    list<Task*>::iterator delIter = iter;

    while( true )
    {
        if( pTThr->m_taskList[ pTThr->m_busyIdx ].empty() )
        {
            /* 两个任务链表都为空时，阻塞*/
            if( pTThr->m_taskList[ pTThr->m_freeIdx ].empty() ) 
            {
                pTThr->m_cond.wait();

                /* 有新任务到来在空闲任务链表，在此交换两个链表身份*/
                pTThr->ExchangeList();

                /* 遍历迭代器始终指向繁忙任务链表的结点*/
                iter = pTThr->m_taskList[ pTThr->m_busyIdx ].begin();
            }
            /* 如果空闲任务链表不为空时，交换两个链表的身份*/
            else
            {
                pTThr->ExchangeList();
                iter = pTThr->m_taskList[ pTThr->m_busyIdx ].begin();
            }

        }

        /* 链表迭代器走到头，即轮转一遍，然后再从头开始，计数器+1*/
        if( iter == pTThr->m_taskList[ pTThr->m_busyIdx ].end() )
        {
            count++;

            /* 轮转指定次数后切换两个链表的身份*/
            if( count % pTThr->m_count == 0 )
            {
                pTThr->ExchangeList();
            }

            iter = pTThr->m_taskList[ pTThr->m_busyIdx ].begin();
            continue;
        }

        /* 执行任务结点任务*/
        bool ret = (*iter)->ExecuteTask();
        
        /* 执行任务期间未出错 */
        if( ret == true )
        {
            /*如果任务已完成，则删除并释放任务结点，并将客户端描述符添加回epoll监听集合*/
            if( (*iter)->TaskIsFinish() )
            {
                printf("任务已完成...\n");

                (*iter)->AddTaskFdToEpoll( pTThr->m_epollFd );

                delIter = iter;       
                ++iter;
                free((*delIter));
                pTThr->m_taskList[ pTThr->m_busyIdx ].erase(delIter);
            }
            else 
            {
                ++iter;
            }
        }

        /* 执行任务期间出错，则关闭套接字，删除任务结点*/
        else
        {
            delIter = iter;       
            ++iter;

            (*delIter)->CloseSocket();
            free((*delIter));
            pTThr->m_taskList[ pTThr->m_busyIdx ].erase(delIter);
        }
    }
}
