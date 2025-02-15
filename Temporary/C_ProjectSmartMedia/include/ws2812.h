/*----------------------------------------------------------------------------   
 * 项目名称:

     RGB LED驱动程序

 * 功能描述:

     1、将整个数组的数据发送得到LED进行显示
	 2、从指定像素开始将整个数组的数据发送到LED显示
	 3、发送复位信号

 * 版权信息:

     (c) 飞翼电子, 2014.

 * 历史版本:
     2014-03-15:
       - 初始版本 V1.0.0;

 * 配置说明:
     MCU:             STC15W204S
     晶振:      	  内部晶振：33MHz
     扩展模块:  	  -
     软件:            Keil.C51.V9.01

 * 备注:

------------------------------------------------------------------------------*/

#ifndef _WS2812_H_
#define _WS2812_H_

#include "gpio.h"


#define SNUM 12
/*--------------------------------函数声明------------------------------------*/
void IO_Init(void);						//IO端口初始化
void SendOnePix(unsigned char *ptr);	//发送一个LED的24bit数据
void ResetDataFlow(void);				//芯片复位脉冲，为发送下一帧做准备
void SendOneFrame(unsigned char *ptr);	//发送一帧数据
void SendSameColor(unsigned char *ptr,unsigned char cnt);  //相同颜色发送若干次
void SendOneFrameFrom(unsigned char i,unsigned char *ptr);//从第i个像素开始发送
														  //一帧数据
void SendOneFrameSince(unsigned char i,unsigned char *ptr);//从第i个像素点的数据
void Switch12LED(int flag);

#endif

//-------------------------THE END------------------------*/

