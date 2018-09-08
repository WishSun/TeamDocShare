/*************************************************************************
	> File Name: UserManage.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分19秒
 ************************************************************************/

#ifndef _USERMANAGE_H
#define _USERMANAGE_H

#include "../../common/inc/common.h"
#include "./MysqlConn.h"
#include <list>
#include <mysql/mysql.h>
using namespace std;

class UserManage
{
private:
    char sql[ TEXT_LENGTH ];     /* sql语句缓冲区*/

    static UserManage *m_pUM;    /* 用户管理类单例对象指针*/

    MysqlConn m_sqlConn;         /* 数据库连接对象*/
    
private:
    UserManage();                /* 用户管理类构造函数*/

public:
    static UserManage* CreateUserManage();/* 创建单例对象*/

    ~UserManage();                      /* 析构函数*/

    bool FindUser(UserInfo *pUser);     /* 查看某用户是否存在*/
    bool AddNewUser(UserInfo *pUser);   /* 添加一个新用户记录*/
    bool DelUser(char *pName);          /* 删除一个用户记录*/
    bool ChangeUserGroupID(char *pName, int gid);            /* 修改指定用户的组ID*/
    bool  GetFileList(char *pName, int gid, list<FileListInfo> &pFList);        /* 获取该用户的团队文件列表*/
    bool GetFilePathInMD5(char *pMD5, char *filePath);       /* 通过md5值获取文件路径*/
    bool SetFilePathtoMD5Map(char *pMD5, char *filePath);    /* 设置md5值与文件路径映射*/
    bool SetFilePathtoUserName(char *filePath, char *uName); /* 设置文件路径与用户名的映射*/
};
#endif
