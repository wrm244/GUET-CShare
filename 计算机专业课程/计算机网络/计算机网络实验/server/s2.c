#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#define PORT 8888

int main()
{
	int sockfd;
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)  return -1;
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));    //清空
	addr.sin_family = AF_INET;
	addr.sin_port = htons(PORT);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	//与PORT端口进行绑定
	if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
	    return -1;
	}
	char data[100];
	//接收客户端传来的数据，并打印出来(提示：换行打印)
	//同时将接收到的数据原样发送给客户端
	//当接收到"exit"字符串时，退出当前程序，不打印出"exit"字符串
	//提示：在每次接收字符串前要将存放字符串的变量清空
	struct sockaddr_in clientAddr;
                int clientAddrLen = sizeof(clientAddr);
                memset(data, 0, sizeof(data));
                int len;//接收数据长度
                for(;;)
               {
                   /**********BEGIN **********/ //接收数据
                   len=recvfrom(sockfd,data,sizeof(data),0,(struct sockaddr*)&clientAddr,&clientAddrLen);
                   /********** END **********/
                   if(len<=0) break;
                   if(strcasecmp(data, "exit") == 0)
                   break;
                   /**********BEGIN **********/ //发送数据
                    sendto(sockfd,data,strlen(data)+1,0,(struct sockaddr*)&clientAddr, clientAddrLen);
                   /********** END **********/
                   printf("%s\n", data);
                   memset(data, 0, sizeof(data));
               }
	/**********BEGIN **********/ 
		close(sockfd);
                /********** END **********/
	return 0;
}

