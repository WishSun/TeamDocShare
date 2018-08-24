/*************************************************************************
	> File Name: GroupManage.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分52秒
 ************************************************************************/

#ifndef _GROUPMANAGE_H
#define _GROUPMANAGE_H

#include "../../common/inc/common.h"
#include <mysql/mysql.h>
#include <queue>
using namespace std;

class GroupManage
{
private:
    static GroupManage  *m_pGM;  /* 群组管理类对象单例指针*/

    queue<MYSQL*> m_connQue;  /* 数据库连接队列*/
    int    m_maxConnNum;      /* 最大支持的数据库连接数*/
    Mutex  m_mutex;           /* 互斥锁*/

private:
    GroupManage(int initNum, int maxNum);   /* 构造函数*/

    MYSQL* AllocConn();                 /* 获取一个数据库连接*/
    void FreeConn(MYSQL *pMysql);       /* 归还一个数据库连接*/

public:
    GroupManage* CreateGroupManage();   /* 获取群组管理类单例对象*/

    bool AddNewGroup(GroupInfo *pG);    /* 增加一个新的组*/
    GroupInfo* GetGroupList();          /* 查找所有群组信息*/
};


#endif
