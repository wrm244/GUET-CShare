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
#include <pthread.h>

//定义数据块大小
char data[20000];
//定义服务器端口
int PORT=1234;

int download()
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
	{
		printf("创建UDP套接字失败: %s\n", strerror(errno));
		return  -1;
	}
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));    //清空，相当于memset(&addr,0,sizeof(addr))
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//绑定本地UDP端口
	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		printf("绑定端口失败: %s\n", strerror(errno));
		return  -1;
	}
	//存放客户端主机信息
	struct sockaddr_in clientAddr;
	int clientAddrLen = sizeof(clientAddr);
	int fd;
	char filePath[100];
	char ack[5];
	//
	//int len=sizeof(addr);
	//getsockname(sockfd, (struct sockaddr *)&addr, &len);  //取出套接字地址，内含本地端通信用的IP、端口
	//提示当前的状态
	printf("本端【%s,%d】可供下载的文件：s3.c\n正在等待下载请求。。。\n",inet_ntoa(addr.sin_addr),PORT);
	//接收要下载的文件名称
	recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
	//数据块是下载文件的名称，在本地打开该文件
	sprintf(filePath, "%s", data);
	printf("客户端【%s,%d】下载文件:%s\n",inet_ntoa(clientAddr.sin_addr),ntohs(clientAddr.sin_port),filePath);
	fd = open(filePath, O_RDONLY);
	//文件不存在时的处理
	if(fd==-1)
	{
	 data[0] = 'n';
	 sendto(sockfd, data, 1, 0, (struct sockaddr *)&clientAddr, clientAddrLen); 
	 printf("文件不存在!\n");
	 close(fd);
	 close(sockfd);   //关闭服务器套接字
	 return -1;    
         }
	//设置数据块类型为文件内容
	data[0] = 'c';
	int readSize = 0;
	//读取要下载文件的数据，每次尝试读2000字节，最后一次不能保证
	while((readSize = read(fd, &(data[1]), 2000)) != 0)
	{
		//向客户端发送数据
		sendto(sockfd, data, readSize+1, 0, (struct sockaddr *)&clientAddr, clientAddrLen);
		//等待客户端的接收确认
		recvfrom(sockfd, ack, sizeof(ack), 0, (struct sockaddr *)&clientAddr, &clientAddrLen);
		//清空data文件数据部分
		memset(&(data[1]), 0, sizeof(data)-1);
	}

	//当文件内容读取完成后，发送下载结束标识
	data[0] = 'e';
	sendto(sockfd, data, 1, 0, (struct sockaddr *)&clientAddr, clientAddrLen); 
	close(fd);
	close(sockfd);   //关闭服务器套接字
	return 0;
}

int main(int argc, char* argv[])
{
	if(argc>2) {printf("用法：uploadFileServer  [端口]\n"); return -1;}
	if(argc==2)  PORT = atoi(argv[1]);
	download();
	return 0;
}

