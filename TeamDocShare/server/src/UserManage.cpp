/*************************************************************************
	> File Name: UserManage.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分28秒
 ************************************************************************/

#include "./UserManage.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define INIT_NUM  20
#define MAX_NUM   100

/*----------------------------------private----------------------------------*/

UserManage* UserManage::m_pUM = NULL;

/* 用户管理类构造函数*/
UserManage::UserManage()
{

}



/*----------------------------------public----------------------------------*/

/* 创建单例对象*/
UserManage* UserManage::CreateUserManage()
{
    if(m_pUM == NULL)
    {
        m_pUM = new UserManage();
    }

    return m_pUM;
}


/* 析构函数*/
UserManage::~UserManage()
{

}


/* 查看某用户是否存在*/
bool UserManage::FindUser(UserInfo *pUser)
{
    char sql[1024] = {0};

    /* 获取一条数据库连接*/
    MYSQL *p_mysql = m_sqlConn.AllocConn();

    sprintf(sql, "select groupID from userInfo where userName='%s' and userPassword='%s';", pUser->m_userName, pUser->m_userPwd);

    /* 执行sql语句*/
    if( mysql_query(p_mysql, sql) != 0 )
    {
        printf("query sql error: %s\n", mysql_error(p_mysql));

        m_sqlConn.FreeConn(p_mysql);
        return false;
    }

    /* 获取执行结果集*/
    MYSQL_RES *p_res = mysql_store_result(p_mysql);
    if( p_res == NULL )
    {
        printf("store result error: %s\n", mysql_error(p_mysql));
        
        m_sqlConn.FreeConn(p_mysql);
        return false;
    }

    /* 获取结果集的行数*/
    int num = mysql_num_rows(p_res);

    /* 未查找到记录*/
    if( num == 0 )
    {
        /* 释放结果集*/
        mysql_free_result(p_res);

        /* 归还数据库连接*/
        m_sqlConn.FreeConn(p_mysql);

        return false;
    }

    MYSQL_ROW p_row = mysql_fetch_row(p_res);
    if( p_row != NULL )
    {
        /* 如果未加入群组*/
        if( p_row[0] == NULL )
        {
            pUser->m_groupID = -1;
        }
        else
        {
            pUser->m_groupID = atoi(p_row[0]);
        }
    }

    /* 释放结果集*/
    mysql_free_result(p_res);

    /* 归还数据库连接*/
    m_sqlConn.FreeConn(p_mysql);

    return true;
}


