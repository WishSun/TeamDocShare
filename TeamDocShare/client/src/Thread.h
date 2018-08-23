/*************************************************************************
	> File Name: Thread.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 16时29分23秒
 ************************************************************************/

#ifndef _THREAD_H
#define _THREAD_H

#include "../../common/common.h"

class Thread
{
private:
    pthread_t   m_tid;    /* 线程id*/

public: 
    Thread(handle_t handle);  /* 线程类构造函数*/
};

#endif
