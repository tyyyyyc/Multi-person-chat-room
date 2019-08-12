#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

int clifd[20];
int size = 20;
char ip[20][20];

void* star_run(void* arg)
{
	int index = 0;
	int fd = *(int*)arg;
	char buf[1024] = {};
	for(int i=0;i<size;i++)
	{
		if(fd == clifd[i])
		{
			index = i;
			break;
		}
	}
	sprintf(buf,"欢迎新人%s",ip[index]);
	for(int i=0;i<size;i++)
	{
		if(clifd[i]!=0 && clifd[i]!=fd)
		{
			send(clifd[i],buf,strlen(buf)+1,0);
		}
	}
	
	while(1)
	{
		char buf1[1024] = {};
		char buf2[1024] = {};
		recv(fd,buf1,sizeof(buf1),0);
		if(0 == strcmp("quit",buf1))
		{
			for(int i=0;i<size;i++)
			{
				if(fd == clifd[i])
				{
					clifd[i] = 0;
					close(fd);
					break;
				}
			}
			sprintf(buf2,"%s已退出该群聊...",ip[index]);
			puts(buf2);
			int j;
			for(j=0;j<size;j++)
			{
				if(clifd[j]!=0 && clifd[j]!=fd)
				{
					send(clifd[j],buf2,strlen(buf2)+1,0);
				}
			}
			send(clifd[j],buf2,strlen(buf2)+1,0);
			pthread_exit((void*)j);
	  	}

		printf("%s\n",buf1);
		for(int k=0;k<size;k++)
		{
			if(clifd[k]!=0 && clifd[k]!=fd)
			{
				send(clifd[k],buf1,strlen(buf1)+1,0);
			}
		}
	}
}

int main()
{
//	Pthread_t pid;
	printf("服务器创建socket...\n");
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(0 > sockfd)
	{
		perror("socket");
		return -1;
	}

	printf("准备地址...\n");
	struct sockaddr_in addr = {};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(7786);
	addr.sin_addr.s_addr = inet_addr("172.20.10.2");
	socklen_t len = sizeof(addr);

	printf("绑定socket与地址...\n");
	if(bind(sockfd,(struct sockaddr*)&addr,len))
	{
		perror("bind");
		return -1;
	}

	printf("设置监听...\n");
	if(listen(sockfd,5))
	{
		perror("listen");
		return -1;
	}

	printf("等待客户端连接...\n");
	while(1)
	{		
		struct sockaddr_in addrcli = {};
		int fd = accept(sockfd,(struct sockaddr*)&addrcli,&len);
		if(0 > fd)
		{
			perror("accept");
			continue;
		}
		int i;
		for(i=0;i<size;i++)
		{
			if(clifd[i] == 0)
			{
				//记录客户端的socket
				clifd[i] = fd;
				//创建线程为客户端服务
				pthread_t pid;
				pthread_create(&pid,NULL,star_run,&fd);
				strcpy(ip[i],inet_ntoa(addr.sin_addr));
				
				break;
			}		
		}
		if(i == size)
		{
			char* str = "聊天室已满，谢谢！";
			send(fd,str,strlen(str),0);
			close(fd);
		}
	}
}
















