/*************************************************************************
	> File Name: ThreadPool.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时20分53秒
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include "./TaskThread.h"
#include "./Task.h"
#include "./UserManage.h"
#include "./GroupManage.h"
#include <vector>
using namespace std;


class ThreadPool
{
private:
    static ThreadPool *m_pBigTaskPool;    /* 大任务线程池单例对象*/
    static ThreadPool *m_pSmallTaskPool;  /* 小任务线程池单例对象*/

    bool m_isBig;

    vector<TaskThread*> m_pBigThreads;   /* 大任务线程数组*/
    vector<TaskThread*> m_pSmaThreads;   /* 小任务线程数组*/

private:
    ThreadPool( int epollFd, bool isBig );/* 线程池构造函数*/
public:
    ~ThreadPool();                        /* 线程池析构函数*/

    static ThreadPool* CreateBigThreadPool(int epollFd);  /* 创建大任务线程池单例对象*/
    static ThreadPool* CreateSmallThreadPool(int epollFd);/* 创建小任务线程池单例对象*/

    bool AppendTask(Task *pTask);         /* 想线程池中添加任务*/

};

#endif
