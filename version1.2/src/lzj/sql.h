/*************************************************************************
	> File Name: sql.h
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月30日 星期一 13时06分55秒
 ************************************************************************/

#ifndef _SQL_H
#define _SQL_H
#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<mysql/mysql.h>


// 判断管理员是否在数据库中

int CheckAdmin(char *AdminId,char *AdminPwd);

// 新增管理用户进入数据库

int Add_admin_user(char *NewAdminId,char *NewAdminPwd);

// 新用户注册

int Add_user_user(char* NewUserId,char *NewUserPwd);

// 检查用户是否在数据库中

int CheckUser(char *userId,char *userPwd);

#endif
