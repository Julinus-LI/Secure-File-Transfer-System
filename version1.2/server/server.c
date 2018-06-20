/*************************************************************************
	> File Name: server.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年05月01日 星期二 18时18分03秒
 ************************************************************************/

#include"server.h"
int main(int argc,char *argv[])
{
    // 界面线程 ID 
    pthread_t controlId;
    // 处理数据线程 ID
    pthread_t mainId;

    // 初始化客户端最大连接数
    maxClientNum = InitMaxClientNum();

    // 显示文件传输系统的界面
    Log();
    
    printf("maxClientNum = %d\n",maxClientNum);

    int flag = 0;
    char AdminName[20] ="";
    char AdminPwd[20] ="";
   
    // 管理员登录
    while(flag != 1)
    {
        printf("Admin Id:"); 
        scanf("%s",AdminName);
        printf("Admin PassWord:");
        scanf("%s",AdminPwd);
        
        int res = CheckAdmin(AdminName,AdminPwd);
        if( res )
        {
            flag = 1;
            printf("\n\033[33mlogin success!\033[0m\n\n");
            break;
        }
        else
        {
            flag = 0;
            memset(AdminName,0x00,20);
            memset(AdminPwd,0x00,20);
           printf("\n\033[33mAdmin name or passwd is error!\033[0m\n\n");
        }
        
    }

    // 开启主界面线程
    if(pthread_create(&controlId,NULL,(void*)mainMenu,NULL) != 0)
    {
       printf("\033[33mCreate menu error!\033[0m\n");
    }

    // 开启处理函数线程
    if(pthread_create(&mainId,NULL,(void*)mainThread,NULL) != 0)
    {
        printf("\033[33mCreate mainTHread thread error!\033[0m\n");
    }

    pthread_join(controlId,NULL);
    pthread_join(mainId,NULL);
    return 0;
}

// 线程池中线程执行任务
void *process(void *arg) 
{
    struct FilePackage sendPackage;
    // 当前客户端连接数 加 1
      ++CurrentClientNum;

    int clie_fd = *(int*)arg;
 
	SSL *ssl = NULL;
	
	/* 基于 ctx 产生一个新的 SSL */
	SSL_new(ctx);
	
	 /* 将连接用户的 socket 加入到 SSL */
	SSL_set_fd(ssl,clie_fd);

	/*建立 SSL 连接 */
	if(SSL_accept(ssl) == -1)
	{
		perror("SSL_accept error");
		close(clie_fd);
	}
	
	SSL* NewFd = ssl;

   // 客户端连接到最大
    if(CurrentClientNum > maxClientNum)
    {
        sendPackage=pack('L'," "," ",0,2,1,"");
       if(send(clie_fd,&sendPackage,sizeof(struct FilePackage),0) == -1) 
        {
            printf("send sendPackage error"); 
            return NULL;
        }
        --CurrentClientNum;
        close(clie_fd);
        return NULL;
    }
 
    /* 关闭 SSL连接*/
	
	SSL_shutdown(NewFd);

	/* 释放 SSL*/
	SSL_free(NewFd);

   struct FilePackage buff;
    while(1)
    {
        memset(&buff,0x00,sizeof(struct FilePackage));
        if( recv(clie_fd,&buff,sizeof(struct FilePackage),0) == -1)        
        {
            perror("recv data error");
        }
      
      //  printf("buff.cmd = %c,buff.ack = %d\n",buff.cmd,buff.ack);

        if( buff.cmd == 'Q' && buff.ack == '0' )
        //if( buff.cmd == 'Q' && buff.ack == 0 )
        {
            /* 关闭 SSL连接*/
            SSL_shutdown(NewFd);
            /* 释放 SSL*/
            SSL_free(NewFd);
            close(clie_fd);
            break;
        }
        else
        {
            sendPackage = unpack(clie_fd,buff);
            if(sendPackage.cmd!='\0')
            {
                send(clie_fd,&sendPackage,sizeof(struct FilePackage),0);
                memset(&sendPackage,0x00,sizeof(struct FilePackage));
            }
        }
    }
    printf("process: CurrentClientNum = %d\n",CurrentClientNum);
    --CurrentClientNum;
}


// 解包函数

