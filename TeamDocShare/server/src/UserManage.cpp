/*************************************************************************
	> File Name: UserManage.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 19时21分28秒
 ************************************************************************/

#include "./UserManage.h"


/*----------------------------------private----------------------------------*/
/* 用户管理类构造函数*/
UserManage::UserManage(int initNum, int maxNum)
{

}


/* 从数据库连接队列申请一个连接*/
MYSQL* UserManage::AllocConn()
{

}


/* 将一条数据库连接归还队列*/ 
void UserManage::FreeConn(MYSQL *pMysql)
{
    
}


/*----------------------------------public----------------------------------*/

/* 创建单例对象*/
UserManage* UserManage::CreateUserManage()
{
    
}


/* 查看某用户是否存在*/
bool UserManage::FindUser(UserInfo *pUser)
{
    
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
