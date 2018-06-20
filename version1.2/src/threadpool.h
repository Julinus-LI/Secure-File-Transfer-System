/*************************************************************************
	> File Name: threadpool1.h
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年05月02日 星期三 05时12分34秒
 ************************************************************************/

#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include<unistd.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<pthread.h>
#include<assert.h>

/* 
*线程池里所有运行和等待的任务都是一个CThread_worker 
*由于所有任务都在链表里，所以是一个链表结构 
*/ 
typedef struct worker 
{ 
    /*回调函数，任务运行时会调用此函数，注意也可声明成其它形式*/ 
    void *(*process) (void *arg); 
    void *arg;/*回调函数的参数*/ 
    struct worker *next; 

} CThread_worker; 



/*线程池结构*/ 
typedef struct 
{ 
    pthread_mutex_t queue_lock; 
    pthread_cond_t queue_ready; 

    /*链表结构，线程池中所有等待任务*/ 
    CThread_worker *queue_head; 

    /*是否销毁线程池*/ 
    int shutdown; 
    pthread_t *threadId;
    /*线程池中允许的活动线程数目*/ 
    int max_thread_num; 
    /*当前等待队列的任务数目*/ 
    int cur_queue_size; 

}threadpool_t; 


/* 创建线程池函数 */

threadpool_t *threadpool_create(int _max_thread_num);


/* 向线程池中添加任务 */
int threadpool_add_task(threadpool_t *pool,void *(*process)(void *arg),void *arg);


/* 销毁线程池,等待队列中的任务不会再被执行，但是正在运行的线程会一直把任务运行完后再退出 */
int threadpool_destroy(threadpool_t *pool);

// 线程处理函数

void *thread_routine(void *arg);

#endif
