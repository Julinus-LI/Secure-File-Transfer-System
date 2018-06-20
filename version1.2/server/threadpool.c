/*************************************************************************
	> File Name: threadpool.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月25日 星期三 19时03分54秒
 ************************************************************************/

#include"threadpool.h"


/* 创建线程池函数 */

threadpool_t *threadpool_create(int _max_thread_num)
{
    threadpool_t *pool = NULL;
   /* 为线程池动态申请内存,开辟空间*/ 
    pool = (threadpool_t*) malloc(sizeof(threadpool_t));
    if( NULL == pool )
    {
       printf("threadpool create failed!\n"); 
        return NULL;
    }
    
    /* 初始化互斥量 */
    pthread_mutex_init(&pool->queue_lock,NULL);

    /* 初始化条件变量 */
    pthread_cond_init(&pool->queue_ready,NULL);

    /* 初始化任务队列 */
    pool->queue_head = NULL;

    /* 初始化允许线程活动的最大数目 */
    pool->max_thread_num = _max_thread_num;

    /* 初始化当前队列的大小 */
    pool->cur_queue_size = 0;

    /* 不销毁当前线程池*/
    pool->shutdown = 0; 

    /* 为线程 id 数组开辟空间 */
    pool->threadId = (pthread_t*)malloc(sizeof(pthread_t)*_max_thread_num);
    if( NULL == pool->threadId )
    {
        printf("pthreadId create failed\n");
        return NULL;
    }

    /* 开始为线程池创建 _max_thread_num 线程的个数*/
    int i;
    for(i = 0; i < _max_thread_num; i++ )
    {
        pthread_create(&pool->threadId[i],NULL,thread_routine,(void*)pool);
    }

    /* 返回线程池句柄*/
    return pool;
}


/* 向线程池中添加任务 */
int threadpool_add_task(threadpool_t *pool,void *(*process)(void *arg),void *arg)
{
    /* 上锁*/
    pthread_mutex_lock(&pool->queue_lock);

    /* 构造一个新的任务 */
    CThread_worker *newWorker =  (CThread_worker*)malloc(sizeof(CThread_worker));
    if( NULL == newWorker)
    {
        printf("threadpool_add_task faile!\n"); 
        return -1;
    }
    
    newWorker->process = process;
    newWorker->arg = arg;
    newWorker->next = NULL;


    /* 将任务加入到等待队列中,放入对尾 */
    CThread_worker *member = pool->queue_head;
    if( member != NULL )
    {
       while(member->next != NULL) 
            member = member->next;
        member->next = newWorker;
    }
    else
    {
       pool->queue_head = newWorker; 
    }
   
    assert(pool->queue_head != NULL);
    /* 当前任务队列的任务数加 1 */
    pool->cur_queue_size++;

    /* 释放锁*/
    pthread_mutex_unlock(&pool->queue_lock);
    
    /*等待队列中增加任务了，唤醒线程池中等待的一个线程；注意：如果所有线程都在忙碌，那么下面这句话没有作用*/
    pthread_cond_signal(&pool->queue_ready);
    
    
    return 0;
}

/* 销毁线程池,等待队列中的任务不会再被执行，但是正在运行的线程会一直把任务运行完后再退出 */
int threadpool_destroy(threadpool_t *pool)
{
    /* 防止两次调用，即已经释放了线程池资源，然后再次释放资源*/
   if(pool->shutdown) 
        return -1;
    pool->shutdown = 1;

    /* 唤醒所有等待线程，线程池要销毁了*/
    pthread_cond_broadcast(&pool->queue_ready);
    
    /* 阻塞等待线程退出，否则就变成了僵尸线程*/
    int i;
    for( i=0; i<pool->max_thread_num;i++ )
    {
        pthread_join(pool->threadId[i],NULL);
    }

    /* 释放线程id 数组 */
    free(pool->threadId);
    pool->threadId = NULL;

    /* 销毁等待队列 */
    CThread_worker *head = NULL;

    while(pool->queue_head != NULL)
    {
        head = pool->queue_head;
        pool->queue_head = head->next;
        free(head);
        head = NULL;

    }

    /*销毁条件变量和互斥变量 */
    pthread_mutex_destroy(&pool->queue_lock);
    pthread_cond_destroy(&pool->queue_ready);

    /* 销毁线程池句柄*/
    free(pool);
    pool = NULL;
    return 0;
}


// 线程处理函数

void *thread_routine(void *arg)
{
    threadpool_t * pool = (threadpool_t*)arg;
//   printf("starting thread 0x%x\n",(unsigned int)pthread_self()); 
   
    while(1)
   {
        /* 给线程上锁，进入临街资源区*/
        pthread_mutex_lock(&pool->queue_lock);
        
        /* 如果等待队列为 0 并且不销毁线程池，则线程池处于阻塞状态；注意
         * pthread_cond_wait() 是一个原子操作，等待前会解锁，唤醒后会加锁 */
        while(pool->cur_queue_size == 0 && !pool->shutdown)
        {
//            printf("thread 0x%x is waiting\n",(unsigned int)pthread_self()); 
            pthread_cond_wait(&pool->queue_ready,&pool->queue_lock);
        }
        
        /* 线程池要销毁了，要关闭线程池里的每个线程,自行退出处理*/
        if(pool->shutdown)
        {
            pthread_mutex_unlock(&pool->queue_lock);
 //           printf("thread 0x%x will exit\n ",(unsigned int)pthread_self());

            /* 退出当前线程 */
            pthread_exit(NULL);
        }
  
       
        /* 任务队列为不空*/
        /* 调试信息：如果当前任务队列是否有任务,则继续向下执行程序；否则，程序出错 */
        assert(pool->cur_queue_size != 0);

        /* 调试信息：如果当前任务队列不存在的话，程序出错*/
        assert(pool->queue_head != NULL);
    
        /*等待队列中的任务数 减1 ，从队列中取出任务 */
        pool->cur_queue_size--;
        CThread_worker *worker = pool->queue_head; 
        pool->queue_head = worker->next;
        /* 任务队列中的任务数减 1 */
        pthread_mutex_unlock(&pool->queue_lock);
        
        /* 调用回调函数，执行任务 */
        (*(worker->process))(worker->arg);
        
        /* 释放结点*/
        free(worker);
        worker = NULL;
    }
    /* 这一句话是不可达的 */
    pthread_exit(NULL);
}
