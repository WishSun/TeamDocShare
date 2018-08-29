/*************************************************************************
	> File Name: client.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 16时28分36秒
 ************************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H
#include "../../common/inc/common.h"
#include "./AutoUpload.h"

class Client
{
private:
    char m_sevIP[16];       /* 服务端IP地址*/
    int  m_sevPort;         /* 服务端端口*/
    int  m_sockFd;          /* 客户端套接字描述符*/
    Protocol m_prot;        /* 协议包*/
    Mutex m_mutex;          /* 互斥锁*/
    char m_userName[ NAME_LENGTH ];  /* 用户名*/
    char m_pwd[ PWD_LENGTH ];        /* 密码*/
    AutoUpload *m_pAutoUpload;       /* 自动上传类对象*/

private:
    bool ConnectServer();        /* 连接服务端*/
    void PrintLoginRegisterUI(); /* 打印登录注册界面*/
    bool CheckLogin();           /* 验证登录*/
    bool CheckRegister();        /* 验证注册*/
    void StartAutoUpload();      /* 开启自动上传功能，即开启浏览目录线程，开启上传线程*/
    void PrintFunctionUI();      /* 打印功能界面*/
    void GetTeamFileList();      /* 获取团队文件列表*/
    void DownloadTeamFile(char *filePath);   /* 拉取团队文件到本地*/
    void GetAllGroupInfo();      /* 获取所有组信息*/
    void CreateNewGroup( GroupInfo *pGroup );/* 创建新的组*/
    bool ChangeMyGroup(int gid); /* 更改所属组*/
    void GetMyGroupInfo();       /* 获取用户所属组信息*/
    

    void GetNameAndPwd(bool isRegister);     /* 从标准输入获取用户名和密码*/
public:
    Client(char *ip, int port);  /* 构造函数*/
    ~Client();                   /* 析构函数*/
    void Run();                  /* 开始执行客户端程序*/
};


#endif
