/*************************************************************************
	> File Name: sql.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月30日 星期一 13时07分48秒
 ************************************************************************/

#include"sql.h"


// 判断管理员是否在数据库中
int CheckAdmin(char *AdminId,char *AdminPwd)
{
    int res = 0;
    MYSQL *p_mysql = NULL;
    
    // 初始化 mysql 句柄
    if( ( p_mysql = mysql_init(NULL)) == NULL)
    {
       printf("init mysql error"); 
       return 0;
    }
    // 连接诶数据库
    if(mysql_real_connect(p_mysql,"127.0.0.1","root","lzj201511",NULL,0,NULL,0) == NULL)
    {
       printf("connect mysql server error:%s\n",mysql_error(p_mysql)); 
       return 0;
    }

    //选择进行操作的数据库
    //if(mysql_selcet_db(p_mysql,"file_trans") != 0)
     if(mysql_select_db(p_mysql,"file_trans") !=0)
    {
       printf("select db error:%s\n",mysql_error(p_mysql));
       return 0;
    }

    char sql_buf[128] = "";
    sprintf(sql_buf,"select *from admin");
    // 查询数据库
    if(mysql_query(p_mysql,sql_buf) != 0)
    {
        printf("query sql error:[%s]\n",mysql_error(p_mysql));
        return 0;
    }


    // 保存查询结果 
    MYSQL_RES *p_res = NULL;
    p_res = mysql_store_result(p_mysql);
    if( p_res == NULL )
    {
        printf("store result error:%s!\n",mysql_error(p_mysql));
        
        return 0;
    }
    // 获取结果条数
    int num_row = mysql_num_rows(p_res);
    // 获取结果列数
    int num_col = mysql_num_fields(p_res);
   
    MYSQL_ROW p_row;
    while( (p_row = mysql_fetch_row(p_res)) != NULL)
    {
        int i;
        char name[20]="";
        char pwd[20]="";
        
        if( p_row[0] != NULL)
            strcpy(name,p_row[0]);
        if( p_row[1] != NULL)
            strcpy(pwd,p_row[1]);

        if(strcmp(AdminId,name) == 0)
        {
            if(strcmp(AdminPwd,pwd) == 0)
            {
                //printf("\n\033[33mlogin success!\033[0m\n\n");
                res = 1;
                break;
            }
            else
            {
                res = 0;
                memset(name,0x00,20);
                memset(pwd,0x00,20);
                //printf("\n\033[33mAdmin name or passwd is error!\033[0m\n\n");
            }
        }
    }

    // 释放 mysql 查询结果占用的空间
    mysql_free_result(p_res);
    mysql_close(p_mysql);

    return res;
}


// 新增管理用户进入数据库

int Add_admin_user(char *NewAdminId,char *NewAdminPwd)
{
    MYSQL *p_mysql = NULL;
    
    // 初始化 mysql 句柄
    if( ( p_mysql = mysql_init(NULL)) == NULL)
    {
       printf("init mysql error"); 
       return 0;
    }
    // 连接诶数据库
    if(mysql_real_connect(p_mysql,"127.0.0.1","root","lzj201511",NULL,0,NULL,0) == NULL)
    {
       printf("connect mysql server error:%s\n",mysql_error(p_mysql)); 
       return 0;
    }

    //选择进行操作的数据库
    //if(mysql_selcet_db(p_mysql,"file_trans") != 0)
     if(mysql_select_db(p_mysql,"file_trans") !=0)
    {
       printf("select db error:%s\n",mysql_error(p_mysql));
       return 0;
    }

    char sql_buf[128] = "";
    sprintf(sql_buf,"insert into admin(Id,pwd) values('%s','%s')",NewAdminId,NewAdminPwd);
   if(mysql_query(p_mysql,sql_buf)!=0)
   {
        printf("query sql error:[%s]\n",mysql_error(p_mysql));
        return 0;
   }

    // 释放 mysql 查询结果占用的空间
    mysql_close(p_mysql);

    return 1;
}

