/*************************************************************************
	> File Name: test_sql.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年05月02日 星期三 01时20分03秒
 ************************************************************************/

#include<stdio.h>
#include"sql.h"

int main(void)
{
    int res = CheckAdmin("abc","111");
    if(res)
    {
        printf("true\n");
    }
    else
    {
        printf("false\n");
    }
    
    res =CheckUser("lzj","111");
    if(res)
    {
        printf("true\n");
    }
    else
    {
        printf("false\n");
    }
   
    return 0;
}
