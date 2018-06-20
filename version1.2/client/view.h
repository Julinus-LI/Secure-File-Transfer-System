/*************************************************************************
	> File Name: view.h
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月30日 星期一 12时31分22秒
 ************************************************************************/

#ifndef _VIEW_H
#define _VIEW_H
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>


// 客户端和服务器端显示界面
void Log();

// 显示客户端登录成功后的界面

void Clie_Func();

// 显示服务器管理登录成功后的界面

void Serv_LoginSuc();

// 显示服务器配置界面

void Serv_Config();
#endif
