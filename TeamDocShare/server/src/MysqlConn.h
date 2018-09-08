/*************************************************************************
	> File Name: MysqlConn.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年09月08日 星期六 11时36分00秒
 ************************************************************************/

#ifndef _MYSQLCONN_H
#define _MYSQLCONN_H


#include "../../common/inc/common.h"
#include <mysql/mysql.h>
#include <queue>
using namespace std;


class MysqlConn
{
private:
    queue<MYSQL*>  m_connQue;    /* 数据库连接队列*/
    int     m_maxConnNum;        /* 最大支持的数据库连接数*/
    Mutex   m_mutex;             /* 互斥锁*/

    void AppendConn();           /* 在不大于最大连接数的情况下添加APPEND_NUM条连接*/
public:
    MysqlConn();                 /* 数据库连接类构造函数*/

    MYSQL* AllocConn();                   /* 从数据库连接队列申请一个连接*/

    void FreeConn(MYSQL *pMysql);         /* 将一条数据库连接归还队列*/ 

    bool ExecuteInsertSql(MYSQL *p_mysql, char *sql);     /* 执行插入sql语句*/

};

#endif
