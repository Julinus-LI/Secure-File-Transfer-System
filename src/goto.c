/*************************************************************************
	> File Name: goto.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月30日 星期一 12时17分26秒
 ************************************************************************/

#include<stdio.h>

int main()
{
    int a = 2, b = 3;
    if( b > a )
        goto aa;
    printf("hello\n");
    aa:printf("s\n");


    if(a>b)
        goto bbb;
    printf("hello1\n");
    bbb:printf("s1\n");

    return 0;
}
