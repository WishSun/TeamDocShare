/*************************************************************************
	> File Name: client.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 16时26分52秒
 ************************************************************************/
#include "./Client.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>


/*-------------------------------private-------------------------------*/

/* 连接服务端*/
bool Client::ConnectServer()
{
    struct sockaddr_in  sev_addr;
    int len = sizeof(sev_addr);

    /* 配置监听地址和端口*/
    sev_addr.sin_family = AF_INET;
    sev_addr.sin_port = htons(m_sevPort);
    sev_addr.sin_addr.s_addr = inet_addr(m_sevIP);

    /* 连接服务端*/
    if( connect(m_sockFd, (struct sockaddr*)&sev_addr, len) == -1 )
    {
        perror("connect");
        return false;
    }

    return true;
}


/* 打印登录注册界面*/
void Client::PrintLoginRegisterUI()
{
    printf("------------------------------登录/注册------------------------------\n\n");
    printf("\t\t1. 登录\n");
    printf("\t\t2. 注册\n");
    printf("---------------------------------------------------------------------\n\n");

    printf("请选择功能编号: ");
}


/* 验证登录*/
bool Client::CheckLogin()
{
    
}


/* 验证注册*/
bool Client::CheckRegister()
{
    
}


/* 开启自动上传功能，即开启浏览目录线程，开启上传线程*/
void Client::StartAutoUpload()
{
    
}


/* 打印功能界面*/
void Client::PrintFunctionUI()      
{
    
}


/* 获取团队文件列表*/
void Client::GetTeamFileList()      
{
    
}


/* 拉取团队文件到本地*/
void Client::DownloadTeamFile(char *filePath)
{
    
}


/* 获取所有组信息*/
void Client::GetAllGroupInfo()      
{
    
}


/* 创建新的组*/
void Client::CreateNewGroup( GroupInfo *pGroup )
{
    
}


/* 更改所属组*/
bool Client::ChangeMyGroup(int gid) 
{
    
}


/* 获取用户所属组信息*/
void Client::GetMyGroupInfo()       
{
    
}


/*-------------------------------public-------------------------------*/

/* 构造函数*/
Client::Client(char *ip, int port)  
{
    /* 初始化IP和端口*/
    strcpy(m_sevIP, ip);
    m_sevPort = port;


    /* 创建socket*/
    if( (m_sockFd = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
    {
        perror("socket");
        exit(1);
    }
}


/* 析构函数*/
Client::~Client()                   
{
    
}


/* 开始执行客户端程序*/
void Client::Run()      
{
    if( ! ConnectServer() )
    {
        return;
    }
    printf("连接服务器成功\n");
    PrintLoginRegisterUI();
}

