/*************************************************************************
	> File Name: http.cpp
	> Author: 
	> Mail: 
	> Created Time: 2018年06月11日 星期一 16时19分35秒
 ************************************************************************/

#include"http.h"
#include<unistd.h>

/* 创建一个 socket 监听套接字 */
int http_start_up(const char*ip,int port)
{
    /* 创建 listen_sock 监听套接字 */
    int listen_sock = socket(AF_INET,SOCK_STREAM,0);
    if(listen_sock < 0)
    {
        perror("socket():");
        exit(ERROR_SOCKET);
    }
  
    /* 端口复用 */
    int opt = 1;
    setsockopt(listen_sock,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
    
    struct sockaddr_in server;
    bzero(&server,sizeof(server));

    server.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&server.sin_addr.s_addr);
    server.sin_port = htons(port);
    
    /* 邦定地址结构  */
    if(bind(listen_sock,(struct sockaddr*)&server,sizeof(server)) < 0)
    {
       perror("bind():"); 
        exit(ERROR_BIND);
    }
    
    /* 监听 */
    if(listen(listen_sock,LISTEN_NUM) <0 )
    {
       perror("listen():"); 
        exit(ERROR_LISTEN);
    }
   
    /* 返回监听 socket 文件描述符 */
    return listen_sock;
}

/* 处理客户端发来的请求 */
void* http_handler_request(void *newfd)
{
//    printf("http_handler_request exec ok\n");
    int sockfd = *(int*)newfd;
    char read_buf[BUFSIZE];     /* 读数据缓冲区 */
    int read_length = 0;        /* 读取数据的长度 */
    int cgi = 0;                /* 执行 cgi 程序标志 */
    int state_code = 0;         /* 状态码 */
    int idx_url = 0;            /* 读取 url 的下标 */
    char* query_string = NULL;  /* 记录 GET请求的参数 */
    /* 获取一行 Http 请求报文 */
    read_length = http_getline(sockfd,read_buf,sizeof(read_buf));
    printf("release\n");
    printf("read_buf = [%s]\n",read_buf);
    /* save http method */
    char method_buf[BUFSIZE];
    /* save http url */
    char url_buf[BUFSIZE];
    /* save http path */
    char path_buf[BUFSIZE];

    /* 获取方法字段 */
    int idx_buf = 0;
    int idx_method = 0;
   
    /* 获取 HTTP 请求报文中的 GET/POST 请求方法到 method  */
    while( !isspace(read_buf[idx_buf]) && idx_buf < sizeof(read_buf) && idx_method < sizeof(method_buf) )
    {
        method_buf[idx_method] = read_buf[idx_buf];
        idx_buf++;
        idx_method++;
    }
    
    method_buf[idx_buf] = '\0';
    printf("method_buf = [%s]\n",method_buf);
    /* only support GET and POST method */
    /* strcasecmp() 函数和 strcmp() 函授的区别： strcasecmp() 函数不区分大小写，而 strcmp() 函数区分大小写 */  
   
    /* 如果获取到的方法非 GET/POST 的话，那么程序出错 */
    if(strcasecmp(method_buf,"GET") && strcasecmp(method_buf,"POST"))
    {
        /* 表示出了 GET/POST 方法外，服务器支持其他方法 ,返回 状态码 501 */
       state_code = 501;
        goto end;
    }
    
    /* 获取到的方法为 GET/POST 方法后，跳过空格 */
    while( isspace(read_buf[idx_buf]) && idx_buf < sizeof(read_buf))
    {
        idx_buf++;
    }
   
    /* 获取 url 到 url_buf中 */
    while( !isspace(read_buf[idx_buf]) && idx_buf < sizeof(read_buf)  && idx_url < sizeof(url_buf))
    {
        url_buf[idx_url] = read_buf[idx_buf];
        idx_url++;
        idx_buf++;
    }
    
    url_buf[idx_url] = '\0';
    printf("METHOD : %s\n",method_buf); 
    printf("URL : %s\n",url_buf);
    
    /* 如果是 POST 将 cgi 置为 1 */
    if(strcasecmp(method_buf,"POST") == 0)
    {
       cgi = 1; 
    }
    
    query_string = url_buf;
    /* 如果是 GET 且没有参数，需要将 cgi  置 为 1 */
    /* 将路径和参数 分开 */
    if(strcasecmp(method_buf,"GET") == 0)
    {
       while(*query_string != '\0' && *query_string != '?')
            query_string++;
        if(*query_string == '?')
        {
            cgi = 1;
            *query_string = '\0';
            query_string++;
        }
    }
/*    
    if(strcasecmp(url_buf,"/favicon.ico") == 0)
    {
        memset(url_buf,0x00,sizeof(url_buf));
        strcpy(url_buf,"/");
    } 
*/
    printf("-----------------\n");
    printf("url_buf : %s\n",url_buf);
    printf("-----------------\n");
    printf("query_string : %s\n",query_string);
    printf("-----------------\n");
   
    /* 判断是否为访问路径,是的话重定向到首页 */
    sprintf(path_buf,"wwwroot%s",url_buf); 
    if(path_buf[strlen(path_buf) - 1]  == '/')
    {
        // 使用 stract 有 Bug
        // 使用sprintf
       sprintf(path_buf,"wwwroot/index.html"); 
        printf("path_buf = [%s]\n",path_buf);
    }
    
    /* 检查 path_buf 或者 index.html  文件是否存在*/
    struct stat file_stat;
    if(stat(path_buf,&file_stat) < 0)
    {
        /* 资源不存在 */
       state_code = 404; 
        goto end;
    }
    /* 若文件存在 */
    else
    {
        /* 如果具有可执行权限，则将 cgi 置 为 1 */
        if(file_stat.st_mode & S_IXUSR || file_stat.st_mode & S_IXGRP || file_stat.st_mode & S_IXOTH)
            cgi  = 1;
        /* 可执行就去 fork-exec 执行 cgi 脚本,此时无需获取清空头部，因为需要获取长度 */
        if(cgi)
        {
            state_code = http_execute_cgi(sockfd,method_buf,path_buf,query_string);    
            if(state_code == 200)
            {
                printf("execute cgi ok...\n");
            }
        }
        /* 如果发送的是静态页面，需要清空头部 */
        else
        {
#ifdef __DEBUG__
            printf("send html file\n");
            http_clear_head(sockfd);
            printf("11111111111start\n");
            state_code = http_send_html(sockfd,path_buf,file_stat.st_size);
            printf("11111111111end\n");
            if(state_code == 200)
            {
               printf("send html ok...\n"); 
            }
            printf("222222222222\n");
#else
            http_clear_head(sockfd);
            state_code = http_send_html(sockfd,path_buf,file_stat.st_size);
            if(state_code == 200)
            {
               printf("send html ok...\n"); 
            }
#endif
        }

    }

end:
    /* echo_error no handler */
    /* 如果 http 响应出错的话，则清空剩余 HTTP 头部请求信息 */
    if( state_code != 200 )
        http_clear_head(sockfd);
    http_echo_error(sockfd,state_code);
    close(sockfd);
    return NULL;
}

