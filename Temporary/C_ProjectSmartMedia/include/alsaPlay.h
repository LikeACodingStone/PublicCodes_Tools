#ifndef _ALSA_PLAY_H_
#define _ALSA_PLAY_H_
#include <semaphore.h>
#include "alsaInterface.h"
#include "cmdReqInterface.h" 
#include "speex/speex_resampler.h"


sem_t SEM_CMD_DEALED;
sem_t SEM_PLAY_READED;
sem_t SEM_PLAY_WRITED;
sem_t RECORD_IDLING;
sem_t PTHREAD_CHANGE;


pthread_mutex_t		pthread_state;
pthread_mutex_t		pthread_id_state;
pthread_mutex_t		MUTEX_FOR_PLAYING;
pthread_mutex_t		MUTEX_OVER_DETECT;




int   CONTROL_STREAM_FLAG;  

int  SOCKET_FOR_CLOSE;
char commonBuff[1024 *8];
char *	CHARplayPre;
char *	CHARplayNow;
char *	CHARplayStart;
pthread_t OVER_TIME_DETECT;
int  DEMO_DEADING;

AlsaHandle alsaWakeUpBeep;
pthread_mutex_t		pthread_record;




#define PAUSE_RESUME_PAUSE    1
#define PAUSE_RESUME_RESUME   2
#define PAUSE_RESUME_STOP     3
#define DOWN_VOICE 	          4
#define UPPER_VOICE           5
#define CHANGE_SONGS          6

#define true 	1
#define false	0


#define PERROR(RC,STR)\
{\
	if(RC < 0)\
	{\
		BASE_ERROR_LOG(ErrLogHandle,"%s%s",STR,"\n");\
		exit(-1);\
	}\
}

struct play_params
{
	ControlHandle  pCtrlHandle;
 	snd_mixer_t **		    handleMixer;
  	snd_mixer_elem_t **    handleElement;
	unsigned char *outPtr;
	int srLength;
	int preTTSFlag;
	int *pauseFlag;
	int samRate;
	int chnNum;
};

//snd_pcm_t*            handle;   
//snd_pcm_hw_params_t*  params;

struct buffer {
  unsigned char  *start;
  unsigned char  *pre;
  unsigned char  *playing;
  unsigned long       length;
  unsigned long       lengPre;
  void (*playMP3)(struct play_params *);
  void (*gettingBuff)(char**,int*,int);
  ControlHandle pCtlHandle; 
  snd_mixer_t *		    mixer;
  snd_mixer_elem_t *    element;
};

struct buffer_input
{
	struct buffer *buff;
	struct mad_decoder *decoder;
};

#define WITHOUT_PALYING  0
#define MEDIA_PLAYING    1
#define TTS_PLAYING      2
struct req_stream_content
{
	int flag_play_type;
	char * buff_HTTP;
	char * req_Content;	
};

typedef struct controlStruct
{
	SpeexResamplerState *resampler16;
	SpeexResamplerState *resampler225;
	SpeexResamplerState *resampler48;
	int16_t  int16InBuf[4096];
	int16_t  *int16OutBuf;
	char     *outPlayBuf;
	AlsaHandle AlsaPlayHandle;
	char *	chPlayPre;
	char *	chPlayNow;
	char *	chPlayStart;
	void (*playfunc)(struct play_params *);
	void (*pushBuff)(char**,int*,int);
	char *  quesCmdBuff;						/************quesCmd ********/
	char *  httpMp3Buff;						/************store  mp3	stream from http 	*****/
	char *  postHttpReq;						/************store  request cmd	 to get 8098 content *****/	
	char *  jsonBaseBuff;						/************for json Dealing process *******/
	char *  jsonTempBuff;						/************for json Dealing process 	too*******/
	struct req_stream_content StruReqCont;
	
}ControlBuffer;


//int end_decoding_flag = 1;


void playMusic(struct play_params* param);

void fflushCommonBuffer(char **cData,int *cLength,int ffFlag);

int decPlayingThread(AlsaHandle alsaPlayHd);


#endif

