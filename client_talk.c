#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>


void* start_recv(void* arg)
{
	int sockfd = *(int*)arg;
	char buf[1024] = {};
	for(;;)
	{
		recv(sockfd,buf,sizeof(buf),0);
		printf("%s\n",buf);
	}
}

int main()
{
	//创建套接字
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sockfd)
	{
		perror("socket");
		return -1;
	}
	//准备地址
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7786);
	addr.sin_addr.s_addr = inet_addr("172.20.10.2");
	socklen_t len = sizeof(addr);
	//连接
	if(connect(sockfd,(struct sockaddr*)&addr,len))
	{
		perror("connect");
		return -1;
	}
	puts("***欢迎来到聊天室***");
	printf("请输入你的聊天名：");
	char name[20] = {};
	gets(name);

	pthread_t pid;
	pthread_create(&pid,NULL,start_recv,&sockfd);
	
	char buf[1024] = {};
	char buf1[1024] = {};
	while(1)
	{
		gets(buf);
		sprintf(buf1,"%s：%s",name,buf);
		send(sockfd,buf1,strlen(buf1)+1,0);
		if(0 == strcmp("quit",buf))
		{
			printf("***退出聊天室***\n");
			close(sockfd);
			return 0;
		//	pthread_exit(NULL);		
		}
	}
//	void* ptr = NULL;
//	pthread_join(pid,&ptr);
}
























