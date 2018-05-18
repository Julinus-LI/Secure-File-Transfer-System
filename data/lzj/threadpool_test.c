/*************************************************************************
	> File Name: threadpool_test.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年05月03日 星期四 03时28分27秒
 ************************************************************************/

#include<stdio.h>
#include"threadpool.h"

void *myprocess(void *arg)
{
    printf ("threadid is 0x%x, working on task %d\n", (unsigned int)pthread_self (),*(int *) arg);
    sleep(1);
    return NULL;
}

int main(void)
{
    // 创建线程池
    threadpool_t *pool = threadpool_create(3);
    
    // 连续向线程池中投入 10 个任务
    int *workingnum = (int*)malloc(sizeof(int)*10);
    
    int i;
    for( i = 0;i<10;i++ )
    {
       workingnum[i]  = i;
        threadpool_add_task(pool,myprocess,(void*)&workingnum[i]);
    }

    // 等待所有任务完成
    sleep(5);
    // 销毁线程池
    threadpool_destroy(pool);
    free(workingnum);
    workingnum = NULL;
    return 0;
}
