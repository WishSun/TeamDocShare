/*************************************************************************
	> File Name: UploadTask.h
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月24日 星期五 09时26分36秒
 ************************************************************************/

#ifndef _UPLOADTASK_H
#define _UPLOADTASK_H

#include "../../common/inc/common.h"
#include <sys/stat.h>


struct UploadTask
{
    char    m_filePath[ PATH_LENGTH ];  /* 文件全路径名*/
    mode_t  m_fileMode;   /* 文件权限*/
    int64_t m_fileSize;   /* 文件大小*/
};
#endif
