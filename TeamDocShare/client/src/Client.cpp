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
#include <errno.h>

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
    printf("\t\t3. 退出程序\n");
    printf("---------------------------------------------------------------------\n\n");

    printf("请选择功能编号: ");
}


/* 验证登录*/
bool Client::CheckLogin()
{
    memset(&m_prot, 0x00, sizeof(m_prot));
    m_prot.m_rqs_PType = PTYPE_LOGIN;
    strcpy(m_prot.m_userName, m_userName);
    strcpy(m_prot.m_filePath, m_pwd);
    
    int ret = -1;
    int needToSend = sizeof(sizeof(m_prot));

    /* 发送登录请求包*/
    if( ! Common::SendData(m_sockFd, (char *)&m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("send");
    }
  
    /* 接收服务端对登录请求的响应包*/
    if( ! Common::RecvData(m_sockFd, (char *)&m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("recv");
    }

    if( m_prot.m_rsp_PType == PTYPE_TRUE )
    {
        printf("登录成功\n");
        return true;
    }
    else
    {
        printf("登录失败\n");
        return false;
    }
}


/* 验证注册*/
bool Client::CheckRegister()
{
    memset(&m_prot, 0x00, sizeof(m_prot));
    m_prot.m_rqs_PType = PTYPE_REGISTER;
    strcpy(m_prot.m_userName, m_userName);
    strcpy(m_prot.m_filePath, m_pwd);
    m_prot.m_groupID = -1;
    
    int ret = -1;
    int needToSend = sizeof(sizeof(m_prot));

    /* 发送注册请求包*/
    if( ! Common::SendData(m_sockFd, (char *)&m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("send register request");
    }
  
    /* 接收服务端对注册请求的响应包*/
    if( ! Common::RecvData(m_sockFd, (char *)&m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("receive register response");
    }

    if( m_prot.m_rsp_PType == PTYPE_TRUE )
    {
        printf("注册成功\n");
        return true;
    }
    else
    {
        printf("注册失败\n");
        return false;
    }   
}


/* 开启自动上传功能，即开启浏览目录线程，开启上传线程*/
void Client::StartAutoUpload()
{
    
}


/* 打印功能界面*/
void Client::PrintFunctionUI()      
{
    printf("------------------------------功能界面------------------------------\n\n");
    printf("\t\t1. 获取团队文件列表\n");
    printf("\t\t2. 拉取团队文件到本地\n");
    printf("\t\t3. 获取所有组信息\n");
    printf("\t\t4. 获取用户所属组信息\n");
    printf("\t\t5. 修改用户所属组\n");
    printf("\t\t6. 创建新的组\n");
    printf("\t\t7. 退出程序\n");
    printf("--------------------------------------------------------------------\n\n");

    printf("请选择功能编号: ");
}


/* 获取团队文件列表*/
void Client::GetTeamFileList()      
{
    m_prot.m_rqs_PType = PTYPE_GET_FILE_LIST;
    strcpy(m_prot.m_userName, m_userName);
    
    int ret = -1;
    int needToSend = sizeof(m_prot);

    /* 发送请求包*/
    if( ! Common::SendData(m_sockFd, (char *)&m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("send get file list request");
    }
  
    /* 接收服务端对请求的响应包*/
    if( ! Common::RecvData(m_sockFd, (char *)&m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("receive register response");
    }

    int needToRecv = m_prot.m_contentLength;
   

    FileListInfo fileInfo;

    printf("\n团队文件列表:\n");
    while( needToRecv )
    {
        /* 接收一个文件路径结构*/
        if( ! Common::RecvData(m_sockFd, (char *)(&fileInfo), sizeof(FileListInfo)) )      
        {
            Common::PerrExit("receive fileListInfo");
        }

        /* 打印文件路径*/
        printf("%s\n", fileInfo.m_filePath);

        needToRecv -= sizeof(FileListInfo);
    }
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

/* 从标准输入获取用户名和密码*/
void Client::GetNameAndPwd(bool isRegister)
{
    printf("请输入用户名: ");
    scanf("%s", m_userName);
   
    if( isRegister )
    {
        char pwd[ PWD_LENGTH ] = {0};
        while( true )
        {
            printf("请输入注册密码: ");
            memset(pwd, 0x00, PWD_LENGTH);
            scanf("%s", pwd);
           
            printf("请再次输入注册密码: ");
            memset(m_pwd, 0x00, PWD_LENGTH);
            scanf("%s", m_pwd);

            if( strncmp(pwd, m_pwd, PWD_LENGTH) == 0 )
            {
                break;   
            }
            else
            {
                printf("两次密码不一致，请重新输入！\n\n");
            }
        }
    }
    else
    {
        printf("请输入登录密码: ");
        memset(m_pwd, 0x00, PWD_LENGTH);
        scanf("%s", m_pwd);
    }
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

    int funNo = 0;   /* 选择功能编号*/
    bool haveLogin = false;

    while( ! haveLogin )
    {
        PrintLoginRegisterUI();

        scanf("%d", &funNo);
        switch(funNo)
        {
            case 1:
            {
                GetNameAndPwd(false);

                printf("登录用户名为: [%s], 密码为: [%s]\n\n", m_userName, m_pwd);
                haveLogin = CheckLogin();
                break;
            }
            case 2:
            {
                GetNameAndPwd(true);

                printf("登录用户名为: [%s], 密码为: [%s]\n\n", m_userName, m_pwd);
                haveLogin = CheckRegister();
                break;
            }
            case 3:
            {
                printf("程序已退出");
                exit(0);
            }
            default:
            {
                printf("输入编号有误，请重新确认后，再次输入...\n\n");
                break;
            }
        }
    }

    /* 登录成功-----------------------------------------------------*/
    /* 开启自动上传*/
    m_pAutoUpload = AutoUpload::CreateAutoUpload(m_userName, m_prot.m_groupID, m_sockFd);


    while( true )
    {
        PrintFunctionUI();

        scanf("%d", &funNo);
        switch( funNo )
        {
            /* 获取团队文件列表*/
            case 1:
            {
                GetTeamFileList();   
                break;
            }
            /* 拉取团队文件到本地*/
            case 2:
            {
                
                break;
            }
            /* 获取所有组信息*/
            case 3:
            {
                
                break;
            }
            /* 获取用户所属组信息*/
            case 4:
            {
                
                break;
            }
            /* 修改用户所属组*/
            case 5:
            {
                
                break;
            }
            /* 创建新的组*/
            case 6:
            {
                
                break;
            }
            /* 退出程序*/
            case 7:
            {
                printf("程序已退出");
                exit(0);
            }
            default:
            {
                printf("输入编号有误，请重新确认后，再次输入...\n\n");
                break;
            }
        }
    }
    

    while(1)
    {
        sleep(1);
    }
}

