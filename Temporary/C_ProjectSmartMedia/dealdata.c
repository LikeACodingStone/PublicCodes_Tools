#include "dealdataInterface.h"
#include "dealdata.h"
#define _WAKEUP_
#ifndef _WAKEUP_
char *playbuf;
#endif
//#define RCDF
AsrIntAndStopp AsrInit;
AsrIntAndStopp AsrStop;
AsrStrAndDesp AsrStart;
AsrStrAndDesp AsrDestroy;
AsrfeedDatap AsrfeedData;
//AsrStopp AsrStop;

void asrgetbuf(char* buffer){
	//printf("you can desplay func  string is %s\n",buffer);
}
void backfunc(tyasrintf func,char* getbuf){
	func(getbuf);
}
void onResult(char *pcmBuffer,int length,void* phdle,void* hdle) {
	AlsaHandle phandle = *((AlsaHandle*)phdle);
	AlsaHandle handle = *((AlsaHandle*)hdle);
	//pcmAlsaFeedData(&pcmHandle,pcmBuffer,length);
	alsaStop(handle);
	alsaStart(phandle);
	int size = length;	
	alsaSend(phandle,pcmBuffer,length);	
}
static int shtofl(short* shin, float* flout,int len ){
	int i = 0;
	for(;i < len; i++){
		flout[i] = shin[i]/32768.0f;
	}

}
static int fltosh(float* flin, short* shout, int len ){
	int i = 0;
	for(;i < len; i++){
		shout[i] = (short)(flin[i] * 32768.0f);
	}
}
#ifndef _WAKEUP_
static int playanswer(char* inpath,AlsaHandle phandle,AlsaHandle handle){

	char buffer1[3200 * 10];
	FILE *fd = fopen(inpath,"rb");
	if(fd < 0){
		perror("open playanswer failed");
		return -1;
	}
	fseek(fd,44,SEEK_SET);
	//	if(read(fd,playbuf,44) > 0){
	//	memset(playbuf,0,10240);
	alsaStop(handle);
	alsaStart(phandle);
	int size = -1;
	while ((size=fread(playbuf,1,3200,fd)) >= 3200) {
		if (size < 1024){
			memset(playbuf + size,0,3200 - size);
		}
		alsaSend(phandle,playbuf,size);	
		//	alsaRead(handle,buffer1,size * 10);
	}
	alsaStop(phandle);
	alsaStart(handle);
	//	memset(playbuf,0,3200);
	//	alsaSend(phandle,playbuf,3200);
	//	}
	fclose(fd);

}
#endif

#ifdef _WAKEUP_
char *hxgc;
int hxgcLength = -1;
void readHxgc() {
	FILE *fp = fopen("./hxcg1.wav","rb");
	if (fp == NULL) {
		printf("NULL");
	}
	fseek(fp,0,SEEK_END);
	int length = ftell(fp);
	//length = 32000;
	hxgc = (char *)malloc(length - 44);
	fseek(fp,44,SEEK_SET);
	fread(hxgc,length -44,1,fp);
	fclose(fp);
	hxgcLength = length - 44;
}
#endif


static void micShortToFloat(short *input,float *output,float *aec,int nMics1,int length) {
	int k = 0;
	int j = 0;
	//        int length = (FRAME_LEN << 1);
	float *aec1 = aec + 1024;
	for (k = 0; k < length;k++) {
		output[k] = input[8 + 12 * k] * 0.00003052f;
		aec[k] = input[10 + 12 * k] * 0.00003052f;
		aec1[k] = input[11 + 12 * k] * 0.00003052f;
	}
	for (; j < nMics1 - 1;j++) {
		for (k = 0; k < length;k++) {
			output[k + (j + 1) * length] = input[j + 12 * k] * 0.00003052f;
		}
	}
}