// 新用户注册

int Add_user_user(char* NewUserId,char *NewUserPwd)
{
     MYSQL *p_mysql = NULL;
    
    // 初始化 mysql 句柄
    if( ( p_mysql = mysql_init(NULL)) == NULL)
    {
       printf("init mysql error"); 
       return 0;
    }
    // 连接诶数据库
    if(mysql_real_connect(p_mysql,"127.0.0.1","root","lzj201511",NULL,0,NULL,0) == NULL)
    {
       printf("connect mysql server error:%s\n",mysql_error(p_mysql)); 
       return 0;
    }

    //选择进行操作的数据库
    //if(mysql_selcet_db(p_mysql,"file_trans") != 0)
     if(mysql_select_db(p_mysql,"file_trans") !=0)
    {
       printf("select db error:%s\n",mysql_error(p_mysql));
       return 0;
    }

    char sql_buf[128] = "";
    sprintf(sql_buf,"insert into user(Id,pwd) values('%s','%s')",NewUserId,NewUserPwd);
   if(mysql_query(p_mysql,sql_buf)!=0)
   {
        printf("query sql error:[%s]\n",mysql_error(p_mysql));
        return 0;
   }

    // 释放 mysql 查询结果占用的空间
    mysql_close(p_mysql);
    return 1;
}

// 检查用户是否在数据库中

int CheckUser(char *userId,char *userPwd)
{
    int res = 0;
    MYSQL *p_mysql = NULL;

    // 初始化 mysql 句柄
    if( ( p_mysql = mysql_init(NULL)) == NULL)
    {
        printf("init mysql error"); 
        return 0;
    }
    // 连接诶数据库
    if(mysql_real_connect(p_mysql,"127.0.0.1","root","lzj201511",NULL,0,NULL,0) == NULL)
    {
        printf("connect mysql server error:%s\n",mysql_error(p_mysql)); 
        return 0;
    }

    //选择进行操作的数据库
    //if(mysql_selcet_db(p_mysql,"file_trans") != 0)
    if(mysql_select_db(p_mysql,"file_trans") !=0)
    {
        printf("select db error:%s\n",mysql_error(p_mysql));
        return 0;
    }

    char sql_buf[128] = "";
    sprintf(sql_buf,"select *from user");
    // 查询数据库
    if(mysql_query(p_mysql,sql_buf) != 0)
    {
        printf("query sql error:[%s]\n",mysql_error(p_mysql));
        return 0;
    }


    // 保存查询结果 
    MYSQL_RES *p_res = NULL;
    p_res = mysql_store_result(p_mysql);
    if( p_res == NULL )
    {
        printf("store result error:%s!\n",mysql_error(p_mysql));

        return 0;
    }
    // 获取结果条数
    int num_row = mysql_num_rows(p_res);
    // 获取结果列数
    int num_col = mysql_num_fields(p_res);

    MYSQL_ROW p_row;
    while( (p_row = mysql_fetch_row(p_res)) != NULL)
    {
        int i;
        char name[20]="";
        char pwd[20]="";

        if( p_row[0] != NULL)
        strcpy(name,p_row[0]);
        if( p_row[1] != NULL)
        strcpy(pwd,p_row[1]);

        if(strcmp(userId,name) == 0)
        {
            if(strcmp(userPwd,pwd) == 0)
            {
                //printf("\n\033[33mlogin success!\033[0m\n\n");
                res = 1;
                break;
            }
            else
            {
                res = 0;
                memset(name,0x00,20);
                memset(pwd,0x00,20);
                //printf("\n\033[33mAdmin name or passwd is error!\033[0m\n\n");
            }
        }
    }

    // 释放 mysql 查询结果占用的空间
    mysql_free_result(p_res);
    mysql_close(p_mysql);

    return res;   

}
