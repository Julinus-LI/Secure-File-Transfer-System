/*************************************************************************
	> File Name: main.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年05月01日 星期二 22时10分45秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
int main(void)
{
    int i = 0;
    while(1)
    {
        printf("hello world %d\n",i);
        i++;
        sleep(1);
        if( i  == 10000 )
            i = 0;            

    }
    return 0;
}
