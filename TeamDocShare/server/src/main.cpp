/*************************************************************************
	> File Name: main.cpp
	> Author: WishSun
	> Mail: WishSun_Cn@163.com
	> Created Time: 2018年08月23日 星期四 21时36分43秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "./Server.h"


/* 程序配置文件路径*/
#define CONF_PATH "../../common/etc/sevInfo.cfg"

char conf_path[ PATH_LENGTH ] = {0};

/* 获取当前运行程序的所在路径, 是为了得到配置文件的绝对路径*/
static int get_path()
{
    int ret = -1;
    char buff[ PATH_LENGTH ] = {0};
    char *ptr = NULL;

    /* 通过/proc/self/exe文件获取当前程序的运行绝对路径，并将路径放到 buff 中*/
    ret = readlink("/proc/self/exe", buff, PATH_LENGTH);
    if (ret < 0)
    {
        return -1;
    }

    /* 刚才获取的绝对路径包含了程序名，而我们需要的是当前程序所在的目录，所以不需要程序名( 找到最后一个'/', 并将其后面的部分截断 )*/
    ptr = strrchr(buff, '/');
    if ( ptr )
    {
        *ptr = '\0';
    }

    /* 将配置文件路径组装好并填充到 conf_path 中*/
    snprintf(conf_path, PATH_LENGTH, "%s/%s", buff, CONF_PATH); 

    return 0;
}

/* 从配置文件中解析出服务端ip地址和端口*/
int get_ip_port(char *conf_path, char *ip, int *port)
{
    if( conf_path == NULL )
    {
        printf("init_data arguments error!\n");
        return -1;
    }

    /* 打开配置文件*/
    if( open_conf( conf_path ) < 0 )
    {
        return -1;
    }

    const char *temp = "server_info.ip";
    /* 获取ip*/
    if( get_val_single( temp, ip, TYPE_STRING ) < 0 )
    {
        return -1;
    }
    /* 获取端口号*/
    const char *temp2 = "server_info.port";
    if( get_val_single( temp2, port, TYPE_INT ) < 0 )
    {
        return -1;
    }

    /* 关闭配置文件并释放资源*/
    close_conf();
    return 0;
}


int main(void)
{
    char ip[16] = {0};
    int port;

    if( get_path() < 0 )
    {
        printf("get configure file path error!");
        return -1;
    }
    printf("configure file path : %s\n", conf_path);

    if( get_ip_port(conf_path, ip, &port) < 0 )
    {
        printf("parse IP and port from configure file error!\n");
        return -1;
    }

    printf("ip: %s\tport: %d\n", ip, port);

    Server *pServer = Server::CreateServer(ip, port);
    pServer->Run();

    return 0;
}
