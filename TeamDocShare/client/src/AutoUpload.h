/*************************************************************************
	> File Name: AutoUpload.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 16时30分08秒
 ************************************************************************/

#ifndef _AUTOUPLOAD_H
#define _AUTOUPLOAD_H

#include <queue>
#include "../../common/common.h"
#include "./Thread.h"


class AutoUpload
{
private:
    queue<char[ PATH_LENGTH ]> m_uploadQueue;   /* 文件上传队列*/
    Thread  m_scanThread;      /* 浏览目录线程*/
    Thread  m_uploadThread;    /* 上传文件线程*/
    Mutex   m_mutex;           /* 互斥锁*/
    Cond    m_cond;            /* 条件变量*/

public:
    AutoUpload();                 /* 构造函数*/
    void *ScanDirent(void *arg);  /* 浏览目录线程函数*/
    void *UploadFile(void *arg);  /* 上传文件线程函数*/
};
#endif
