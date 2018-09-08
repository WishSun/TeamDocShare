/*************************************************************************
	> File Name: MysqlConn.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年09月08日 星期六 11时41分12秒
 ************************************************************************/

#include "./MysqlConn.h"
#include <stdio.h>
#include <unistd.h>
#include <assert.h>


/* 数据库连接类构造函数*/
MysqlConn::MysqlConn()
{
    MYSQL *p_mysql = NULL;

    for(int i = 0; i < INIT_NUM; i++)
    {
        p_mysql = mysql_init(NULL);   
        assert( p_mysql != NULL );

        /* 连接数据库服务器*/
        assert(mysql_real_connect(p_mysql, "127.0.0.1", "root", "123456", NULL, 0, NULL, 0) != NULL);

        /* 选择数据库*/
        assert(mysql_select_db(p_mysql, "TeamDocShare") == 0 );

        /* 数据库连接句柄添加到连接队列*/
        m_connQue.push(p_mysql);
    }
}

/* 在不大于最大连接数的情况下添加APPEND_NUM条连接*/
void MysqlConn::AppendConn()
{
    MYSQL *p_mysql = NULL;

    for(int i = 0; i < APPEND_NUM; i++)
    {
        p_mysql = mysql_init(NULL);   
        assert( p_mysql != NULL );

        /* 连接数据库服务器*/
        assert(mysql_real_connect(p_mysql, "127.0.0.1", "root", "123456", NULL, 0, NULL, 0) != NULL);

        /* 选择数据库*/
        assert(mysql_select_db(p_mysql, "TeamDocShare") == 0 );

        /* 数据库连接句柄添加到连接队列*/
        m_connQue.push(p_mysql);
    }
}

/* 从数据库连接队列申请一个连接*/
MYSQL* MysqlConn::AllocConn()                   
{
    m_mutex.lock();

    /* 如果连接队列空了，则再配置几条*/
    if( m_connQue.empty() )
    {
        AppendConn();
    }

    MYSQL* p_mysql = m_connQue.front();
    m_connQue.pop();

    m_mutex.unlock();

    return p_mysql;
}


/* 将一条数据库连接归还队列*/ 
void MysqlConn::FreeConn(MYSQL *pMysql)         
{
    m_mutex.lock();

    m_connQue.push(pMysql);

    m_mutex.unlock();
}


/* 执行插入sql语句, 如果p_mysql为空，则申请一个连接使用完之后释放，如果p_mysql不为空，则使用完后不释放*/
bool MysqlConn::ExecuteInsertSql(MYSQL *p_mysql, char *sql)     
{
    int flag = false;
    if( p_mysql == NULL )
    {
        flag = true;

        /* 获取一条数据库连接*/
        p_mysql = AllocConn();
    }

    /* 执行sql语句*/
    if( mysql_query(p_mysql, sql) != 0 )
    {
        printf("query sql error: %s\n", mysql_error(p_mysql));

        if( flag )
        {
            /* 归还数据库连接*/
            FreeConn(p_mysql);
        }
        return false;
    }

    /* 获取插入新用户影响的行数*/
    int num = mysql_affected_rows(p_mysql);
 
    if( flag )
    {
        FreeConn(p_mysql);
    }

    if( num != 1 )
    {
        return false;
    }

    return true;   
}

