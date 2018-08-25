/*************************************************************************
	> File Name: locker.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年04月15日 星期日 16时57分44秒
 ************************************************************************/

#ifndef _LOCKER_H
#define _LOCKER_H

#include <exception>
#include <pthread.h>
#include <semaphore.h>
#include "./parseCfg.h"

using namespace std;

/* 文件路径长度*/
#define PATH_LENGTH 1024

/* 用户名/群组名长度*/
#define NAME_LENGTH 50

/* 密码长度*/
#define PWD_LENGTH  7

/* md5码长度*/
#define MD5_LENGTH  33

/* 信息长度, 包括错误信息以及群组简介*/
#define TEXT_LENGTH 1024

/* IP地址长度*/
#define IP_LENGTH 16

typedef void*(* handle_t )(void*);

/* 客户端请求类型*/
enum PTYPE
{
    PTYPE_UPLOAD_FILE = 0,      /* 上传文件*/
    PTYPE_DOWNLOAD_FILE,        /* 下载文件*/
    PTYPE_LOGIN,                /* 登录*/
    PTYPE_REGISTER,             /* 注册*/
    PTYPE_GET_FILE_LIST,        /* 获取团队文件列表*/
    PTYPE_GET_GROUP_LIST,       /* 获取所有组信息列表*/
    PTYPE_GET_MY_GROUP_INFO,    /* 获取用户自己的组信息*/
    PTYPE_CHANGE_GROUP,         /* 更改所属组*/
    PTYPE_CREATE_GROUP,         /* 创建新组*/
    PTYPE_ERROR,                /* 出错的请求或处理*/
    PTYPE_TRUE                  /* 正确的请求*/
};

/* 服务端与客户端通信协议类*/
struct Protocol
{
    PTYPE   m_PType;        /* 请求类型*/
    int     m_sockFd;       /* 客户端套接字描述符*/
    char    m_userName[ NAME_LENGTH ];   /* 用户名*/
    int     m_groupID;      /* 用户所属ID*/
    char    m_fileMD5[ MD5_LENGTH ];     /* 文件的MD5码*/
    char    m_filePath[ PATH_LENGTH ];   /* 文件路径*/
    int64_t m_contentLength;/* 协议包后边的数据实体字节数*/

    Protocol()
    : m_fileFd(-1), m_sockFd(-1), m_groupID(-1), m_contentLength(0)
    {}
};

/* 用户信息类*/
struct UserInfo
{
    char m_userName[ NAME_LENGTH ];       /* 用户名*/
    char m_userPwd[ PWD_LENGTH ];         /* 密码*/
    int  m_groupID;                       /* 所属组ID*/
};

/* 群组信息类*/
struct GroupInfo
{
    int  m_groupID;                  /* 群组ID*/
    char m_groupName[ NAME_LENGTH ]; /* 群组名*/
    int  m_groupMemNum;              /* 群组成员数*/
    char m_groupInfo[ TEXT_LENGTH ]; /* 群组简介*/ 
};

/* 封装互斥锁的类------------------------------------------------------*/
class Mutex
{
private:
    pthread_mutex_t  m_mutex;

public:
    /* 创建并初始化互斥锁*/
    Mutex()
    {
        if(pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
    }

    /* 销毁互斥锁*/
    ~Mutex()
    {
        pthread_mutex_destroy(&m_mutex);
    }

    /* 获取互斥锁*/
    bool lock()
    {
        return pthread_mutex_lock(&m_mutex) == 0;
    }

    /* 尝试获取互斥锁*/
    bool trylock()
    {
        return pthread_mutex_trylock(&m_mutex) == 0;
    }

    /* 释放互斥锁*/
    bool unlock()
    {
        return pthread_mutex_unlock(&m_mutex) == 0;
    }
};


/* 封装条件变量的类--------------------------------------------------*/
class Cond
{
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t  m_cond;

public:
    /* 创建并初始化条件变量*/
    Cond()
    {
        if(pthread_mutex_init(&m_mutex, NULL) != 0)
        {
            throw std::exception();
        }
        if(pthread_cond_init(&m_cond, NULL) != 0)
        {
            /* 构造函数一旦出了问题，就应该立即释放已成功分配了的资源*/
            pthread_mutex_destroy(&m_mutex);
            throw std::exception();
        }
    }

    /* 销毁条件变量*/
    ~Cond()
    {
        pthread_mutex_destroy(&m_mutex);
        pthread_cond_destroy(&m_cond);
    }

    /* 等待条件变量*/
    bool wait()
    {
        int ret = 0;

        /* 在调用pthread_cond_wait函数前，必须确保互斥锁mutex已经加锁, 
         * 否则将导致不可预期的结果
         */
        pthread_mutex_lock(&m_mutex);

        /* pthread_cond_wait函数执行时，首先把调用线程放入条件变量的等
         * 待队列中，然后将互斥锁mutex解锁(为了能让别的线程也可以等待)。
         * 当pthread_cond_wait函数成功返回时，互斥锁mutex将再次被锁上*/
        ret = pthread_cond_wait(&m_cond, &m_mutex);
        pthread_mutex_unlock(&m_mutex);
        return ret == 0;
    }

    /* 唤醒等待一个条件变量的线程*/
    bool signal()
    {
        return pthread_cond_signal(&m_cond) == 0;
    }
};
#endif
