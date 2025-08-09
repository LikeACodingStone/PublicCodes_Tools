/***************************************************************************
 *  gpio4405.h
 *
 *	Health Gene Technologies, All Rights Reserved
 *
 *  Created on  : Aug 29, 2021 16:47:05 PM
 *  Author      : Justin Xia
 *
 *  Version Info: 1.0
 *
 ***************************************************************************/
#include <string>
using namespace std;

enum AudioChannel
{
	CH_VOICE,
	CH_INTERSPK,
	CH_MEDIA,
	CH_XFchip_RESET
};

const string CH_CTRL_GPIO_2 = "/dev/gpio2";
const string CH_CTRL_GPIO_3 = "/dev/gpio3";

const string CH_CTRL_GPIO_8 = "/dev/gpio8";
const string CH_CTRL_GPIO_9 = "/dev/gpio9";

const int GPIO_OFF	= 0;
const int GPIO_ON	= 1;

bool setGPIO(string gpio, char status);

bool getGPIO(string gpio, char &status);

/*
 * IO ports defined:
 *
 * GPIO56=0   GPIO57=0     �ı�ѡͨ
 * GPIO56=1   GPIO57=0     �Խ�ѡͨ
 * GPIO56=0   GPIO57=1     ���ѡͨ
 * GPIO56=1   GPIO57=1     XFоƬ��λ
 *
 * GPIO56�Ŷ�Ӧgpio8��GPIO57�Ŷ�Ӧgpio9
 *
 */

DLL_EXPORT bool openAudioChannel(AudioChannel channel);

/* 
 * Define both of left down two buttons pressed to enable the configuration reset
 * on navigation button area when system power on.
 * All the devices connected to HMI will be reset to NOT configured.
 *
 */

DLL_EXPORT bool isResetConfigEnabled();