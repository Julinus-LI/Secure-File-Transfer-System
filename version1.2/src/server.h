/*************************************************************************
	> File Name: server.h
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年05月01日 星期二 18时18分10秒
 ************************************************************************/

#ifndef _SERVER_H
#define _SERVER_H

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<strings.h>
#include<signal.h>
#include<time.h>
#include<dirent.h>
#include<errno.h>
#include<sys/vfs.h>
#include"threadpool.h"
#include"view.h"
#include"sql.h"

#define MAX_THR_NUM 100

// 最大客户端链接数
int maxClientNum = 0;

// 判断服务器是否开启

int isRun = 0;

// 判断是否退出服务器
int isExit = 0; 

//当前客户端链接数

int CurrentClientNum = 0;

int sockfd;

int tempsockfd;

pthread_t id;
// 端口号
int portNumber = 3333;

char clientIP[15];

// 线程池句柄

threadpool_t *pool = NULL;

// 自定义协议的传输结构
struct FilePackage
{
	char cmd;
	int  filesize;
	int  ack;
	char username[50];
	char filename[125];
    mode_t mode;        // 文件的权限
	char buf[1024];  
};
// ----------------------------------处理函数---------------------------------

/*打包函数*/
struct FilePackage pack(char tCmd, char* tBuf, char* tFilename, int tFilesize, int tAck,int count,char *uname);

// 解包函数

struct FilePackage unpack(int sockfd,struct FilePackage tpack);


// 初始化客户端最大连接数
int InitMaxClientNum();


// 主线程处理函数

void mainThread(void);

// 信号处理函数

void receivePipBroken(int signo);


// 菜单函数

void mainMenu(void);

// 线程池中线程执行任务
void* process(void *arg);

// 获取的当前时间

char *getCurrentTime();

// 获取文件列表

void getlist(char *username,char *filelist);

#endif