struct FilePackage unpack(int sockfd,struct FilePackage tpack)
{
   struct FilePackage sendPack;
    char username[20] = "";
    char userpwd[20] = "";
    char pfilename[256] = "";
    char *pUser = tpack.buf;
    int filesize = 0;
    int currentFsize = 0;
    int fd;
    int fdlog;
    char filelist[1024] = "";    
    int flag = 1;
    
    //printf("tpack.cmd = %c,tpack.filename = %s,tpack.ack = %d,tpack.filesize = %d,tpack.mode = %d\n",tpack.cmd,tpack.filename,tpack.ack,tpack.filesize,tpack.mode);
    switch(tpack.cmd)
    {
//        printf("tpack.cmd = %c\n",tpack.cmd);
        // 登录请求
        case 'L':
        {
            int i = 0;
            while(*pUser != '*')
            {
                if(i<20)
                {
                    username[i] = *pUser;
                    i++;
                    ++pUser;
                }
            }
            ++pUser;
            i = 0;
            while(*pUser != '#')
            {
                 if(i<20)
                {
                    userpwd[i] = *pUser;
                    i++;
                    ++pUser;
                }
            }
 //           printf("username = %s,userpwd = %s\n",username,userpwd);
            if(CheckUser(username,userpwd) == 1) 
            {
                sendPack = pack('L',"","",0,1,1,"");
                strcpy(filelist,"");
            }
            else
            {
               sendPack=pack('L',"","",0,0,1,"");                          /*登陆失败*/
            }
                         
            return sendPack;
        }
        break;
        case 'U':
        {
          struct statfs stafbuf; 
            int count = 0;
            currentFsize = 0;
            mode_t mode = tpack.mode;
            
            if(tpack.ack == 9)
            {
  //              printf("tapck.username = %s\n",tpack.username);
                strcat(pfilename,"../data/");
                strcat(pfilename,tpack.username);
                strcat(pfilename,"/");
                strcat(pfilename,tpack.filename);
                filesize = tpack.filesize;
                // 文件名存在以后实现
                statfs("./",&stafbuf);
                if( stafbuf.f_bsize * stafbuf.f_bfree <= filesize )
                {
                    printf("\033[31m磁盘空间不足\033[0m\n");
                    sendPack=pack('U',"","",0,1,1,"");
                    send(sockfd,&sendPack,sizeof(struct FilePackage),0);
                    exit(1);
                }
                printf("server: pfilename = %s\n",pfilename);
                printf("opne pfilename mode is %o\n",mode);

                if((fd = open(pfilename,O_CREAT|O_RDWR,mode))<0)
                {
                    perror("open error"); 
                }
                sendPack=pack('U',"","",0,0,1,"");
                //send(sockfd,&sendPack,sizeof(struct FilePackage),0);

                SSL_write(sockfd,&sendPack,sizeof(struct FilePackage));
                struct FilePackage buff;
                memset(&buff,0x00,sizeof(struct FilePackage));
                //if((count = recv(sockfd,&buff,sizeof(struct FilePackage),0)) == -1)
                
                if((count = SSL_read(sockfd,&buff,sizeof(struct FilePackage))) == -1)
                {
                    perror("read error:\n");
                }
                while(buff.ack == 2)
                {
                    count = buff.filesize;
                    if( write(fd,buff.buf,count) == -1 )
                    {
                        perror("wirte error:\n");     
                    }
                    memset(&buff,0x00,sizeof(struct FilePackage));
                    //if( recv(sockfd,&buff,sizeof(struct FilePackage),0) == -1)
                
                    if( SSL_read(sockfd,&buff,sizeof(struct FilePackage)) == -1)
                    {
                        perror("read error:\n");     
                    }
                }// 文件上传结束
           
                // 写入日志
                char Log[256] = "";
                strcat(Log,"");
                strcat(Log,"Clien IP: ");
                strcat(Log,clientIP);
               
                strcat(Log,"\n");
                
                strcat(Log,"upload Date: ");
                strcat(Log,getCurrentTime());

                strcat(Log,"File name: ");
                strcat(Log,pfilename);
                
                strcat(Log," \n");

                if( (fdlog = open("../log/log.txt",O_WRONLY|O_APPEND)) == -1)
                {
                    printf("\033[33mlog.sys file open error!\033[0m\n");
                    exit(-1);
                }
                
                if(write(fdlog,Log,strlen(Log)) <0)
                {
                    perror("write long.txt error:\n"); 
                    exit(-1);
                }

                close(fdlog);
                
                if(buff.ack == 4)
                {
                   sendPack = pack('U',"","",0,3,1,"");
                    close(fd);
                }
                return sendPack;
            }
        }
        break;
        case 'S':
        {
   //         printf("tpack.username = %s,filelist = %s\n",tpack.username,filelist);

            getlist(tpack.username,filelist);
            sendPack = pack('S',filelist,"",0,1,strlen(filelist)+1,"");
            
            strcpy(filelist,"");
            return sendPack;
        }
        break;
        case 'D':
        {
            int Fd;
            char buf[1025] = "";
            int count = 0;
            struct stat statbuf;
            char filename[256] = "";
            struct FilePackage data;
             
            if(tpack.ack == 9)
            {
                strcat(filename,"../data/");
                strcat(filename,tpack.username);
                strcat(filename,"/");
                strcat(filename,tpack.filename);
            }
            if( stat(filename,&statbuf) == -1)
            {
               sendPack = pack('D',"","",0,8,1,"");
                return sendPack;
            }
            sendPack=pack('D', " ", filename, statbuf.st_size , 0,1,"");
            // 获取下载文件的权限
            sendPack.mode = statbuf.st_mode;

            //send(sockfd,&sendPack,sizeof(struct FilePackage),0);
           
            SSL_write(sockfd,&sendPack,sizeof(struct FilePackage));

            if((Fd = open(filename,O_RDONLY)) == -1)
            {
                perror("open error: \n");     
            }
        
            while((count = read(Fd,(void*)&buf,1024)) > 0 )
            {
               sendPack=pack('D', buf, filename, count , 2,count,""); 
                //if( send(sockfd,&sendPack,sizeof(struct FilePackage),0) == -1 )
                 
                if( SSL_write(sockfd,&sendPack,sizeof(struct FilePackage)) == -1 )
                {
                    perror("send login message:");
                }
            }
            
            sendPack=pack('D', " ", filename, statbuf.st_size , 4,1,"");
            //send(sockfd,&sendPack,sizeof(struct FilePackage),0);
            SSL_write(sockfd,&sendPack,sizeof(struct FilePackage));

            //recv(sockfd,&data,sizeof(struct FilePackage),0);
            SSL_read(sockfd,&data,sizeof(struct FilePackage));

            if( data.cmd == 'D' && data.ack == 3)
            {
               sendPack=pack('\0', "", "", 0,8,1,"");
                close(Fd);
            }
          
            // 写入日志
            char Log[256] = "";
            strcat(Log,"");
            strcat(Log,"Clien IP: ");
            strcat(Log,clientIP);

            strcat(Log,"\n");

            strcat(Log,"download Date: ");
            strcat(Log,getCurrentTime());
            
            strcat(Log,"File name: ");
            //strcat(Log,pfilename);
            strcat(Log,filename);

            strcat(Log," \n");
            
            if( (fdlog = open("../log/log.txt",O_WRONLY|O_APPEND)) == -1)
            {
                printf("\033[33mlog.txt file open error!\033[0m\n");
                exit(-1);
            }
            if(write(fdlog,Log,strlen(Log)) <0)
            {
                perror("write long.txt error:\n");
                exit(-1);
            }

            close(fdlog);

            return sendPack;
        }
        break;

    }
}

