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



