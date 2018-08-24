/*************************************************************************
	> File Name: MyThreadPool.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月24日 星期五 21时05分07秒
 ************************************************************************/

#ifndef _MYTHREADPOOL_H
#define _MYTHREADPOOL_H

#include "../../common/inc/common.h"
#include "./ThreadPool.h"

/* 对线程池类的一个包装*/
class MyThreadPool
{
private:
    ThreadPool *m_pBigThrPool;  /* 大任务线程池对象*/
    ThreadPool *m_pSmaThrPool;  /* 小任务线程池对象*/

    static MyThreadPool *m_pMyThreadpool; /* 单例线程池包装类单例对象*/
    MyThreadPool(int epollFd);      /* 构造函数*/

public:
    
    static MyThreadPool* CreateMyThreadPool(int epollFd);   /* 获取单例线程池包装类对象*/
    
    bool AppendTaskToPool( Protocol& prot );     /* 将新任务添加到合适的线程池*/
};


#endif
