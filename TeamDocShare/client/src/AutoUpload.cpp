/*************************************************************************
	> File Name: AutoUpload.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 16时30分00秒
 ************************************************************************/
#include "./AutoUpload.h"
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*----------------------------private----------------------------*/

AutoUpload* AutoUpload::m_pAutoUpload = NULL;

/* 构造函数*/
AutoUpload::AutoUpload(char *uName, int gid, int sockFd)
{
    sprintf(m_userName, "%s", uName);
    sprintf(m_shareDirent, "./%s.share", uName);
    m_groupID = gid;
    m_sockFd = sockFd;

    /* 分别开启浏览线程和上传线程*/
    pthread_create(&m_scanThread, NULL, ScanDirent, this);
    pthread_create(&m_uploadThread, NULL, UploadFile, this);

    /* 将线程设置为分离状态*/
    pthread_detach(m_scanThread);
    pthread_detach(m_uploadThread);
}


/* 检查上传文件链表中的文件是否被占用，如果被占用则剔除*/
void AutoUpload::CheckFileOccupied(AutoUpload *pAU)
{
    /* 为命令申请内存(fuser, 检查文件是否被占用)*/
    char *cmdBuf = (char *)malloc( pAU->m_uploadList.size() * PATH_LENGTH );
    memset(cmdBuf, 0x00, pAU->m_uploadList.size() * PATH_LENGTH);

    /* 构造命令*/
    char *cmd = cmdBuf;
    sprintf(cmd, "fuser -a ");
    cmd += strlen(cmd);

    list<UploadTask*>::iterator iter = pAU->m_uploadList.begin();
    for( ; iter != pAU->m_uploadList.end(); ++iter )
    {
        sprintf(cmd, "%s ", (*iter)->m_filePath);   
        cmd += strlen( cmd );
    }

    sprintf(cmd, " 2>&1");

    /* 执行命令*/
    FILE *fp = NULL;
    char resultBuf[ TEXT_LENGTH ] = {0};
    char *ptr = NULL;

    fp = popen(cmdBuf, "r");
    if( fp == NULL )
    {
        perror("popen");   
        exit(1);
    }

    iter = pAU->m_uploadList.begin();
    list<UploadTask*>::iterator delIter;

    while(fgets(resultBuf, TEXT_LENGTH, fp) != NULL)
    {
        /* 通过查看':'后面是不是'\n', 来判断是否被占用*/
        ptr = strchr(resultBuf, ':');
        ptr++;

        /* 如果被占用*/
        if(*ptr != '\n')
        {
            delIter = iter;           
            ++iter;

            UploadTask* pDel = *delIter;
            delete(pDel);
            pAU->m_uploadList.erase(delIter);

            memset(resultBuf, 0x00, TEXT_LENGTH);
            continue;
        }
        memset(resultBuf, 0x00, TEXT_LENGTH);
        ++iter;
    }

    pclose(fp);
    free(cmdBuf);
}

/* 浏览共享目录，将目录中的文件放入上传链表*/
void AutoUpload::ScanDirentCore(char *shareDirent, AutoUpload *pAU)
{
    /* 打开目录*/
    DIR *pDir = opendir(shareDirent);
    if( pDir == NULL )
    {
        perror("opendir");
        exit(1);
    }

    struct dirent *pFile = NULL;
    while( (pFile = readdir(pDir)) != NULL )
    {
        /* 文件名为.或..则不浏览*/
        if( !strcmp(pFile->d_name, ".") || !strcmp(pFile->d_name, "..") )
        {
            continue;
        }

        /* 将浏览到的文件名(仅有文件名，无路径)构造成全路径名*/
        char pathBuf[ PATH_LENGTH ] = {0};
        sprintf(pathBuf, "%s/%s", shareDirent, pFile->d_name);

        /* 如果是目录，则递归浏览*/
        if(pFile->d_type == DT_DIR)
        {
            ScanDirentCore(pathBuf, pAU);
            continue;
        }

        /* 将文件添加进上传任务链表*/
        struct stat st;
        if( stat(pathBuf, &st) == -1 )
        {
            perror("stat");
            continue;
        }

        UploadTask *pNewTask = new UploadTask();
        memset(pNewTask, 0x00, sizeof(UploadTask));
        sprintf(pNewTask->m_filePath, "%s", pathBuf);
        pNewTask->m_fileSize = st.st_size;
        pNewTask->m_fileMode = st.st_mode;

        pAU->m_uploadList.push_back(pNewTask);
    }

    closedir(pDir);
}

/*----------------------------public----------------------------*/

/* 获取自动上传类单例指针*/
AutoUpload* AutoUpload::CreateAutoUpload(char *uName, int gid, int sockFd)      
{
    if( m_pAutoUpload == NULL )   
    {
        m_pAutoUpload = new AutoUpload(uName, gid, sockFd);
    }
    return m_pAutoUpload;
}