// 获取文件列表
void getlist(char *username,char *filelist)
{
    DIR *pdir = NULL;
    struct dirent *pent = NULL;
    char DEFDIR[60] = "";

    strcpy(filelist,"");
    strcat(DEFDIR,"../data/");
    strcat(DEFDIR,username);
    
    if( (pdir = opendir(DEFDIR) ) == NULL)
    {
       fprintf(stderr,"open dir error\n");
        return;
    }
    while(1)
    {
        pent = readdir(pdir); 
        if(pent == NULL)
        {
           break; 
        }
        else
        {
            strcat(filelist,pent->d_name);
            strcat(filelist,"\t");
        }
    }
    closedir(pdir);
}

// 获取的当前时间

char *getCurrentTime()
{
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    return asctime(timenow);
}

// 主线程处理函数

void mainThread(void)
{
    int reuse = 1;
    int i =0;

    int new_fd;
    int tempsockfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clie_addr;
    socklen_t clie_len;

    signal(SIGPIPE,receivePipBroken);

	/*------------------------SSL--------------------------------*/
	char pwd[100] = "";		// 用来获取当前程序所在的路径
	char *temp;

	/* SSL 库初始化 */
	SSL_library_init();
	
	/* 载入所有 SSL 算法*/
	OpenSSL_add_all_algorithms();
	
	/* 载入所有 SSL 错误消息*/
	SSL_load_erro_strings();
	
	 /* 以 SSL V2 和 V3 标准兼容方式产生一个 SSL_CTX ，即 SSL Content Text */
	  ctx = SSL_CTX_new(SSLv23_server_method());

	  /* 也可以用 SSLv2_server_method() 或 SSLv3_server_method() 单独表示 V2 或 V3标准 */
	  if (ctx == NULL)
	  {
		  ERR_print_errors_fp(stdout);
		  exit(1);
	  }

	  /* 载入用户的数字证书来发送给客户端。证书里包含公钥*/

	  if(getcwd(pwd,100) < 0 )
	  {
		  perror("getcwd error");
	  }

	  /* 载入用户私钥 */
	  if( strlen(pwd) == 1)
		  pwd[0] = '\0';

	  if( SSL_CTX_use_certificate_file(ctx,temp = strcat(pwd,"/cacert.pem"),SSL_FILETYPE_PEM) <= 0)
	  {
		  ERR_print_errors_fp(stdout);
		  exit(1);
	  }
		
	  /* 检查用户私钥是否正确 */
		if( !SSL_CTX_check_private_key(ctx))
		{
			ERR_print_errors_fp(stdout);
			exit(1);
		}

	/*------------------------SSL--------------------------------*/

    if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) <0 )
    {
        fprintf(stderr,"\033[33mSocket error:%s\033[0m\n\a",strerror(errno));
        exit(-1);
    }

    bzero(&serv_addr,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNumber);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // 设置端口复用
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

    if( bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr)) <0 )
    {
        fprintf(stderr,"\033[33mBind error:%s\033[0m\n\a",strerror(errno));
        exit(1);
    }
    if(listen(sockfd,maxClientNum) <0 )
    {
        fprintf(stderr,"\033[33mListen error:%s\033[0m\n\a",strerror(errno));
        exit(1);
    }

    // 创建线程池
    pool = threadpool_create(MAX_THR_NUM);

    //    threadpool_create(maxClientNum);

    while(1)
    {
        // 判断是否退出服务器
        if( isExit ==1  )
        {
            int i;
            printf("\033[31m\nclosing server\n\033[0m");
            for( i=0;i<60; i+= 10 )
            {
                printf("\033[31m.\033[0m");
                fflush(stdout);
                usleep(100000);
            }
            printf("\033[31m\nserver closed\n\033[0m");
            pthread_exit(0);
        }
        // 判断服务器是否正常运行
        if( isRun == 1 )
        {
            int  new_fd;
            if((new_fd = accept(sockfd,(struct sockaddr*)&clie_addr,&clie_len)) == -1 )
            {
                perror("accept error!");
                exit(-1);
            }

            printf("fd = %d 的客户端链接上服务器\n",new_fd);
            //tempsockfd = new_fd;
            // 保存当前连接客户单 IP

            memset(clientIP,0x00,sizeof(clientIP));
            strcpy(clientIP,inet_ntoa(clie_addr.sin_addr));

            printf("CurrentClientNum = %d\n",CurrentClientNum);
            // 把新的任务添加到线程池中去
            threadpool_add_task(pool,process,(void*)&new_fd); 

            printf("fd = %d 的客户端的任务正在处理中\n",new_fd);
        }

    }

    close(sockfd);
    //pthread_join(tid,NULL);
}

