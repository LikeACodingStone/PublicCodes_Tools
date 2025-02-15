#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <alsa/asoundlib.h>
#include <linux/soundcard.h>
#include "baseModule.h"
#include "semcParse.h"
#include "madAlsaPlay.h"
#include "huwenvadInterface.h"
#include "voiceInterface.h"
#include "wakeupInterface.h"
#include "speechCloudClient.h"
#include "cloudSoundsRecognition.h"

pthread_mutex_t		MUTEX_RcdRcg;

static void micShortToFloat(short *input,float *output,float *aec,int nMics,int length)
{
    int k = 0;
    int j = 0;
    //        int length = (FRAME_LEN << 1);
    float *aec2 = aec + 1024;
    for (k = 0; k < length;k++) {
            output[k] = input[8 + 12 * k] * 0.00003052f;
            aec[k] = input[10 + 12 * k] * 0.00003052f;
            aec2[k] = input[11 + 12 * k] * 0.00003052f;
    }
    for (; j < nMics - 1;j++) {
            for (k = 0; k < length;k++) {
                    output[k + (j + 1) * length] = input[j + 12 * k] * 0.00003052f;
            }
    }
}

static void micfloatToShort(float *input,short *output,int length)
{
	int i = 0, j = 0;
	for(j = 0;j < length;j++)
	{
		output[j] = input[j] / 0.00003052f;
	}
}


extern void rcdSpeechRecognition(void (*func_result)(char*),int noSpeekTm,int maxSpeekTm)
{
	USC_HANDLE handleUSC;
	struct recog_params recogParam = {0};
	strcpy(recogParam.appKey,USC_ASR_SDK_APP_KEY);
	strcpy(recogParam.secretKey,USC_ASR_SDK_SECRET_KEY);
	strcpy(recogParam.domain,RECOGNITION_FIELD_GENERAL);
	strcpy(recogParam.audioFormat,AUDIO_FORMAT_PCM_16K);
	recogParam.IsUseNlu = 0;
	init_speech_recognition(&handleUSC,&recogParam);	
	
	pthread_mutex_init(&MUTEX_RcdRcg,NULL);

	int doaData = 0,vadNumber = -1,wakeRetVal = -1,i = 0,countSc = 0;
	float scores[4];
	time_t preSeconds,curSeconds,vadOverSeconds;
	char *buffer = (char *)malloc(2048 *12);
	char *buffOut = (char *)malloc(2048);
	float *aec = (float *)calloc(2048,4);
	float *input = (float *)calloc(1024 * 12,4);
	short *output = (short *)calloc(1024,2);
	int doadir[4];	
	char *pRcdOutBuf = (char*)calloc(1024,10);
	static int flag_speeked = 0;	
	
	
	HuwenVADHandle	vadHandle;
	huwenVadInit(&vadHandle,16000,0.5,0.5,2,30);

	VoiceHandle voiceHandle;
	voiceInit(&voiceHandle);
	

	WakeupHandle wakeHandle;
	wakeupSingleInit(&wakeHandle,"./wakeupFile/JSnnet_1440x128x3x3_0328.txt.xiaozhi"
						,"./wakeupFile/JS_wakeup.config");
			
	AlsaHandle recordHandle;
	recordHandle = alsaInit("plughw:2,0",1,12,16000,0);
	alsaStart(recordHandle);	
	
	wakeupSingleStart(wakeHandle);

	while(1)
	{
		alsaRead(recordHandle,buffer,2048 * 12);
//		fwrite(buffer,1,2048*12,fp);
		
		micShortToFloat((short *)buffer,input,aec,9,1024);
		voiceProcess(voiceHandle,input,output,aec,doadir);
//		fwrite((char*)output,1,2048,fp1);
		
		wakeRetVal = wakeupSingleDecode(wakeHandle,(char *)output,2048);
		if(wakeRetVal == 0)
		{	
			wakeupSingleStop(wakeHandle);
			start_speech_recognition(handleUSC);
			
			BASE_INFO_LOG(InfoLogHandle,"%d%s",FLAG_BEGAIN_DETOVTM,"****************Aready**Wakeup\n");

			pthread_mutex_lock(&pthread_record);
				CONTROL_STREAM_FLAG = 1;
			pthread_mutex_unlock(&pthread_record);

			memset(buffer,0,12 *2048);
			memset(pRcdOutBuf,0,1024 *10);

			preSeconds = time((time_t *)NULL); 
			for(i = 0;i < 15;i++)
			{
				memset(buffer,0,12 *2048);
				alsaRead(recordHandle,buffer,2048 * 12);
				micShortToFloat((short *)buffer,input,aec,9,1024);
				voiceProcess(voiceHandle,input,output,aec,doadir);
				
				automatic_speech_recognition(handleUSC,(char *)output,2048,&pRcdOutBuf);
			}
			BASE_INFO_LOG(InfoLogHandle,"%s","finish post first Sec Data @@\n");
			while(1)
			{
				memset(buffer,0,12 *2048);
				alsaRead(recordHandle,buffer,2048 * 12);
				micShortToFloat((short *)buffer,input,aec,9,1024);		
				voiceProcess(voiceHandle,input,output,aec,doadir);

				vadNumber = huwenVadFeedData(&vadHandle,(char*)output,2048,scores);
				if(vadNumber == 1)
				{
					vadOverSeconds = time((time_t *)NULL);
					if(vadOverSeconds > preSeconds + maxSpeekTm)
					{
						break;
					}
					automatic_speech_recognition(handleUSC,(char *)output,2048,&pRcdOutBuf);
				}
				else if(vadNumber == 2)
				{
					BASE_INFO_LOG(InfoLogHandle,"%s","vad detected ended Speek\n");
					break;	
				}
				else
				{	
					curSeconds = time((time_t *)NULL);
					if(curSeconds > preSeconds + noSpeekTm)
					{
						BASE_INFO_LOG(InfoLogHandle,"%s","three Sec No speeking End WakeUp\n");
						break;
					}
				}
				usleep(100);
			}
			stop_speech_recognition(handleUSC,&pRcdOutBuf,func_result);
			BASE_INFO_LOG(InfoLogHandle,"%s","begain to start Wakeup  \n");
			wakeupSingleStart(wakeHandle);
		}
	}
}

