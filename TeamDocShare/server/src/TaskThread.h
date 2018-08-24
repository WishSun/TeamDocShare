/*************************************************************************
	> File Name: TaskThread.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时20分10秒
 ************************************************************************/

#ifndef _TASKTHREAD_H
#define _TASKTHREAD_H

#include "../../common/inc/common.h"
#include "./Task.h"
#include "./UserManage.h"
#include "./GroupManage.h"
#include <list>
using namespace std;

class TaskThread
{
private:
    bool    m_isBigTask;    /* 是否为大任务线程*/
    int     m_maxLength;    /* 线程的任务链表的最大长度*/

    list<Task*> m_taskList[2];  /* 任务链表数组*/
    int m_freeIdx;              /* 空闲任务链表下标*/
    int m_busyIdx;              /* 繁忙任务链表下标*/
    int m_count;                /* 记录执行任务链表轮转次数*/

    Cond    m_cond;         /* 条件变量*/
    Mutex   m_mutex;        /* 互斥量*/

    UserManage  *m_pUM;     /* 用户管理类对象指针*/
    GroupManage *m_pGM;     /* 群组管理类对象指针*/

    pthread_t    m_tid;     /* 线程id*/

public:
    TaskThread( int epollFd, int maxLength, UserManage *pUM, GroupManage *pGM ); /* 任务线程构造函数*/
    ~TaskThread();         /* 任务线程析构函数*/

    int GetFreeNum();      /* 获取当前空闲任务链表可插入任务结点数*/
    void AddTaskToFree(Task *pTask);    /* 将一个新任务添加到空闲任务链表*/
    void* Run(void *arg);  /* 任务线程的线程函数*/
};
#endif