/* 从 socket 中读取一行数据 */
int http_getline(int sockfd,char*buf,int length)
{
    char read_char = '\0';  /* 读到的字符 */
    int read_idx = 0;      /* 读到的数据的位置 */
    
    /* 这里数据，采取一个字节一个字节的读取；应为要判断是否到行末，即 最后读到的字符是 \r 或者 \r\n 为行结束符 */
    /* 只是把 \r\n 之前的内容读到 buf 中，然后在 buf 的末尾加上 \n\0 */
    
    //printf("buf_length = %d\n",length);
    while( read_char != '\n'  && read_idx < length - 1)
    {
       ssize_t s = recv(sockfd,&read_char,1,0); 
    //    printf("c = %c\n",read_char);
        /* 如果从 socket 中读取到数据 */
        if( s > 0 )
        {
            /* 判断是否读取到 \r 字符 */
            if( read_char == '\r' )
            {
                /* 使用 MSG_PEEK 标志使下一次读取依然可以得到这次读取的内容，可以认为是接收窗口不滑动 */
                s = recv(sockfd,&read_char,1,MSG_PEEK);

                /* 读取到换行符 \n */
                if(s > 0 && read_char == '\n')
                {
                   recv(sockfd,&read_char,1,0);  /* 读取最后一个换行符 \n,因为之前读取的 \n 相当于没有读取 */
                }
                else
                    read_char = '\n';
            }
            /* 没有读取到 \r，则把读取的内容放到 buf 中 */
            buf[read_idx++] = read_char;
        }
    }
   
    /* 最后把字符串尾部加上 '\0' */
    buf[read_idx] = '\0';
    /* 返回读到的字节数 */
    return read_idx;

}

/* 返回错误状态码 */
int http_echo_error(int sockfd,int state)
{
   switch(state)
    {
        case 404:
        http_echo_404(sockfd);
        break;
        case 403:
        http_echo_403(sockfd);
        break;
        case 501:
        http_echo_501(sockfd);
        default:
        break;
    }
    return 0;
}

