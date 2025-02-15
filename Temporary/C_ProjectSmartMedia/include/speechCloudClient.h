#ifndef _SPEECH_CLOUD_CLIENT_
#define _SPEECH_CLOUD_CLIENT_
#include <semaphore.h>
#include "cmdReqInterface.h"


time_t writeOverTime;
int FLAG_BEGAIN_DETOVTM;
pthread_mutex_t		send_state;
pthread_mutex_t		time_state;
pthread_mutex_t		pthread_record;
sem_t 	SEM_OVER_SEND;
int SOCK_FD_CLIENT;


int CLOUD_SPEECH_SENDING;

struct detect_stru
{
	char serAddr[20];
	int serPort;
	int  sockFd;
	char ** resCnt;
};

struct post_content
{
	int contType;
	int contLength;
	char buff[2048];
};

struct get_content
{
	char serAddr[20];
	int serPort;
	int socket_fd;
	char **cloudResult;
	void (*resultFun)(char *,char **);
};

struct cont_write_stru
{
	char * arryA;
	char * arryB;	
	int *  ptrInt;
};


typedef struct
{
	int fs;
	float startCore;
	float endCore;
	int startFrame;
	int endFrame;
}VAD_RECORD_PARAMS;



typedef struct 
{
	char redDevice[20];
	char serverAddr[20];
	int serPort;
	void (*func_result)(char*,char**);
	char**resOut;
}SPH_RECG_PARAMS;

int writeRecStart(int sockfd);

int writeRecStop(int sockfd);

int connectServer(char *ipaddr,int port,char ** cntRot);

void *pthread_showCont(void *arg);

void create_readFunc(int *sockfd,struct get_content *getParams,void (*funcResult)(char*,char **),char**resOut);


#endif
