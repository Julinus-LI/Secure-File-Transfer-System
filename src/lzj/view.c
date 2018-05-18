/*************************************************************************
	> File Name: view.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月30日 星期一 12时32分15秒
 ************************************************************************/

#include"view.h"


void Log()
{
	system("clear");
	printf("       \033[36m***********\033[0m\033[34mWelcome to Secure File Storage System\033[0m\033[36m***********\n\033[0m");
	printf("       \033[36m*\033[0m \033[31m        ******     ******     ******     ******   \033[0m  \033[36m    *\n\033[0m\033[0m");
	printf("       \033[36m*\033[0m \033[31m       **          *         **         **        \033[0m  \033[36m    *\n\033[0m\033[0m");
	printf("       \033[36m*\033[0m \033[31m        *****      ******     *****      *****    \033[0m  \033[36m    *\n\033[0m\033[0m");
	printf("       \033[36m*\033[0m \033[31m            **     *              **         **   \033[0m  \033[36m    *\n\033[0m\033[0m");
	printf("       \033[36m*\033[0m \033[31m       ******      *         ******     ******  \033[0m \033[34mKJC  \033[0m  \033[36m*\n\033[0m\033[0m");
	printf("       \033[36m***********************************************************\n\033[0m");
	sleep(1);
}

// 显示客户端登录成功后的界面

void Clie_Func()
{
    printf("   \033[34m------------------------------\033[31mClient  console\033[34m-----------------------------\033[0m\n");
    printf("   \033[34m|\033[0m                1.Update Files   2.Download Files  3.Exit               \033[34m|\033[0m\n");
    printf("   \033[34m--------------   \033[36mUse \033[31mls \033[36mor \033[31mcd \033[36mto \033[31mdisplayer \033[36mand \033[31mchange dir   \033[34m--------------\033[0m\n");    
}

// 显示服务器管理登录成功后的界面

void Serv_LoginSuc()
{
    printf("  \033[34m***********Server console***********\033[0m\n");
    printf("  \033[34m*\033[0m          1.Configure             \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          2.Run server            \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          3.Stop server           \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          4.Show status           \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m          5.Exit                  \033[34m*\033[0m\n");
    printf("  \033[34m************************************\033[0m\n");
    printf("  Please input the server command number:");
}

// 显示服务器配置界面

void Serv_Config()
{
    printf("  \033[34m***************Configure**************\033[0m\n");
    printf("  \033[34m*\033[0m        1.Set maximum client        \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m        2.Add admin account         \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m        3.Add client account        \033[34m*\033[0m\n");
    printf("  \033[34m*\033[0m        4.Go back                   \033[34m*\033[0m\n");
    printf("  \033[34m**************************************\033[0m\n");
    printf("  Please input the configuration command number:");
}
