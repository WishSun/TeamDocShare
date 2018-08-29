/*************************************************************************
	> File Name: AutoUpload.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 16时30分08秒
 ************************************************************************/

#ifndef _AUTOUPLOAD_H
#define _AUTOUPLOAD_H

#include <list>
#include "../../common/inc/common.h"
#include "./UploadTask.h"
#include <pthread.h> 

/* 自动上传类*/
class AutoUpload
{
private:
    list< UploadTask*> m_uploadList;   /* 文件上传链表*/
    pthread_t  m_scanThread;      /* 浏览目录线程*/
    pthread_t  m_uploadThread;    /* 上传文件线程*/
    Mutex   m_mutex;           /* 互斥锁*/
    Cond    m_cond;            /* 条件变量*/
    char    m_userName[ NAME_LENGTH ]; /* 用户名*/
    int     m_groupID;                 /* 所属组ID*/

    static AutoUpload *m_pAutoUpload;    /* 自动上传类单例对象指针*/
    Protocol m_prot;           /* 协议包*/
    int      m_sockFd;         /* 与服务端通信套接字描述符*/

private:
    AutoUpload(char *uName, int gid, int sockFd);   /* 构造函数*/
    static void ScanDirentCore( char *shareDirent, AutoUpload* pAU );   /* 浏览共享目录，将共享目录中文件放入上传链表中*/
    static void CheckFileOccupied(AutoUpload *pAU); /* 检查上传文件链表中的文件是否被占用，如果被占用则剔除*/
    static bool UploadFileCore(AutoUpload *pAU, UploadTask *pUTask);   /* 上传单个文件任务*/

public:
    static AutoUpload* CreateAutoUpload(char *uName, int gid, int sockFd);      /* 获取自动上传类单例指针*/
    static void *ScanDirent(void *arg);  /* 浏览目录线程函数*/
    static void *UploadFile(void *arg);  /* 上传文件线程函数*/
};
#endif
