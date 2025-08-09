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
 * GPIO56=0   GPIO57=0     文本选通
 * GPIO56=1   GPIO57=0     对讲选通
 * GPIO56=0   GPIO57=1     广告选通
 * GPIO56=1   GPIO57=1     XF芯片复位
 *
 * GPIO56脚对应gpio8，GPIO57脚对应gpio9
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