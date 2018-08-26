/*************************************************************************
	> File Name: UserManage.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分28秒
 ************************************************************************/

#include "./UserManage.h"
#include <assert.h>
#include <stdlib.h>

#define INIT_NUM  20
#define MAX_NUM   100

/*----------------------------------private----------------------------------*/

UserManage* UserManage::m_pUM = NULL;

/* 用户管理类构造函数*/
UserManage::UserManage(int initNum, int maxNum)
{
    MYSQL *p_mysql = NULL;

    for(int i = 0; i < initNum; i++)
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
MYSQL* UserManage::AllocConn()
{
    m_mutex.lock();

    MYSQL* p_mysql = m_connQue.front();
    m_connQue.pop();

    m_mutex.unlock();

    return p_mysql;
}


/* 将一条数据库连接归还队列*/ 
void UserManage::FreeConn(MYSQL *pMysql)
{
    m_mutex.lock();

    m_connQue.push(pMysql);

    m_mutex.unlock();
}


/*----------------------------------public----------------------------------*/

/* 创建单例对象*/
UserManage* UserManage::CreateUserManage()
{
    if(m_pUM == NULL)
    {
        m_pUM = new UserManage(INIT_NUM, MAX_NUM);
    }

    return m_pUM;
}


/* 析构函数*/
UserManage::~UserManage()
{
    /* 释放所有数据库连接*/
    while( !m_connQue.empty() )
    {
        MYSQL* p_mysql = m_connQue.front();
        mysql_close(p_mysql);
        m_connQue.pop();
    }
}


/* 查看某用户是否存在*/
bool UserManage::FindUser(UserInfo *pUser)
{
    char sql[1024] = {0};

    /* 获取一条数据库连接*/
    MYSQL *p_mysql = AllocConn();

    sprintf(sql, "select groupID from userInfo where userName='%s' and userPassword='%s';", pUser->m_userName, pUser->m_userPwd);

    /* 执行sql语句*/
    if( mysql_query(p_mysql, sql) != 0 )
    {
        printf("query sql error: %s\n", mysql_error(p_mysql));
        return false;
    }

    /* 获取执行结果集*/
    MYSQL_RES *p_res = mysql_store_result(p_mysql);
    if( p_res == NULL )
    {
        printf("store result error: %s\n", mysql_error(p_mysql));
        return false;
    }

    /* 获取结果集的行数*/
    int num = mysql_num_rows(p_res);

    /* 未查找到记录*/
    if( num == 0 )
    {
        return false;
    }

    MYSQL_ROW p_row = mysql_fetch_row(p_res);
    if( p_row != NULL )
    {
        pUser->m_groupID = atoi(p_row[0]);
    }

    /* 释放结果集*/
    mysql_free_result(p_res);

    /* 归还数据库连接*/
    FreeConn(p_mysql);

    return true;
}


/* 添加一个新用户记录*/
bool UserManage::AddNewUser(UserInfo *pUser)
{
    
}


/* 删除一个用户记录*/
bool UserManage::DelUser(char *pName)
{
    
}


/* 修改指定用户的组ID*/
bool UserManage::ChangeUserGroupID(char *pName, int gid)
{
    
}


/* 获取该用户的团队文件列表*/
int  UserManage::GetFileList(char *pName, int gid, char **pList)
{
    
}


/* 通过md5值获取文件路径*/
bool UserManage::GetFilePathInMD5(char *pMD5, char *filePath)
{
    
}


/* 设置md5值与文件路径映射*/
bool UserManage::SetFilePathtoMD5Map(char *pMD5, char *filePath)
{
    
}
