/*************************************************************************
	> File Name: Task.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时19分46秒
 ************************************************************************/

#ifndef _TASK_H
#define _TASK_H
#include "../../common/inc/common.h"

class Task
{
private:
    Protocol  m_prot;       /* 该任务对象应处理的协议包*/
    bool      m_isFinish;   /* 任务是否已完成*/

public:
    Task(const Protocol& prot);  /* 任务类构造函数*/
    ~Task();                       /* 任务类析构函数*/

    bool TaskIsFinish();    /* 判断任务是否已完成*/

    void HandleUploadFile();         /* 处理用户上传文件任务*/
    void HandleDownloadFile();       /* 处理用户上传文件任务*/
    void HandleUserLogin();          /* 处理用户登录任务*/
    void HandleUserRegister();       /* 处理用户注册任务*/
    void HandleGetFileList();        /* 处理获取文件列表任务*/
    void HandleGetGroupList();       /* 处理获取群组列表任务*/
    void HandleGetMyGroupInfo();     /* 处理获取用户自己所属群组信息任务*/
    void HandleChangeGroup();        /* 处理用户更改所属群组任务*/
    void HandleCreateGroup();        /* 处理用户创建新群组任务*/
};

#endif