/* 浏览目录线程函数*/
void *AutoUpload::ScanDirent(void *arg)
{
    AutoUpload *pAU = (AutoUpload*)arg;


    while( true )
    {
        while( true )
        {
            /* 浏览共享目录，将文件放入上传链表*/
            ScanDirentCore(pAU->m_shareDirent, pAU);

            /* 如果没有文件，则睡1秒*/
            if( pAU->m_uploadList.empty() )
            {
                sleep(1);
            }
            else
            {
                break;
            }
        }

        /* 检查文件是否被占用, 将占用的文件从链表中剔除*/
        CheckFileOccupied(pAU);

        if( !pAU->m_uploadList.empty() )
        {
            /* 向上传线程发送唤醒信号，有任务了*/
            pAU->m_cond.signal();
        }
        else
        {
            continue;
        }

        /* 等待上传链表中文件上传完毕*/
        pAU->m_cond.wait();
    }
}


/* 上传单个文件任务*/
bool AutoUpload::UploadFileCore(AutoUpload *pAU, UploadTask *pUTask)
{
    memset(&pAU->m_prot, 0x00, sizeof(m_prot));
    pAU->m_prot.m_rqs_PType = PTYPE_UPLOAD_FILE;
    pAU->m_prot.m_contentLength = pUTask->m_fileSize;  /* 文件大小*/
    strcpy(pAU->m_prot.m_userName, pAU->m_userName);   /* 用户名*/
    pAU->m_prot.m_fileMode = pUTask->m_fileMode;       /* 文件权限*/
    pAU->m_prot.m_groupID = pAU->m_groupID;

    /* 从路径中获取文件名*/
    char *ptr = strrchr(pUTask->m_filePath, '/');
    if( ptr == NULL )
    {
        sprintf(pAU->m_prot.m_filePath, "%s", pUTask->m_filePath);
    }
    else
    {
        sprintf(pAU->m_prot.m_filePath, "%s", ptr+1);
    }
    
    /* 获取文件的MD5值*/
    char cmd[1024] = "";
    sprintf(cmd, "md5sum %s", pUTask->m_filePath);

    char resultBuf[ TEXT_LENGTH ] = {0};

    FILE *fp = popen(cmd, "r");
    if( fp == NULL )
    {
        printf("执行get MD5 cmd错误");
        Common::PerrExit("pepen md5sum"); 
    }

    if(fgets(resultBuf, TEXT_LENGTH, fp) != NULL)
    {
        ptr = strchr(resultBuf, ' ');
        *ptr = '\0';
        sprintf(pAU->m_prot.m_fileMD5, "%s", resultBuf);
    }
    pclose(fp);

    /* 发送上传文件请求包*/
    if( ! Common::SendData(pAU->m_sockFd, (char *)&pAU->m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("send upload file request");
    }

    /* 接收服务端对上传请求的响应*/
    if( ! Common::RecvData(pAU->m_sockFd, (char *)&pAU->m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("receive upload file reponse");
    }

    /* 接收秒传*/
    if( pAU->m_prot.m_rsp_PType == PTYPE_TRUE )
    {
        printf("秒传实现！文件[%s]上传成功\n", pUTask->m_filePath);

        /* 将文件从本地删除*/
        unlink(pUTask->m_filePath);

        return true;
    }
    else if( pAU->m_prot.m_rsp_PType != PTYPE_CONTINUE )
    {
        printf("文件[%s]上传失败\n", pUTask->m_filePath);

        return false;
    }

    /* 不接受秒传，则打开文件, 发送文件数据*/
    int fd;
    if( (fd = open(pUTask->m_filePath, O_RDONLY)) == -1 )
    {
        perror("open");
        return false;
    }
    
    char buff[ SENDRECV_LENGTH ] = {0};
    int64_t needSend = pUTask->m_fileSize;
    int64_t haveSend = 0;
    while( needSend )
    {
        int sendLen = needSend > SENDRECV_LENGTH ? SENDRECV_LENGTH : needSend;
        if( ! Common::RecvData(fd, buff, sendLen) )
        {
            Common::PerrExit("RecvData");
        }

        if( ! Common::SendData(pAU->m_sockFd, buff, sendLen) )
        {
            Common::PerrExit("SendData");
        }

        needSend -= sendLen;
        haveSend += sendLen;
    }


    /* 接收服务端对上传文件的响应*/
    if( ! Common::RecvData(pAU->m_sockFd, (char *)&pAU->m_prot, sizeof(m_prot)) )
    {
        Common::PerrExit("receive upload file reponse");
    }

    if( pAU->m_prot.m_rsp_PType == PTYPE_TRUE )
    {
        printf("普通上传！文件[%s]上传成功\n", pUTask->m_filePath);

        close(fd);

        /* 将文件从本地删除*/
        unlink(pUTask->m_filePath);
        return true;
    }
    else
    {
        printf("文件[%s]上传失败\n", pUTask->m_filePath);
        return false;
    }
}


/* 上传文件线程函数*/
void *AutoUpload::UploadFile(void *arg)
{
    AutoUpload *pAU = (AutoUpload*)arg;
    while( true )
    {
        /* 如果没有文件上传任务，唤醒可能阻塞的浏览线程，然后上传线程阻塞等待*/
        if( pAU->m_uploadList.empty() )
        {
            pAU->m_cond.signal();
            pAU->m_cond.wait();
        }

        list<UploadTask*>::iterator iter = pAU->m_uploadList.begin();
        list<UploadTask*>::iterator delIter;

        for( ; !pAU->m_uploadList.empty(); )
        {
            UploadFileCore(pAU, *iter);
            
            delIter = iter;
            ++iter;
            free(*delIter);
            pAU->m_uploadList.erase(delIter);
        }
    }
}

