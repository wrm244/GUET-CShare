#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#define SERVER_IP "127.0.0.1"
#define PORT 8888

char *Data[] = {"First", "Second", "Third", "exit"};
int main()
{
	int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd==-1) return -1;
                struct sockaddr_in servAddr;
	int servAddrLen = sizeof(servAddr);
	bzero(&servAddr, sizeof(servAddr));    //清空
	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(PORT);
	//使用inet_addr函数将点分十进制的IP转换成一个长整数型数
	servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	int i;
	char data[100];
	for(i = 0; i < 3; i++)
	{
		sendto(sockfd, Data[i], strlen(Data[i])+1, 0, (struct sockaddr *)&servAddr, servAddrLen);
		memset(data, 0, 100);
		recvfrom(sockfd, data, sizeof(data), 0, (struct sockaddr *)&servAddr, &servAddrLen);
		printf("%s\n", data);
	}
	sendto(sockfd, Data[i], strlen(Data[3])+1, 0, (struct sockaddr *)&servAddr, servAddrLen);
	close(sockfd);	
	return 0;
}