#ifdef __DEBUG__
// 主线程处理函数

void mainThread(void)
{
    int reuse = 1;
    int i =0;
    
    int new_fd;
    int tempsockfd;
    struct sockaddr_in serv_addr;
    struct sockaddr_in clie_addr;
    socklen_t clie_len;

    signal(SIGPIPE,receivePipBroken);
    
    if( (sockfd = socket(AF_INET,SOCK_STREAM,0)) <0 )
    {
       fprintf(stderr,"\033[33mSocket error:%s\033[0m\n\a",strerror(errno));
        exit(-1);
    }
    
    bzero(&serv_addr,sizeof(struct sockaddr_in));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portNumber);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    
    // 设置端口复用
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));
    
    if( bind(sockfd,(struct sockaddr*)&serv_addr,sizeof(struct sockaddr)) <0 )
    {
       fprintf(stderr,"\033[33mBind error:%s\033[0m\n\a",strerror(errno));
        exit(1);
    }
    if(listen(sockfd,maxClientNum) <0 )
    {
        fprintf(stderr,"\033[33mListen error:%s\033[0m\n\a",strerror(errno));
        exit(1);
    }
    
    // 创建线程池
     pool = threadpool_create(MAX_THR_NUM);
    
//    threadpool_create(maxClientNum);

    while(1)
    {
        // 判断是否退出服务器
        if( isExit ==1  )
        {
            int i;
            printf("\033[31m\nclosing server\n\033[0m");
            for( i=0;i<60; i+= 10 )
            {
                printf("\033[31m.\033[0m");
                fflush(stdout);
                usleep(100000);
            }
            printf("\033[31m\nserver closed\n\033[0m");
            pthread_exit(0);
        }
        // 判断服务器是否正常运行
        if( isRun == 1 )
        {
            int  new_fd;
            if((new_fd = accept(sockfd,(struct sockaddr*)&clie_addr,&clie_len)) == -1 )
            {
                perror("accept error!");
                exit(-1);
            }
            
            printf("fd = %d 的客户端链接上服务器\n",new_fd);
            //tempsockfd = new_fd;
            // 保存当前连接客户单 IP
            
            memset(clientIP,0x00,sizeof(clientIP));
            strcpy(clientIP,inet_ntoa(clie_addr.sin_addr));
           
            printf("CurrentClientNum = %d\n",CurrentClientNum);
            // 把新的任务添加到线程池中去
           threadpool_add_task(pool,process,(void*)&new_fd); 

            printf("fd = %d 的客户端的任务正在处理中\n",new_fd);
        }

    }
    
    close(sockfd);
    //pthread_join(tid,NULL);
}

