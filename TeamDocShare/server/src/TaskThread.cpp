/*************************************************************************
	> File Name: TaskThread.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时20分20秒
 ************************************************************************/

#include "./TaskThread.h"


/*----------------------------public----------------------------*/

/* 任务线程构造函数*/
TaskThread::TaskThread( int epollFd, int maxLength, UserManage *pUM, GroupManage *pGM )
{

}


/* 任务线程析构函数*/
TaskThread::~TaskThread()
{
    
}


/* 获取当前空闲任务链表可插入任务结点数*/
int TaskThread::GetFreeNum()
{
    
}


/* 将一个新任务添加到空闲任务链表*/
void TaskThread::AddTaskToFree(Task *pTask)
{
    
}


/* 任务线程的线程函数*/
void* TaskThread::Run(void *arg)
{
    
}