int http_echo_501(int sockfd)
{
    /* HTTP 报文首部第一部分：状态行：由 HTTP 协议版本号，状态码，状态消息三部分组成 */
    const char* stat = "HTTP/1.0 404 NOT FOUND \r\n";
    send(sockfd,stat,strlen(stat),0);
    /* HTTP 报文首部第二部分：消息报头：用来说明客户使用的报文主体使用的对象类型 */ 
    //const char* type = "Content-Type: /text/html\r\n";
    const char* type = "Content-Type: text/html\r\n";
    send(sockfd,type,strlen(type),0);
    /* HTTP 报文首部第三部分：空行 */
    const char* blank = "\r\n";
    send(sockfd,blank,strlen(blank),0);
    /* HTTP 报文首部第四部分：响应正文，服务端返回给客户端的文本信息 */
    const char* html = "<html><title 404 Not Found></title><h1 404 ERRON><h1></html>";
    send(sockfd,html,strlen(html),0);
    return 0;
}

int http_echo_403(int sockfd)
{
    /* HTTP 报文首部第一部分：状态行：由 HTTP 协议版本号，状态码，状态消息三部分组成 */
    const char* stat = "HTTP/1.0 404 Forbidden\r\n";
    send(sockfd,stat,strlen(stat),0);
    /* HTTP 报文首部第二部分：消息报头：用来说明客户使用的报文主体使用的对象类型 */ 
    //const char* type = "Content-Type: /text/html\r\n";
    const char* type = "Content-Type: text/html\r\n";
    send(sockfd,type,strlen(type),0);
    /* HTTP 报文首部第三部分：空行 */
    const char* blank = "\r\n";
    send(sockfd,blank,strlen(blank),0);
    /* HTTP 报文首部第四部分：响应正文，服务端返回给客户端的文本信息 */
    const char* html = "<html><title 403 forbidden></title><h1 403 forbidden><h1></html>";
    send(sockfd,html,strlen(html),0);
}

int http_echo_404(int sockfd)
{
    /* HTTP 报文首部第一部分：状态行：由 HTTP 协议版本号，状态码，状态消息三部分组成 */
    const char* stat = "HTTP/1.0 501 Unavailable\r\n";
    send(sockfd,stat,strlen(stat),0);
    /* HTTP 报文首部第二部分：消息报头：用来说明客户使用的报文主体使用的对象类型 */ 
    //const char* type = "Content-type: /text/html";
    const char* type = "Content-Type: text/html\r\n";
    send(sockfd,type,strlen(type),0);
    /* HTTP 报文首部第三部分：空行 */
    const char* blank = "\r\n";
    send(sockfd,blank,strlen(blank),0);
    /* HTTP 报文首部第四部分：响应正文，服务端返回给客户端的文本信息 */
    const char* html = "<html><title 501 Unavailable></title><h1 501 Unavailable><h1></html>";
    send(sockfd,html,strlen(html),0);
}

/* 读取并丢弃剩余头部信息 */
int http_clear_head(int sockfd)
{
    char buf[BUFSIZE];
    int i = 0;
    do
    {
        http_getline(sockfd,buf,sizeof(buf));
    }while(strcmp("\n",buf));
    return 0;
}

/* 给客户端发送静态网页 */
int http_send_html(int sockfd,const char* path,int file_size)
{
    /* HTTP 报文首部第一部分：状态行：由 HTTP 协议版本号，状态码，状态消息三部分组成 */
    const char* stat = "HTTP/1.0 200 OK\r\n";
    send(sockfd,stat,strlen(stat),0);
    /* HTTP 报文首部第二部分：消息报头：用来说明客户使用的报文主体使用的对象类型 */ 
    
    /*
     * const char* type = "Content-type: /text/html";
     * 注意这里的大小写是 Content-Type :/text/html 而不是 Content-type: /text/html
     */
    //const char* type = "Content-Type: /text/html\r\n";
    // 这里没有 / 在 text 前 ,即 text/html\r\n*/
    const char* type = "Content-Type: text/html\r\n";
    send(sockfd,type,strlen(type),0);
    /* HTTP 报文首部第三部分：空行 */
    const char* blank = "\r\n";
    send(sockfd,blank,strlen(blank),0);
    printf("html file path = [%s]\n",path);

    /* 打开文件并读文件 */
    int html_fd = open(path,O_RDONLY);
    if(html_fd <0 )
    {
       perror("open():"); 
        return 404;
    }
    
    /* 给客户端发送html 的内容*/
    /* sendfile():                                                                                                        **函数实现了零拷贝，在两个文件描述之间直接传数据（完全在内核操作），从而避免了内核缓冲区和用户缓冲区之间的数据拷贝
    * 效率很高，被称为零拷贝
    */

    if(sendfile(sockfd,html_fd,0,file_size) < 0)
    {
       perror("snedfile():"); 
        close(html_fd);
        return 404;
    }
    close(html_fd);
    return 200;
}

