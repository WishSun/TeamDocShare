/*************************************************************************
	> File Name: GroupManage.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分59秒
 ************************************************************************/

#include "./GroupManage.h"
#include <assert.h>

/*----------------------------------private----------------------------------*/

GroupManage* GroupManage::m_pGM = NULL;

/* 构造函数*/
GroupManage::GroupManage()   
{

}


/*----------------------------------public----------------------------------*/

/* 获取群组管理类单例对象*/
GroupManage* GroupManage::CreateGroupManage()  
{
    if(m_pGM == NULL)
    {
        m_pGM = new GroupManage();
    }

    return m_pGM;
}


/* 增加一个新的组*/
bool GroupManage::AddNewGroup(GroupInfo *pG)
{
    sprintf(sql, 
            "insert into groupInfo(groupName, groupMemberNum, groupInfo) values('%s', %d, '%s')",
            pG->m_groupName, 0, pG->m_groupInfo);

    return m_sqlConn.ExecuteInsertSql(NULL, sql);
}


/* 查找所有群组信息*/
bool GroupManage::GetGroupList(list<GroupInfo>& groupList)
{
    /* 获取一条数据库连接*/
    MYSQL *p_mysql = m_sqlConn.AllocConn();

    sprintf(sql, "select * from groupInfo");

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
    GroupInfo gInfo;
    while( (p_row = mysql_fetch_row(p_res)) )
    {
        gInfo.m_groupID = atoi(p_row[0]);
        sprintf(gInfo.m_groupName, "%s", p_row[1]);
        gInfo.m_groupMemNum = atoi(p_row[2]);
        sprintf(gInfo.m_groupInfo, "%s", p_row[3]);
        groupList.push_back(gInfo);
    }

    /* 释放结果集*/
    mysql_free_result(p_res);

    /* 归还数据库连接*/
    m_sqlConn.FreeConn(p_mysql);

    return true;
}