#endif

// 信号处理函数

void receivePipBroken(int signo)
{
    if(signo == SIGPIPE)
    {
        printf("\n\033[31ma client exit!\033[0m\n\n");
        printf("\n\033[33mplease choose a command:\033[0m\n\n");

        CurrentClientNum --;
        pthread_exit(0);
    }

}


// 菜单函数

void mainMenu(void)
{
    int choice;
    int config;
    int flag = 1;

    char NewUserId[20] = ""; 
    char NewUserPwd[20] = ""; 
    int fdMax;

    // 打开配置文件，连接的最大客户端数
    if((fdMax = open("../etc/maxclientnum.txt",O_RDWR)) == -1)
    {
        printf("\033[31mmaxclientnum.txt open error!\033[0m\n");
        exit(-1);
    }

    while(1)
    {
        // 显示服务器管理登录成功后的界面
       Serv_LoginSuc();         
        scanf("%d",&choice);
        system("clear");
        
        switch(choice)
        {
            // 服务器配置
            case 1:
            {
                flag = 1;
                while( flag != 0 )
                {
                   // 显示服务器配置界面
                    Serv_Config();
                    scanf("%d",&config);
                    system("clear");

                    switch(config)
                    {
                        // 设置最大客端链接数
                        case 1:
                        {
                            int changeMax = 0;
                            
                            while( flag != 0 )
                            {
                                printf("\033[33mthe current max client num is:\033[31m%d\n",maxClientNum);                                
                                printf("\033[34minput the max num U want change:");
                                scanf("%d",&changeMax);
                                
                                if( changeMax <0 )
                                {
                                   printf("\033[34mU input the num is below 0\n"); 
                                }
                                else
                                {
                                    char wr[10] = "";
                                    
                                    flag = 0;
                                    maxClientNum = changeMax;
                                    
                                    // 将修改后的最大客户端链接数写入到缓冲区 wr 中
                                    sprintf(wr,"%d%c",maxClientNum,'\0');
                                    
                                    // 设置文件读写指针位置，从文件头部开始写数据
                                    lseek(fdMax,0,SEEK_SET);
                                    if( write(fdMax,wr,strlen(wr))<0 )
                                    {
                                        printf("write to file error!\n");
                                        exit(0);
                                    }
                                    memset(wr,0x00,10);
                                    printf("\033[34mthe new max client num is:\033[31m%d\n",maxClientNum);
                                    
                                }

                            }

                        }
                        break;
                        // 新增管理员用户
                        case 2:
                        {
                            printf("\033[34minput the new admin name:");
                            scanf("%s",NewUserId);
                            printf("\033[34minput the new admin passwd:");
                            scanf("%s",NewUserPwd);
                            
                            if( Add_admin_user(NewUserId,NewUserPwd) !=0)
                            {
                                printf("\033[31madd admin success!\n");
                            }
                            else
                            {
                                perror("  \033[31mwrite admin error\033[0m\n");
                                exit(1);
                            }
                        }
                        break;

                    // 新增用户账户
                        case 3:
                        {
                            char newUserId[20] = "";
                            char newUserPwd[20] = "";
                            char tuserDir[50] = "";
                            printf("\033[34minput the new user name:");
                            scanf("%s",newUserId);

                            printf("\033[34minput the new user passwd:");
                            scanf("%s",newUserPwd);

                            // 向数据库中增加新用户
                            int res = Add_user_user(newUserId,newUserPwd);                          
                            if(res)
                            {
                                printf("\033[31madd user success!\n");
                            }
                            else
                            {
                                perror("  \033[31mwrite user error\033[0m\n");
                                exit(-1);
                            }
                            strcat(tuserDir,"../data/");
                            strcat(tuserDir,newUserId);
                            printf("tuserDir = %s\n",tuserDir);
                            // 给新用户创建目录，命名为 newUserId
                            if(mkdir(tuserDir,0777) == -1)
                            {
                                perror("mkdir error:\n");
                            }
                        }
                        break;
                        case 4:
                            {
                                flag = 0;
                                break;
                            }
                        break;

                    }
                }
            }
            break;
        // 运行服务器
            case 2:
                {
                   isRun = 1;
                    printf("\n\033[32mserver is running now\033[0m\n\n");
                    break;
                }
            break;
        // 关闭服务器
            case 3:
                {
                    isRun = 0;
                    printf("\n\033[32mserver is stop now\033[0m\n\n");
                    break;
                }
            break;
        // 系统日志
            case 4:
                {
                    system("clear");
                    printf("\n\033[31m---------------------System  Log---------------------\033[0m\n\n");

                    FILE *fplog = NULL;
                    if( (fplog = fopen("../log/log.txt","r")) == NULL )
                    {
                        printf("the log.sys file lost!\n");
                    }

                    char logInfo[256] = "";
                    while (fgets(logInfo,256,fplog) != NULL)
                    {
                        printf("\n\033[32m%s\033[0m",logInfo);                            
                    }
                    printf("\n");
                    printf("\n\033[31m-----------------------Log End-----------------------\033[0m\n\n");
                }
            break;
            case 5:
                {
                    // 销毁线程池
                    threadpool_destroy(pool); 
                   isExit = 1;
                    exit(1);
                }
                break;
        }
    }

    close(fdMax);
}

// 初始化客户端最大连接数
int InitMaxClientNum()
{
   int fd;
    char buf[10] = "";
    
    int nBytes = 0;
    if( (fd = open("../etc/maxclientnum.txt",O_RDONLY)) <0 )
    {
        printf("can not open maxclientnum.txt\n");
        return -1;
    }

    if((nBytes = read(fd,buf,10))<0 )
    {
       printf("can not read fro maxclientnum.txt!\n"); 
        return -1;
    }
    close(fd);

    return atoi(buf);
}

/*打包函数*/

struct FilePackage pack(char tCmd, char* tBuf, char* tFilename, int tFilesize, int tAck,int count,char *uname)
{
	struct FilePackage tPackage;
	tPackage.cmd = tCmd;
	memcpy(tPackage.buf,tBuf,count);
	strcpy(tPackage.filename, tFilename);
	strcpy(tPackage.username, uname);
	tPackage.filesize = tFilesize;
	tPackage.ack = tAck; 
	return tPackage;
}

