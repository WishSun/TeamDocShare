/*************************************************************************
	> File Name: GroupManage.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分52秒
 ************************************************************************/

#ifndef _GROUPMANAGE_H
#define _GROUPMANAGE_H

#include "../../common/inc/common.h"
#include "./MysqlConn.h"
#include <mysql/mysql.h>
#include <queue>
#include <list>
using namespace std;

class GroupManage
{
private:
    char sql[ TEXT_LENGTH ];     /* sql语句缓冲区*/

    static GroupManage  *m_pGM;  /* 群组管理类对象单例指针*/

    MysqlConn m_sqlConn;         /* 数据库连接对象*/
   
private:
    GroupManage();               /* 构造函数*/

public:
    static GroupManage* CreateGroupManage();   /* 获取群组管理类单例对象*/

    bool AddNewGroup(GroupInfo *pG);    /* 增加一个新的组*/
    bool GetGroupList(list<GroupInfo>& groupList);                             /* 查找所有群组信息*/
};


#endif