#if 0
extern int DealDataInit(DealDataHandle* dealdatahandle){
#ifdef _WAKEUP_	
	readHxgc();
#else
	playbuf = (char *)malloc(3200);
#endif
	//MicArrayHandle array;/*句柄*/
	micInit(&(dealdatahandle->arrayhandle),nMics,nDirs,FRAME_LEN,chnNum,paramSize,0,0);/*micArray初始化*/
	Debug("micInit ok!\n");

	//HuwenVADHandle vadsnrHandle;
	huwenVadInit(&(dealdatahandle->vadsnrHandle),16000,0.70f,0.50f,2,10);

	//HuwenVADHandle vadHandle;
	huwenVadInit(&(dealdatahandle->vadHandle),16000,0.60f,0.40f,2,20);


	//MicSingleSNRHandle snrHandle;
	micSingleSNRInit(&(dealdatahandle->snrHandle),16000,256,64);
	micSingleSNRReset(&(dealdatahandle->snrHandle));
	Debug("anr prepare ok!\n");

	//mpo init
	//stMPO mpo;
	MPO_Init(&(dealdatahandle->mpo), 16000,15);
	Debug("mpo prepare ok!\n");

	MPO_Init(&(dealdatahandle->mpo0), 16000,0);
	Debug("mpo prepare ok!\n");

	//DoaHandle doahandle;
	doaInit(&(dealdatahandle->doahandle));
	Debug("doa prepare ok!\n");

	DerevebHandle *derevebhandle1 = (DerevebHandle *)malloc(derevebHandleSize());
	dealdatahandle->derevebhandle = derevebhandle1;
	derevebInit(dealdatahandle->derevebhandle);
	Debug("Dereveb prepare ok!\n");

	leveller_Init(&(dealdatahandle->lev), 16000, 512, 128);
	Debug("leveller prepare ok!\n");

	//wakeupMultilateralInit(&(dealdatahandle->wakeupHandle),"/bin/config/JSnnet_1440x128x3x3_0328.txt","/bin/config/JS_wakeup.config");
	wakeupMultilateralInit(&(dealdatahandle->wakeupHandle),"./config/JSnnet_1440x128x3x3_0328.txt","./config/JS_wakeup.config");
	Debug("\nwakeup prepare ok!\n");

	shasrEngineInit(&(dealdatahandle->engine),"./cfg_biglstm.ini");
	Debug("asr prepare ok!\n");
}
#endif
//extern int DealDataProcess(DealDataHandle* dealdatahandle){
static int thread_cap(DealDataHandle *dealdatahandle){
	int asrflag;
	int isWakeup = -1;
	int vadState = -1;

	int muteNumber = 0;
	int voiceNumber = 0;

	Debug("start DealDataProcess\n");
	AlsaHandle handle = alsaInit(dealdatahandle->recdhw,1,12,16000,0);
	AlsaHandle phandle = alsaInit(dealdatahandle->playhw,0,1,16000,0);

#ifdef RCDF
	FILE *fp = fopen("./record.pcm","wb");
	FILE *fp1 = fopen("./test.pcm","wb");
#endif
	int bufferfinalNumber = 15;
	char buffer[2048 * 12];
	float bufferaec[1024];

	char bufferfinal[512 * bufferfinalNumber];
	int index = 0;
	int bufferSize = 0;
	//	int i = 300;
	short pbuffer[512];
	int number = FRAME_LEN * 2;
	int length=0;

	//short *mpo_out = (short *)malloc(sizeof(short) * FRAME_LEN);
	float *mpo_out = (float *)malloc(sizeof(float) * FRAME_LEN);
	if(mpo_out == NULL){
		printf("mpo_out malloc failed!\n");
	}

	float *input = (float*)malloc(nMics * FRAME_LEN * 2 * sizeof(float));
	float *aec = (float *)malloc(FRAME_LEN * 4 * sizeof(float));
	float *aecOut = (float *)malloc(sizeof(float) * nMics * FRAME_LEN * 2); 
	short *output = (short*)input;
	float scores1[4];
	/*
	   char *array_out = (char *)malloc(FRAME_LEN * 2 * sizeof(char));
	   if(array_out == NULL){
	   printf("array_out malloc failed!\n");
	   }
	   float *array_out_f = (float *)malloc(FRAME_LEN *sizeof(float));
	   if(array_out_f == NULL){
	   printf("array_out_f malloc failed!\n");
	   }
	 */
	float *fixfData = (float *)malloc(nDirs * FRAME_LEN * sizeof(float));
	if(fixfData == NULL){
		printf("fixfData malloc failed!\n");
	}

	//int fd6=open("asr_before.pcm",O_WRONLY|O_CREAT,0664);
	Debug("start while!\n");
	//	long start,end;
	//	double runframetime=0;
	wakeupMultilateralStart(&(dealdatahandle->wakeupHandle));
	FILE *fp11 = fopen("./testpcm.pcm","wb");
	//while (i-- > 0) {
	while (1) {
		Debug("start while!\n");
		//length = recordReadData(&handle,buf);
		//fwrite(buf,length,1,record_fp);

		length = alsaRead(handle,buffer,2048 * 12);
		//printf("length = %d\n");
		//		start = clock();
		Debug("record data!\n");
		Debug("record length = %d \n",length);
		//short *buf = (short *)buffer1;


		//		start = clock();

		/*	
			for (; j < nMics;j++) {
			int k = 0;
			for (; k < FRAME_LEN;k++) {
			if(j == 0){
			buffer[k + j * FRAME_LEN] = ((short *)buf)[8 + j + 12 * k];
			}else{				
			buffer[k + j * FRAME_LEN] = ((short *)buf)[0 + j + 12 * k];
			}
		//buffer[k + j * FRAME_LEN] = ((short *)buf)[k * 4 + j ];
		//Debug("%d ",buffer[k + j * FRAME_LEN]);				
		}
		}
		 */
		 
		   //short aecBuff[1024 * 12];
		   //fltosh(buffer,aecBuff,1024);	
		   //fwrite(buffer,1024*12*2,1,fp11);
		//   fclose(fp);


		micShortToFloat((short *)buffer,input,aec,nMics,number);


		aecProcess(&(dealdatahandle->aechandle),aec,input,aecOut);
		fwrite(aecOut,1024*1*2,1,fp11);
		 
		int j = 0;
		//int k = 0;
		for (j = 0; j < nMics;j++) {
			memcpy(input + j * FRAME_LEN,aecOut + j * 1024,2048);
			memcpy(input + FRAME_LEN * nMics + j * FRAME_LEN,aecOut + j * 1024 + FRAME_LEN,2048);
		}
		int dir[4];
		int aecIndex = 0;
		for (; aecIndex < 2;aecIndex++) {
			int *doaDir = dir + aecIndex * 2;

			micProcess(&(dealdatahandle->arrayhandle),input + aecIndex * FRAME_LEN * nMics);/*进行处理，处理完之后将得到fix及自适应之后的结果*/

			length = micGetFixDataf(&(dealdatahandle->arrayhandle),fixfData);

			doaProcess(&(dealdatahandle->doahandle),fixfData,doaDir);
			//printf("%d %d\n",doaDir[0],doaDir[1]);
			Debug("doaProcess!\n");
			MPO_processf(&(dealdatahandle->mpo),fixfData + doaDir[0] * FRAME_LEN,mpo_out, FRAME_LEN >> 1);
			MPO_processf(&(dealdatahandle->mpo),fixfData + doaDir[1] * FRAME_LEN + (FRAME_LEN >> 1),mpo_out + (FRAME_LEN >> 1), FRAME_LEN >> 1);
			Debug("MPO_process over!\n");
			float foutBuf[FRAME_LEN]={0};// = malloc(sizeof(short) *FRAME_LEN);
			float levout[FRAME_LEN]={0};

			j=0;
			Debug("derevebProcess start! derevbsize = %d\n",derevebHandleSize());
			for(; j < 8; j++){
				//printf("===%d\n",j);
				//derevebProcess(dealdatahandle->derevebhandle,(char *)(mpo_out+j*64),foutBuf + 64 * j,128);
				derevebProcess(dealdatahandle->derevebhandle,(mpo_out+j*64),foutBuf + 64 * j,64);	
			}

			//if (length > 0) {
			float scores[2];
			int snrLength;
			//			huwenVadFeedData(&(dealdatahandle->vadsnrHandle),(char *)foutBuf,FRAME_LEN << 1,scores);
			huwenVadFeedFData(&(dealdatahandle->vadsnrHandle),foutBuf,FRAME_LEN << 1,scores);
			snrLength = FRAME_LEN;
			//			leveller(&(dealdatahandle->lev), huwenVadGet(&(dealdatahandle->vadsnrHandle)), foutBuf, levout,FRAME_LEN );
			//			float *snrBuffer = micSingleSNRProcess(&(dealdatahandle->snrHandle),scores,levout,&snrLength);
			float *snrBuffer = micSingleSNRProcess(&(dealdatahandle->snrHandle),scores,foutBuf,&snrLength);
			//short mpoOut[FRAME_LEN];
			short *out = output + aecIndex * FRAME_LEN;
			//MPO_process(&(dealdatahandle->mpo0),snrBuffer,mpoOut, FRAME_LEN >> 1);
			//MPO_process(&(dealdatahandle->mpo0),snrBuffer + (FRAME_LEN >> 1),mpoOut + (FRAME_LEN >> 1), FRAME_LEN >> 1);
			MPO_process(&(dealdatahandle->mpo0),snrBuffer,out, FRAME_LEN >> 1);
			MPO_process(&(dealdatahandle->mpo0),snrBuffer + (FRAME_LEN >> 1),out + (FRAME_LEN >> 1), FRAME_LEN >> 1);
		}

		//if (snrLength <= 0) {
		//	continue;
		//}
		//----------------------------------------add asr and wakeup----------------------------------------------------
		if (isWakeup < 0) {
			//isWakeup = wakeupMultilateralDecode(&wakeupHandle,snrBuffer,snrLength* 6);
			isWakeup = wakeupMultilateralDecode(&(dealdatahandle->wakeupHandle),(char*)output,FRAME_LEN * 2 * 2 * 6);

			if (isWakeup >= 0) {
				//printf("isWakeup ====%d\n",isWakeup);
				wakeupMultilateralStop(&(dealdatahandle->wakeupHandle));
				isWakeup = -1;
				alsaStop(handle);
				alsaStart(phandle);
				//alsaSend(phandle,hxgc,hxgcLength);
				alsaStop(phandle);
				alsaStart(handle);
			}
			//#ifndef _WAKEUP_
		} else {

			//vadState 0,1 = start ,2 = end
			vadState = huwenVadFeedData(&(dealdatahandle->vadHandle),(char*)output,FRAME_LEN << 1,scores1);
			dealdatahandle->wfun((char*)output,isWakeup,vadState);
			micSingleSNRReset(&(dealdatahandle->snrHandle));
			wakeupMultilateralStart(&(dealdatahandle->wakeupHandle));
			//printf("vadstate ====%d\n",vadState);
			//#ifndef _WAKEUP_	
			if (vadState == 2) {

				isWakeup = -1;
				wakeupMultilateralStart(&(dealdatahandle->wakeupHandle));

				micSingleSNRReset(&(dealdatahandle->snrHandle));



			} 
		}


		//free(fixfData);


		}
	}