/* 执行 cgi 程序 */
int http_execute_cgi(int sockfd,const char*method,const char*path,const char* query_string)
{
    int content_len = -1;
    /* 如果是 GET 方法的话，丢弃头部的其余信息 */
    if(strcasecmp(method,"GET") == 0)
    {
        http_clear_head(sockfd);        
    }
    /* 如果是 POST 方法,读取正文 */
    else
    {
        char buff[BUFSIZE];
        int ret = -1;
        do
        {
            ret = http_getline(sockfd,buff,sizeof(buff));            
            /* 这里 Content-Length (注意空格) 有16个字节 */
            if(strncasecmp(buff,"Content-Length: ",16) == 0)
            {
               content_len = atoi(&buff[16]); 
            }
        }while(strcmp(buff,"\n"));
        /* content_len == -1 说明，这里没有正文，不应该调用 cgi ，返回错误信息 */
        if(content_len == -1)
        {
            return 403; 
        }
        printf("Get content length success!\n");
    }

    /* 当走到这一步，说明一定为 cgi 调用 */
    printf("cgi-path : %s\n",path);
    
    /* 这里前端把数据传送给后端，由 cgi fork 子进程取处理数据，右父进程与客户端实现数据交互 
     * 但是 给 cgi 的数据怎么给子进程，这里就用到进程通信，由于是父子进程，所有这里使用管道
     * 就比较方便
     */
    
    int cgi_input[2];  // father  ------>> child
    int cgi_output[2]; // child   ------>> father
    if(pipe(cgi_input) < 0)
    {
       perror("pipe():"); 
        return 403;
    }
    if(pipe(cgi_output) < 0)
    {
       perror("pipe():"); 
        return 403;
    }
    
    pid_t pid = fork();
    if(pid < 0)
    {
        perror("fork():");
        return 403;
    }
    else if(pid == 0)  /* 子进程执行 cgi 程序，并把执行结果通过 管道返回给父进程，父进程在返回给客户端 */
    {
        /* 子进程关闭 cgi_input 管道的写入端，从 cgi_input 的读入端读取父进程传给子进程的数据,把执行结果写入到 ouput 的写入端*/

        close(cgi_input[1]);
        close(cgi_output[0]);
    
        char METHOD[BUFSIZ];
        char CONTENT_LENGTH[BUFSIZE];
        char QUERY_STRING[BUFSIZE];
    
        /* set method */
        sprintf(METHOD,"METHOD=%s",method);
        putenv(METHOD);
        /* judge GET or POST */
        /* 如果是 GET 方法，数据被存储到 QUERY_STRING 的环境变量中 */
        if(strcasecmp(method,"GET") == 0)
        {
           sprintf(QUERY_STRING,"QUERY_STRING=%s",query_string); 
            putenv(QUERY_STRING);
        }
        // POST
        else
        {
           /* 如果是 POST 方法，传正文的时候并不知道什么时候到底传完，所以这里有一个环境变量 CONTENT_LENGTH,记录传输过来有多少个字符的长度  */
            sprintf(CONTENT_LENGTH,"CONTENT_LENGTH=%d",content_len);
            putenv(CONTENT_LENGTH);
        }
        /* 这里使用重定向，将输入输出都变为以客户端的方式读写 */
        dup2(cgi_output[1],STDOUT_FILENO);
        dup2(cgi_input[0],STDIN_FILENO);

        /* 程序替换，替换为 path 路径下的程序 */
        execl(path,path,NULL);
        exit(1);
    }
    /* 父进程从管道里取数据，在向客户端发送数据 */
    else
    {
        close(cgi_input[0]); 
        close(cgi_output[1]);
        
        int i = 0;
        char c = '\0';
        int idx = 0;
        /* 父进程从 socket 中读取数据,并且写入到 cgi_input 中 */
        for( ; idx<content_len; idx++ )
        {
            recv(sockfd,&c,1,0);
            write(cgi_input[1],&c,1);
        }
        
        /* 父进程从 cgi_output 中读取子进程的执行结果，发送给客户端 */
        while(1)
        {
           ssize_t s = read(cgi_output[0],&c,1); 
           if(s > 0)
            {
                send(sockfd,&c,1,0);
            }
            else
                break;
        }
        /* 父进程等待子进程退出 */
        waitpid(pid,NULL,0);
        close(cgi_input[0]);
        close(cgi_output[1]);
        return 200;
    }
}