/* 添加一个新用户记录*/
bool UserManage::AddNewUser(UserInfo *pUser)
{
    char sql[1024] = {0};

    sprintf(sql, "insert into userInfo(userName, userPassword) values('%s', '%s')", pUser->m_userName, pUser->m_userPwd);

    return m_sqlConn.ExecuteInsertSql(NULL, sql);
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
bool  UserManage::GetFileList(char *pName, int gid, list<FileListInfo> &pFList) 
{
    char sql[1024] = {0};

    /* 获取一条数据库连接*/
    MYSQL *p_mysql = m_sqlConn.AllocConn();

    if( gid != -1 )
    {
        /* 查找出该用户的所有团队成员*/
        sprintf(sql, "select userName from userInfo where groupID=%d", gid);
        
        /* 执行sql语句*/
        if( mysql_query(p_mysql, sql) != 0 )
        {
            printf("query sql error: %s\n", mysql_error(p_mysql));

            m_sqlConn.FreeConn(p_mysql);
            return false;
        }

        /* 获取执行结果集*/
        MYSQL_RES *p_res = mysql_store_result(p_mysql);
        if( p_res == NULL )
        {
            printf("store result error: %s\n", mysql_error(p_mysql));

            m_sqlConn.FreeConn(p_mysql);
            return false;
        }

        /* 根据所有成员用户名查找文件路径*/
        sprintf(sql, "select filePath from userFileMap where userName=");
        char *pSql2 = sql;

        MYSQL_ROW p_row = mysql_fetch_row(p_res);
        while( p_row != NULL )
        {
            sprintf(pSql2, "'%s' ", p_row[0]);
            pSql2 += strlen(pSql2);

            p_row = mysql_fetch_row(p_res);
            if( p_row != NULL )
            {
                sprintf(pSql2, "or userName=");
                pSql2 += strlen(pSql2);
            }
        }

        /* 释放结果集*/
        mysql_free_result(p_res);
    }
    else
    {
        sprintf(sql, "select filePath from userFileMap where userName='%s'", pName);
    }

    /* 执行sql语句*/
    if( mysql_query(p_mysql, sql) != 0 )
    {
        printf("query sql error: %s\n", mysql_error(p_mysql));

        m_sqlConn.FreeConn(p_mysql);
        return false;
    }

    /* 获取执行结果集*/
    MYSQL_RES *p_res = mysql_store_result(p_mysql);
    if( p_res == NULL )
    {
        printf("store result error: %s\n", mysql_error(p_mysql));

        m_sqlConn.FreeConn(p_mysql);
        return false;
    }

    MYSQL_ROW p_row = NULL;
    FileListInfo fileInfo;
    while( (p_row = mysql_fetch_row(p_res)) )
    {
        sprintf(fileInfo.m_filePath, "%s", p_row[0]);
        pFList.push_back(fileInfo);
    }

    /* 释放结果集*/
    mysql_free_result(p_res);

    /* 归还数据库连接*/
    m_sqlConn.FreeConn(p_mysql);

    return true;
}


/* 通过md5值获取文件路径*/
bool UserManage::GetFilePathInMD5(char *pMD5, char *filePath)
{
    char sql[1024] = {0};

    /* 获取一条数据库连接*/
    MYSQL *p_mysql = m_sqlConn.AllocConn();

    sprintf(sql, "select filePath from md5Map where md5Value='%s';", pMD5);

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

        m_sqlConn.FreeConn(p_mysql);
        return false;
    }

    /* 获取结果集的行数*/
    int num = mysql_num_rows(p_res);

    /* 未查找到记录*/
    if( num == 0 )
    {
        /* 释放结果集*/
        mysql_free_result(p_res);
        m_sqlConn.FreeConn(p_mysql);
        return false;
    }

    MYSQL_ROW p_row = mysql_fetch_row(p_res);
    if( p_row != NULL )
    {
        if( p_row[0] != NULL )
        {
            sprintf(filePath, "%s", p_row[0]);
        }
    }

    /* 如果该文件已不存在, 则删除这条记录，并返回false*/
    if(access(filePath, F_OK) != 0)
    {
        sprintf(sql, "delete from md5Map where md5Value='%s';", pMD5);

        if( mysql_query(p_mysql, sql) != 0 )
        {
            printf("query sql error: %s\n", mysql_error(p_mysql));
        }

        mysql_free_result(p_res);
        m_sqlConn.FreeConn(p_mysql);
        return false;
    }

    mysql_free_result(p_res);
    m_sqlConn.FreeConn(p_mysql);
    return true;   
}


/* 设置md5值与文件路径映射*/
bool UserManage::SetFilePathtoMD5Map(char *pMD5, char *filePath)
{
    char sql[1024] = {0};

    /* 获取一条数据库连接*/
    MYSQL *p_mysql = m_sqlConn.AllocConn();

    /* 首先删除原来的记录(如果文件被修改的话)*/
    sprintf(sql, "delete from md5Map where filePath='%s'", filePath);


    /* 执行删除语句(不论成败)*/
    mysql_query(p_mysql, sql);

    /* 然后添加新的记录*/
    sprintf(sql, "insert into md5Map values('%s', '%s')", pMD5, filePath);

    bool result = m_sqlConn.ExecuteInsertSql(p_mysql, sql);
    
    /* 归还数据库连接*/
    m_sqlConn.FreeConn(p_mysql);

    return result; 
}


/* 设置文件路径与用户名的映射*/
bool UserManage::SetFilePathtoUserName(char *filePath, char *uName) 
{
    char sql[1024] = {0};

    /* 添加新的记录*/
    sprintf(sql, "insert into userFileMap values('%s', '%s')", uName, filePath);
    
    return m_sqlConn.ExecuteInsertSql(NULL, sql);
}