static void* thread(void* arg){
	thread_cap((DealDataHandle*)arg);
}


extern int DealDataInit(DealContext* dealdatacontext,char* recdhw,char* playhw,int asrkey,char* asrconf,tywkf wfun,char* huanxingmoxing,char* configfile,...){/*tywkf wfun 是回调函数*/
	//printf("heooooooooooooooooooooo\n");
	dealdatacontext->handle = (void *)malloc(sizeof(DealDataHandle));

	DealDataHandle *dealdatahandle = (DealDataHandle *)dealdatacontext->handle;

#ifndef _ANDROID_
#ifdef _WAKEUP_	
	//readHxgc();
#else
	playbuf = (char *)malloc(3200);
#endif
#endif
	//printf("heooooooooooooooooooooo\n");
	//save hw to struct
	dealdatahandle->recdhw = recdhw;
	//printf("record hardware is %s\n",dealdatahandle->recdhw);
	dealdatahandle->playhw = playhw;
	//printf("play hardware is %s\n",dealdatahandle->playhw);

	//MicArrayHandle array;/*句柄*/
	//printf("micInit\n");
	micInit(&(dealdatahandle->arrayhandle),nMics,nDirs,FRAME_LEN,chnNum,paramSize,0,0);/*micArray初始化*/
	Debug("micInit ok!\n");

	//HuwenVADHandle vadsnrHandle;
	huwenVadInit(&(dealdatahandle->vadsnrHandle),16000,0.70f,0.50f,2,10);

	//HuwenVADHandle vadHandle;
	huwenVadInit(&(dealdatahandle->vadHandle),16000,0.60f,0.40f,2,20);


	//MicSingleSNRHandle snrHandle;
	micSingleSNRInit(&(dealdatahandle->snrHandle),16000,256,64);
	micSingleSNRReset(&(dealdatahandle->snrHandle));
	Debug("anr prepare ok!\n");

	//mpo init
	//stMPO mpo;
	MPO_Init(&(dealdatahandle->mpo), 16000,15);
	Debug("mpo prepare ok!\n");

	MPO_Init(&(dealdatahandle->mpo0), 16000,0);
	Debug("mpo prepare ok!\n");

	//DoaHandle doahandle;
	doaInit(&(dealdatahandle->doahandle));
	Debug("doa prepare ok!\n");

	DerevebHandle *derevebhandle1 = (DerevebHandle *)malloc(derevebHandleSize());
	dealdatahandle->derevebhandle = derevebhandle1;
	derevebInit(dealdatahandle->derevebhandle);
	Debug("Dereveb prepare ok!\n");

	leveller_Init(&(dealdatahandle->lev), 16000, 512, 128);
	Debug("leveller prepare ok!\n");

#ifdef _AEC_
	//int i = 0;
	//for (i = 0; i < nMics; i++) {
	//	aecInit(&(dealdatahandle->aechandle[i]));
	//}
	aecInit(&(dealdatahandle->aechandle),nMics,2);
#endif


	va_list argprmf;
	//char *argv1;
	//char *argv2;	
	va_start(argprmf, configfile);
	dealdatahandle->argv1 = va_arg(argprmf, char *);	
	dealdatahandle->argv2 = va_arg(argprmf, char *);
	va_end(argprmf);
	//printf("dealdatahandle->argv1 = %s\n",dealdatahandle->argv1);
	//printf("dealdatahandle->argv2 = %s\n",dealdatahandle->argv2);


	//wakeupMultilateralInit(&(dealdatahandle->wakeupHandle),"./config/JSnnet_1440x128x3x3_0328.txt","./config/JS_wakeup.config");
	wakeupMultilateralInit(&(dealdatahandle->wakeupHandle),huanxingmoxing,configfile);
	Debug("\nwakeup prepare ok!\n");

	dealdatahandle->wfun = wfun;
#ifndef _WAKEUP_
	//printf("asrkey=%d\n",asrkey);
	if(asrkey == 1){
		AsrInit = JMAsrInit;
		AsrStart = JMAsrStart;
		AsrfeedData = JMAsrfeedData;
		AsrStop = JMAsrStop;
		AsrDestroy = JMAsrDestroy;
	}
	else if(asrkey == 2){
		AsrInit = shasrEngineInit;
		AsrStart = shasrEngineStart;
		AsrfeedData = shasrEnginefeedData;
		AsrStop = shasrEngineStop;
		AsrDestroy = shasrEngineDestroy;
	}
	AsrInit(&(dealdatahandle->engine),asrconf);
	//printf("\nhehe\n");
	Debug("asr prepare ok!\n");
#endif
}
extern int DealDataProcess(DealContext* dealdatacontext){
	DealDataHandle *dealdatahandle = (DealDataHandle *)dealdatacontext->handle;
	pthread_create(&(dealdatahandle->pid),NULL,thread,(void*)dealdatahandle);

}

extern int DealDataDestroy(DealContext* dealdatacontext){


	Debug("DealDataDestyoy\n");
	DealDataHandle *dealdatahandle = (DealDataHandle *)dealdatacontext->handle;
	pthread_join(dealdatahandle->pid, &(dealdatahandle->thread_result));

	micDestroy(&(dealdatahandle->arrayhandle));/*销毁micArrayAPI使用的相关资源*/
	huwenVadDestroy(&(dealdatahandle->vadHandle));
	micSingleSNRDestroy(&(dealdatahandle->snrHandle));
	doaDestory(&(dealdatahandle->doahandle));
	leveller_Destroy(&(dealdatahandle->lev));
	free(dealdatacontext->handle);
	return TRUE;

} 






