#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
/************************
 * port: 需要绑定的端口号
 * 返回值: 调用成功返回0，否则返回-1
*************************/
int UDPSocket(unsigned short port)
   {
	int ret = -1;
	int sockfd;
                /********** BEGIN **********/
                sockfd=socket(AF_INET,SOCK_DGRAM,0);
                /********** END **********/
                if(sockfd == -1)
                {
                     printf("创建UDP套接字失败:%s\n", strerror(errno));
                     return ret;
                 }
                /********** BEGIN **********/
                struct sockaddr_in addr;
                /********** END **********/
                bzero(&addr, sizeof(addr));
                addr.sin_family = AF_INET;
                /********** BEGIN **********/
                addr.sin_port = htons(port);
                /********** END **********/
                addr.sin_addr.s_addr = htonl(INADDR_ANY);
                ret = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));		
	return ret;
   }

int main()
  {
	int sockfd;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	//调用UDPSocket函数绑定6667端口
	int ret1 = UDPSocket(6667);
	//继续绑定6667端口，看是否能够绑定成功，从而判断UDPSocket函数是否实现成功
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(6667);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	int ret2 = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
	
	int logFd;
	logFd = open("log", O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	if(ret2 == -1)
	{
	      if(ret1 == 0) write(logFd, "0", 1);
	      else write(logFd, "1", 1);
	}
	else
	{
	     write(logFd, "2", 1);
	}
	sleep(1);
	close(sockfd);
	close(logFd);
	return 0;
  }