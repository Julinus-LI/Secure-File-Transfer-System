/*************************************************************************
	> File Name: client.c
	> Author:    arrayli
	> Mail:      1374367549@qq.com
	> Created Time: 2018年04月30日 星期一 11时51分14秒
 ************************************************************************/

#include"client.h"


int main(int argc,char *argv[])
{
    int sockfd;
    char username[20] = "";
   char userpwd[20] ="";
   if(argc<2)
   {
        printf("./client IP\n");
        exit(0);
   }

   // 客户单和服务器进行连接
   //if((sockfd = connectto(argc,argv) <0 ))
   if((sockfd = connectto(argc,argv)) <0 )
   {
      perror("connectto error"); 
      exit(0);
   }
    printf("sockfd = %d\n",sockfd);    
   // 用户登录
  if( login(sockfd) <0 )
  {
     printf(" client login falied!\n"); 
     exit(-1);
  }

  // 登录成功后，客户端显示的界面
    Log();
    
    // 调用处理函数
    mainHandler(sockfd);

    return 0;
}


// 客户单连接服务器函数
int connectto(int argc,char *argv[])
{
    //--------------------------------------判断输入的 IP 是否正确 --------------------------
    int i,count = 0;
    if( argc != 2 )
    {
        printf("format error: you mast enter ipaddr like this : client 192.168.0.6\n");
        return -1;
    }
    for( i=0;*(argv[1]+i) != '\0'; i++ )
    {
        if(*(argv[1]+i) == '.')
            count++;
    }
    
    if(count != 3)
    {
        printf("IP format error\n");
        return -1;
    }
    
    strcpy(serv_ip,argv[1]);

    //---------------------------------------- 服务端和客户端进行 TCP 连接 -----------------------------
    int socketfd;
    if((socketfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
    {
        perror("socket error");
        exit(-1);
    }
    struct sockaddr_in clie_addr;   
    
    bzero(&clie_addr,sizeof(struct sockaddr_in));
    clie_addr.sin_family = AF_INET;
    clie_addr.sin_port = htons(3333);
    inet_pton(AF_INET,argv[1],&clie_addr.sin_addr.s_addr);
    
    if(connect(socketfd,(struct sockaddr*)&clie_addr,sizeof(struct sockaddr_in)) != 0)
    {
        perror("connect error");
        exit(-1);
    }
   
    // 返回通信套接字描述符
    return socketfd;
}

//  用户登录
//int login(int sockfd,char username[20],char userpwd[20])
int login(int sockfd)
{
    char username[20] = "";
   char userpwd[20] ="";
    
top:
   printf("ID: ");
   scanf("%s",username);
    strcpy(tempusername,username);
    printf("PASSWD: "); 
   scanf("%s",userpwd);
    strcat(username,"*");
    strcat(username,userpwd);
    strcat(username,"#");
   
    struct FilePackage data;
   
    data=pack('L', username, " ",  0, 9,strlen(username),tempusername);

    if(senddata(sockfd,data) == 0)
    {
        printf("senddata error!\n");
        return -1;
    }

    //memset(&data,0x00,sizeof(data));
    if(recvdata(sockfd,&data) == 0)
    {
        printf("recvdata error!\n");
        return -1;
    }

    if(data.cmd == 'L' && data.ack == 0)
    {
        printf("\n\033[33mUsername or Password error!\033[0m\n\n");

        // 所有变量全部清空
        memset(username,0x00,strlen(username));
        memset(userpwd,0x00,strlen(userpwd));
        memset(tempusername,0x00,strlen(tempusername));

        goto top;
    }
    if(data.cmd == 'L' && data.ack == 1)
    {
        printf("Login Success\n");
        printf("%s\n",data.buf);
        return 0;
    }
    
    if(data.cmd == 'L' && data.ack == 2)
    {
        printf("\n\033[32mMaxnum connection!\033[0m\n\n");
        exit(0);
    }

    return 0;
}


// 发送数据包
int senddata(int sockfd,struct FilePackage data)
{
    if( (send(sockfd,&data,sizeof(struct FilePackage),0)) == -1) 
    {
        perror("send login message");
        return 0;
    }
    return 1;
}

// 接收数据包
int recvdata(int sockfd,struct FilePackage *data)
{
  if( (recv(sockfd,data,sizeof(struct FilePackage),0)) == -1) 
    {
        perror("recv login message");
        return 0;
    }

    return 1;
}

// 逻辑处理
void mainHandler(int sockfd)
{
    char temp[100];
    char command[2];
    char Files[100];
    char Files1[100];
    pthread_t threadId;
    strcpy(temp,"ls");

    while(1)
    {
        int count;
        int temp1;
        count = 0;
        temp1 = 0;
        
        if(strncmp(temp,"\n",1) != 0)
            Show(sockfd,temp);
        else
            goto top;
        Clie_Func();
        printf("  Please input the Client command:");
top:        fgets(temp,sizeof(temp),stdin);
       
        printf("temp = %s\n",temp);
        switch(strncpy(command,temp,1),*command)
        {
            case '1':
            {
                printf("\033[33mUpdate Files:\033[0m ");
                fgets(Files,sizeof(Files),stdin);
                Files[strlen(Files)-1]='\0';
                while(Files[count]!='\0' && Files[count]!='\n') 
                {
                   if( Files[count] == ' ' )
                    {
                        Files[count]='\0';
                        pthread_create(&threadId,NULL,UpdateF,(void*)&Files[temp1]);
                        temp1 = count+1;
                    }
                    count++;
                }
                pthread_create(&threadId,NULL,UpdateF,(void*)&Files[temp1]);
            }
            strcpy(temp,"ls");
                break;

            case '2':
            {
                printf("\033[33mDownloadF Files:\033[0m ");
                
                fgets(Files1,sizeof(Files1),stdin);
                Files1[strlen(Files1)-1]='\0';
                while(Files1[count]!='\0' && Files1[count]!='\n') 
                {
                   if( Files1[count] == ' ' )
                    {
                        Files1[count]='\0';
                        pthread_create(&threadId,NULL,DownloadF,(void*)&Files1[temp1]);
                        temp1 = count+1;
                    }
                    count++;
                }
                pthread_create(&threadId,NULL,DownloadF,(void*)&Files1[temp1]);

            }
                break;
            case '3':
            {
                system("clear");    
                exit(0);
            }
                break;
        }

    }
}

// 显示客户单端目录

void Show(int sockfd,char temp[100])
{
    printf("temp = %s\n",temp);
    char command [2];
    if((strncpy(command,temp,1),*command)=='1'||(strncpy(command,temp,1),
                *command)=='2'||(strncpy(command,temp,1),*command)=='3')
        return;
    if(strncmp(temp,"cd",2)==0)
    {
        char dir[40]={'\0'};
        temp[strlen(temp)-1]='\0';
        strncpy(dir,(&(*temp)+3),strlen(&(*temp)+3));

        chdir(dir);
        strcpy(temp,"ls");
    }

    system("clear");
    printf("\n\033[34m-----------------------------   \033[31mClient Files List   \033[34m----------------------------\033[0m\n");
    system(temp);

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~显示服务器目录~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    printf("tempusername = %s\n",tempusername);
    if(senddata(sockfd,pack('S', " ", " ",  0, 9,1,tempusername)) != 1)
    {
        printf("Show error\n");
        return;
    }

    struct FilePackage data;
    memset(&data,0x00,sizeof(data));
    if(recvdata(sockfd,&data)==0)
        exit(0);

    printf("File Listdata.cmd = %c,data.ack = %d\n",data.cmd,data.ack); 
    if(data.cmd=='S')
    {
        printf("\033[34m-----------------------------   \033[31mServer Files List   \033[34m----------------------------\033[0m\n");
        printf("%s\n",data.buf);
        printf("\033[34m--------------------------------------------------------------------------------\033[0m\n");
    }
}

// 上传文件
void *UpdateF(void *filename)
{
        usleep(500);
    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    char *Files=(char *)filename;

    int  sockclient;
  
    struct sockaddr_in sockaddr1;
    
    if((sockclient=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");	
        exit(0);
    }

    memset(&sockaddr1,0,sizeof(sockaddr1));
	sockaddr1.sin_family = AF_INET;
	sockaddr1.sin_addr.s_addr = inet_addr(serv_ip);
	sockaddr1.sin_port = htons(3333);
	
	if(connect(sockclient,(struct sockaddr* )&sockaddr1,sizeof(sockaddr1))==-1)
		{
			perror("connect");
			exit(0);
		}

	int Fd;
	char buf[1025]={'\0'};
	int count=0;
	int temp=0;
	struct stat statbuf;
	struct FilePackage data;
	
	if(stat(Files,&statbuf)==-1)
		{
			perror("*");
			return 0;
		}
	if(!S_ISREG(statbuf.st_mode))
		perror("*");
		
		data=pack('U', " ", Files, statbuf.st_size , 9,0,tempusername);
        data.mode =  statbuf.st_mode;

    if((send(sockclient,&data,sizeof(struct FilePackage),0))==-1)
			perror("send login message:");

	if((recv(sockclient,&data,sizeof(struct FilePackage),0))==-1)
	{	
		perror("recv login message:");
	}
	
	if(data.cmd == 'U' && data.ack == 1)
		{
			printf("服务器磁盘不足\n");
			return 0;
		}

	if(data.cmd == 'U' && data.ack == 0)
		{
			//do noting; 
		}

	if((Fd=open(Files,O_RDONLY))==-1)
		perror("open: ");
	while((count=read(Fd,(void *)buf,1024))>0)
		{
			data=pack('U', buf, Files, count , 2,count,tempusername);
			if((send(sockclient,&data,sizeof(struct FilePackage),0))==-1)
				perror("send login message:");
		}
		

	data=pack('U', " ", Files, statbuf.st_size , 4,1,tempusername);
	if((send(sockclient,&data,sizeof(struct FilePackage),0))==-1)
			perror("send login message:");

	if((recv(sockclient,&data,sizeof(struct FilePackage),0))==-1)
	{	
		perror("recv login message:");
	}
	if(data.cmd == 'U' && data.ack == 3)
		{
			printf("\n\033[31mUpdate Files over\033[0m\n");
		}
	
	data=pack('Q', " ", " ", 0 , 9,0,tempusername);
	if((send(sockclient,&data,sizeof(struct FilePackage),0))==-1)
			perror("send login message:");	
	close(Fd);
	close(sockclient);
	return (void *)1;

}


// 下载文件

void* DownloadF(void *filename)
{
    char *Files=(char *)filename;
    int  sockclient;
    struct sockaddr_in sockaddr1;

    usleep(500);


    if((sockclient=socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");	
        exit(0);
    }
    memset(&sockaddr1,0,sizeof(sockaddr1));
    sockaddr1.sin_family = AF_INET;
    sockaddr1.sin_addr.s_addr = inet_addr(serv_ip);
    sockaddr1.sin_port = htons(3333);

    if(connect(sockclient,(struct sockaddr* )&sockaddr1,sizeof(sockaddr1))==-1)
    {
        perror("connect");
        exit(0);
    }
    
    int Fd;
    char buf[1024];
    int count=0;
    int temp=0;
    struct statfs statfsbuf;
    struct FilePackage data;
    struct stat statbuf;
    if(stat(Files,&statbuf)==0)
    {
        printf("文件重名\n");
        return 0;
    }

    data=pack('D', " ", Files, 0 , 9,1,tempusername);
    if((send(sockclient,&data,sizeof(struct FilePackage),0))==-1)
    {
        perror("send login message");
        exit(0);
    }
    if((recv(sockclient,&data,sizeof(struct FilePackage),0))==-1)
    {	
        perror("recv login message");
        exit(0);
    }
    if(data.cmd == 'D' && data.ack == 0)
    {
        statfs("./",&statfsbuf);
        if((statfsbuf.f_bsize*statfsbuf.f_bfree)<=data.filesize)
        {
            printf("\033[31m磁盘空间不足\033[0m\n");
            return 0;
        }
    }
    if(data.cmd == 'D' && data.ack == 8)
    {
        printf("\033[33mNo such file or directory\033[0m\n");
        return 0;
    }

    // 获取下载文件的文件权限 
    mode_t mode = data.mode;
    if((Fd=open(Files,O_RDWR|O_CREAT,mode))==-1)
        perror("open ");
    if(recv(sockclient,&data,sizeof(struct FilePackage),0)==-1)
    {
        perror("read error:\n");
    }
    printf("DownloadF: recv: data.cmd= %c,data.ack = %d\n",data.cmd,data.ack);  

    while(data.ack==2)
    {
        count=data.filesize;
        printf("DownloadF: data.buf = %s,data.filesize = %d\n",data.buf,data.filesize);
        if(write(Fd,data.buf,count)==-1)
        {
            perror("wirte error:\n");	
        }
        if(recv(sockclient,&data,sizeof(struct FilePackage),0)==-1)
        {
            perror("read error:\n");
        }
    }
    if(data.ack==4)
    {	
        printf("\033[31mDownload Files over\033[0m\n");
        data=pack('D', " ", Files, 0 , 3,1,tempusername);
       if((send(sockclient,&data,sizeof(struct FilePackage),0))==-1)
        perror("send login message:");	

        data=pack('Q', " ", " ", 0 , 9,0,tempusername);
        if((send(sockclient,&data,sizeof(struct FilePackage),0))==-1)
        perror("send login message:");	
        close(sockclient);
        close(Fd);
    }

    return (void *)1;


}


//~~~~~~~~~~~~~~~~~~~~~~~~打包数据~~~~~~~~~~~~~~~~~~~~~~~~~
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

