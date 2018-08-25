/*************************************************************************
	> File Name: UserManage.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分19秒
 ************************************************************************/

#ifndef _USERMANAGE_H
#define _USERMANAGE_H

#include "../../common/inc/common.h"
#include <queue>
#include <mysql/mysql.h>
using namespace std;

class UserManage
{
private:
    static UserManage *m_pUM;    /* 用户管理类单例对象指针*/

    queue<MYSQL*>  m_connQue;    /* 数据库连接队列*/
    int     m_maxConnNum;        /* 最大支持的数据库连接数*/
    Mutex   m_mutex;             /* 互斥锁*/

private:
    UserManage(int initNum, int maxNum);  /* 用户管理类构造函数*/

    MYSQL* AllocConn();          /* 从数据库连接队列申请一个连接*/
    void FreeConn(MYSQL *pMysql);/* 将一条数据库连接归还队列*/ 


public:
    static UserManage* CreateUserManage();/* 创建单例对象*/

    ~UserManage();                      /* 析构函数*/

    bool FindUser(UserInfo *pUser);     /* 查看某用户是否存在*/
    bool AddNewUser(UserInfo *pUser);   /* 添加一个新用户记录*/
    bool DelUser(char *pName);          /* 删除一个用户记录*/
    bool ChangeUserGroupID(char *pName, int gid);            /* 修改指定用户的组ID*/
    int  GetFileList(char *pName, int gid, char **pList);    /* 获取该用户的团队文件列表*/
    bool GetFilePathInMD5(char *pMD5, char *filePath);       /* 通过md5值获取文件路径*/
    bool SetFilePathtoMD5Map(char *pMD5, char *filePath);    /* 设置md5值与文件路径映射*/
};
#endif
