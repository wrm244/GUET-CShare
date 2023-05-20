#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

//定义数据块大小
char data[20000];
//定义服务器端口和服务器地址
int PORT=1234;
char* SERVER_IP="127.0.0.1";

int main(int argc, char *argv[])
{
	int sockfd;
	//由用户传入的要下载文件的名称
	char *downLoadFileName;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		return -1;
	}
	if(argc!=2 && argc!=4) {printf("用法：uploadFileClient [IP 端口] 文件名\n"); return -1;}
	if(argc==4) {
		SERVER_IP = argv[1];
		PORT = atoi(argv[2]);
		/********** BEGIN **********/
		downLoadFileName=argv[3];		
		/********** END ***********/
		}
	else {downLoadFileName = argv[1];};
	struct sockaddr_in servAddr;
	int servAddrLen = sizeof(servAddr);
	bzero(&servAddr, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);
	servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	
	//先在本地创建要下载的文件
	int fd  = open(downLoadFileName, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
	//向服务器发送要上传文件的名称
	/********** BEGIN **********/
	sendto(sockfd, downLoadFileName, strlen(downLoadFileName)+1, 0, (struct sockaddr*)&servAddr, servAddrLen);
	/********** END ***********/
	int recvLen;
	//接收来自服务器发来的数据
	while((recvLen = recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&servAddr, &servAddrLen)) > 0)
	{
	  if(data[0] == 'c')  //data[1]起放的是文件数据
    	  {
           write(fd, &(data[1]), recvLen - 1);
            //给服务器回复一个接收确认的标识OK
           sendto(sockfd, "OK", strlen("OK"), 0, (struct sockaddr *)&servAddr, servAddrLen);
            //清空data数据，用于下一次接收新数据
           memset(data, 0, sizeof(data));
    	  }
    	  else
    	  {   
	      if(data[0]=='n') printf("文件不存在！\n");
             if(data[0]=='e') printf("文件下载完毕！\n");
             close(fd);  //关闭文件
             break;
    	   }
	}	
	close(sockfd);	
	return 0;
}

