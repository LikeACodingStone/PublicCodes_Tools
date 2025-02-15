#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<pthread.h>
#include<signal.h>
#include<time.h>
#include<errno.h>
#include<memory.h>
#include<netinet/tcp.h>
#include<unistd.h>
#include<netdb.h>
#include "logInterface.h"
#include "speechCloudClient.h"
#include "huwenvadInterface.h"
#include "voiceInterface.h"


int writeRecStart(int sockfd)
{
	struct post_content sendData = {0};
	sendData.contType = 1;
	sendData.contLength = 0;

	pthread_mutex_lock(&pthread_record);
		FLAG_BEGAIN_DETOVTM = 1;
		writeOverTime = time((time_t *)NULL);
	pthread_mutex_unlock(&pthread_record);
	int retVal = write(sockfd,&sendData,sizeof(struct post_content));
	BASE_INFO_LOG(InfoLogHandle,"%s","End write Start speech\n");
	if(retVal < 0)
		return -1;
	pthread_mutex_lock(&pthread_record);
		FLAG_BEGAIN_DETOVTM = 0;
	pthread_mutex_unlock(&pthread_record);	
	return retVal;
}
int writeRecStop(int sockfd)
{
	struct post_content sendData = {0};
	sendData.contType = 3;
	sendData.contLength = 0;

	pthread_mutex_lock(&pthread_record);
		FLAG_BEGAIN_DETOVTM = 1;
		writeOverTime = time((time_t *)NULL);
	pthread_mutex_unlock(&pthread_record);
	int retVal = write(sockfd,&sendData,sizeof(struct post_content));
	if(retVal < 0)
		return -1;
	pthread_mutex_lock(&pthread_record);
		FLAG_BEGAIN_DETOVTM = 0;
	pthread_mutex_unlock(&pthread_record);
	BASE_INFO_LOG(InfoLogHandle,"%s","finish write Stop speech\n");
	
	return retVal;
}


int connectServer(char *ipaddr,int port,char ** cntRot)
{
	struct sockaddr_in addr;
	memset(&addr,0,sizeof(addr));
	int sockfd = socket(AF_INET,SOCK_STREAM,0);
	addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
	inet_pton(AF_INET,ipaddr,&addr.sin_addr.s_addr);
	int conRet = 0;
	time_t curSeconds,preSeconds;
	preSeconds = time((time_t *)NULL);
	while(1)
	{
		conRet= connect(sockfd,(struct sockaddr*)&addr,sizeof(addr));
		if(conRet >= 0)
			break;
    	usleep(100000);
		curSeconds = time((time_t *)NULL);
		if(curSeconds > (preSeconds + 4))
		{
			strcpy(*cntRot,"CLOUDSERVER_ERROR");
			preSeconds = time((time_t *)NULL);
		}
    }
	BASE_INFO_LOG(InfoLogHandle,"%s%s%s","Succeed Connect ",ipaddr,"\n");
	return sockfd;
}


void *pthread_showCont(void *arg)
{
	struct post_content sendData = {0};
	struct get_content *getParams = (struct get_content *)arg;
	char * recogOut = (char *)malloc(8192);
	memset(recogOut,0,8192);
	char frameData[4112] = "";
	char frameRest[2056] = "";
	int frameCount = 0;
	int retVal = 0;
	struct post_content recvCont = {0};
//	struct timeval timeout = {8,5}; 
//	setsockopt(getParams->socket_fd,SOL_SOCKET,SO_RCVTIMEO,(char *)&timeout,sizeof(struct timeval));
	while(1)
	{
		memset(&sendData,0,sizeof(struct post_content));
		retVal = recv(SOCK_FD_CLIENT,&sendData,sizeof(struct post_content),0);
		if(retVal < 0)
		{
			while(SOCK_FD_CLIENT == 0)
			{	
				usleep(10);
			}
			continue;
		}
		
//		retVal = read(getParams->socket_fd,&sendData,sizeof(struct post_content));
		memcpy(frameData+frameCount,&sendData,retVal);
		frameCount += retVal;
		if(frameCount < 2056)
		{
			continue;
		}else{
			memset(&recvCont,0,2056);
			memcpy(&recvCont,frameData,2056);
			memset(frameRest,0,2056);
			memcpy(frameRest,frameData+2056,frameCount-2056);
			memset(frameData,0,4112);
			memcpy(frameData,frameRest,frameCount-2056);
			frameCount = frameCount-2056;
			switch(recvCont.contType)
			{
				case 1:
						strcat(recogOut,recvCont.buff);
						break;

				case 2:
						strcat(recogOut,recvCont.buff);
						getParams->resultFun(recogOut,getParams->cloudResult);
						usleep(10);
						memset(recogOut,0,8192);
						pthread_mutex_lock(&send_state);	
							CLOUD_SPEECH_SENDING = 0;
						pthread_mutex_unlock(&send_state);
						sem_post(&SEM_OVER_SEND);
						break;
			}
		}
		usleep(10);
	}
	free(recogOut);
}

void create_readFunc(int *sockfd,struct get_content *getParams,void (*funcResult)(char*,char **),char**resOut)
{
	getParams->socket_fd = sockfd;
	getParams->resultFun = funcResult;
	getParams->cloudResult = resOut;
	pthread_t thread;
    pthread_attr_t  attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
    pthread_create(&thread,&attr,pthread_showCont,(void*)getParams);
}


