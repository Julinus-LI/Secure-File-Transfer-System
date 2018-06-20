/*************************************************************************
	> File Name: main.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年05月04日 星期五 02时16分38秒
 ************************************************************************/

#include<stdio.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<stdlib.h>
#include<string.h>

int main(void)
{
    int fd;
    if( (fd = open("./log.txt",O_RDWR|O_TRUNC)) <0)
    {
       perror("open error"); 
        exit(0);
    }
    char buf[1024] = "";
    strcpy(buf,"asdsjkfbsdjkbf");
    if( write(fd,buf,strlen(buf)) <0)
       {
          perror("write error"); 
           exit(0);
       }
    close(fd);
    return 0;
}
