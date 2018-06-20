/*************************************************************************
	> File Name: client.h
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月30日 星期一 11时51分09秒
 ************************************************************************/

#ifndef _CLIENT_H
#define _CLIENT_H

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/vfs.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<strings.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<pthread.h>
#include<openssl/ssl.h>
#include<openssl/err.h>
#include"view.h"


// IP 地址缓存区大小
#define SERV_IP 15

char serv_ip[SERV_IP];

char tempusername[20] = "";

struct info
{
    int sockfd;
    char filename[1024];
};


SSL_CTX*  ctx = NULL;
SSL *ssl = NULL;
//int sockfd;

struct FilePackage
{
	char cmd;           // 请求类型
	int  filesize;      // 文件大小
	int  ack;           // 确认号
	char username[50];  // 用户名
	char filename[125]; // 文件名
    mode_t mode;        // 文件的权限
	char buf[1024];     // 传输的数据
};


//~~~~~~~~~~~~~~~~~~~~~~~~打包数据~~~~~~~~~~~~~~~~~~~~~~~~~
struct FilePackage pack(char tCmd, char* tBuf, char* tFilename, int tFilesize, int tAck,int count,char *uname);
/*
请求：
Cmd：R
客服端发送数据包：0 请求连接

服务器发送数据包：1答应请求


登陆: 
Cmd:L    用户名与密码放在buf内以*号隔开如 admin*123
服务器发送数据包的ACK：0 用户名或密码错误
     	       	         1 登陆成功
     	                 2 客户端最大连接数

客服端发送数据包的ACK：9 登陆服务器  


下载：
Cmd：D
服务器发送数据包的ACK：0 接受下载，返回待下载文件大小
		       						 2 开始下载
											 4 下载完毕
											 		
客服端发送数据包的ACK：9 请求下载
		       						 1 本地磁盘空间不足
		       						 3 接受完毕


上传：
Cmd：U
服务器发送数据包的ACK：0 接受上传请求
		                   1 本地磁盘空间不足
		                   3 接收完毕

客服端发送数据包的ACK：9 请求上传，上传文件大小,文件名
		                   2 开始上传文件
		                   4 上传完毕
		       

显示文件列表
Cmd：S
服务器发送数据包的ACK: 0 第一个显示文件列表
		                  

客服端发送数据包的ACK：9 请求显示

退出
Cmd: Q
客服端发送数据包的ACK：0
*/

// -------------------------------处理函数-------------------

// 客户单连接服务器函数
int connectto(int argc,char *argv[]);

//  用户登录
int login();

// 发送数据包
int senddata(int sockfd,struct FilePackage data);

// 接收数据包

int recvdata(int sockfd,struct FilePackage *data);

// 处理函数

void* mainHandler(void *arg);

// 显示客户单端目录

void Show(int sockfd,char temp[100]);

// 上传文件大

//void *UpdateF(void *filename);
void UpdateF(struct info sf);

// 下载文件

//void *DownloadF(void *filename);
void DownloadF(struct info sf);

// 显示证书
void ShowCerts(SSL *ssl);
#endif
