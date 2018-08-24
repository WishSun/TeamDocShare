/*************************************************************************
	> File Name: GroupManage.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分59秒
 ************************************************************************/

#include "./GroupManage.h"

/*----------------------------------private----------------------------------*/

/* 构造函数*/
GroupManage::GroupManage(int initNum, int maxNum)   
{

}

/* 获取一个数据库连接*/
MYSQL* GroupManage::AllocConn()                 
{

}


/* 归还一个数据库连接*/
void GroupManage::FreeConn(MYSQL *pMysql)
{
    
}


/*----------------------------------public----------------------------------*/

/* 获取群组管理类单例对象*/
GroupManage* GroupManage::CreateGroupManage()  
{
    
}


/* 增加一个新的组*/
bool GroupManage::AddNewGroup(GroupInfo *pG)
{
    
}


/* 查找所有群组信息*/
GroupInfo* GroupManage::GetGroupList()
{
    
}
