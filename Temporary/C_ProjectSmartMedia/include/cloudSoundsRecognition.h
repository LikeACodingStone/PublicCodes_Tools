#ifndef _CLOUD_SOUNDS_RECOGNITION_H_
#define _CLOUD_SOUNDS_RECOGNITION_H_

#define USC_ASR_SDK_APP_KEY "vov26jpwyzg4ayxec3qoklmtslev2m4a36ka7tac"
#define USC_ASR_SDK_SECRET_KEY "b818fb13ead37df1ef050c6060629b54"

#include "libusc.h"

struct recog_params
{
	char  domain[50];
	char  audioFormat[50];
	int   IsUseNlu;
	char  NluBuff[50];
	char  appKey[100];
	char  secretKey[100];
};

void init_speech_recognition(USC_HANDLE* handle,struct recog_params * recogParam);

int start_speech_recognition(USC_HANDLE handle);

int stop_speech_recognition(USC_HANDLE handle,char **lastBuff,void (*func)(char*));

void destroy_speech_recognition(USC_HANDLE handle);

void automatic_speech_recognition(USC_HANDLE handle,char *pcmBuff,int pcmLength,char **resultBuf);


#endif


