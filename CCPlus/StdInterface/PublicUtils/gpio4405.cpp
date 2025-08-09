#if defined(__UNIX) || defined(unix)
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#endif

#include "gpio4405.h"


bool openAudioChannel(AudioChannel channel)
{
	bool retval = true;
	switch (channel)
	{
		case CH_VOICE:
			/*
			retval = setGPIO(CH_CTRL_GPIO_8,GPIO_OFF);
			if (retval)
				setGPIO(CH_CTRL_GPIO_9,GPIO_OFF);
			*/
			break;
		case CH_INTERSPK:
			/*
			retval = setGPIO(CH_CTRL_GPIO_8,GPIO_ON);
			if (retval)
				setGPIO(CH_CTRL_GPIO_9,GPIO_OFF);
			*/
			break;
		case CH_MEDIA:
			/*
			retval = setGPIO(CH_CTRL_GPIO_8,GPIO_OFF);
			if (retval)
				setGPIO(CH_CTRL_GPIO_9,GPIO_ON);
			*/
			break;
		case CH_XFchip_RESET:
			retval = setGPIO(CH_CTRL_GPIO_8,GPIO_ON);
			if (retval)
				setGPIO(CH_CTRL_GPIO_9,GPIO_ON);
			break;
		default:
			retval = false;
			break;
	}

	return retval;
}

bool setGPIO(string gpio, char status)
{
#if defined(__UNIX) || defined(unix)
	int fd = open(gpio.c_str(), O_RDWR);
	
	if (fd < 0)
	{
		printf("open gpio:%s error!\n", gpio.c_str());
		return false;
	}

	write(fd, &status, sizeof(status));
	close(fd);
#endif

	return true;
}

bool getGPIO(string gpio, char &status)
{
#if defined(__UNIX) || defined(unix)
	int fd = open(gpio.c_str(), O_RDWR);
	
	if (fd < 0)
	{
		printf("open gpio:%s error!\n", gpio.c_str());
		return false;
	}

	read(fd, &status, sizeof(status));
	close(fd);
#endif

	return true;
}

bool isResetConfigEnabled()
{
	char f2Status = 1;
	char f3Status = 1;
	
	if (getGPIO(CH_CTRL_GPIO_2, f2Status) && getGPIO(CH_CTRL_GPIO_3, f3Status))
	{
		if ((f2Status == 0) && (f3Status == 0))
			return true;
	}

	return false;
}
