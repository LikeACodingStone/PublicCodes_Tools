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

#include "ws2812.h"
#include <time.h>

/**********************
*R16 led_gpio_set func us 260ns
*1 times noop us 6.3ns
**********************/
#define T1H 70    //700ns 70=(700-260)/6.3
#define T1L 54    //600ns 54=(600-260)/6.3
#define T0H 14     //350ns
#define T0L 86     //800ns
#define RES 10000  // 63us >50us 10000=63000/6.3
//-------------------------------------------------------------------------------
//子程序名称:IO_Init(void)
//功能：端口初始化
//-------------------------------------------------------------------------------
void IO_Init(void)
{
	;
}

//-------------------------------------------------------------------------------
//子程序名称:ResetDateFlow(void)
//功能：复位，为下一次发送做准备，
//说明：将DI置位为0后，延时约65us
//-------------------------------------------------------------------------------
void ResetDataFlow(void)
{
	unsigned int i,j;
  clock_t start,end;
  ZZF_DEBG;
	led_gpio_set(0);					//DI置为0后，延时50us以上，实现帧复位
  ZZF_DEBG;
#if 1
  start=clock(); 
  for(i=0;i<RES;i++){
    ;//>50us
  }
  end=clock();
  printf("%s:use time:%ld\n",__func__,end-start);

#else
	for(i=0;i<15;i++)		//此处33Mhz时延时65us
	{
		for(j=0;j<20;j++)
		{
			;
		}
	}
#endif
}
//-------------------------------------------------------------------------------
//子程序名称:SendOnePix(unsigned char *ptr)
//功能：发送一个像素点的24bit数据
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的连续的三个Byte的数据发送
//说明：在主函数中直接调用此函数时，在整个帧发送开始前需要先执行 ResetDataFlow()
//		数据是按归零码的方式发送，速率为800KBPS
//-------------------------------------------------------------------------------
void SendOnePix(unsigned char *ptr)
{
	unsigned char i,j;
  int k;
	unsigned char temp;
  clock_t start,end;

	for(j=0;j<3;j++)
	{
		temp=ptr[j];
		for(i=0;i<8;i++)
		{
			if(temp&0x80)		 //从高位开始发送
			{
				//DI=1;			 //发送“1”码
        led_gpio_set(1);
        for(k=0;k<T1H;k++)
          ;//700ns

        led_gpio_set(0);
        for(k=0;k<T1L;k++)
          ;//600ns
			}
			else				//发送“0”码
			{
				//DI=1;
        led_gpio_set(1);
        for(k=0;k<T0H;k++)
          ;//350ns

				//DI=0;
        led_gpio_set(0);
        for(k=0;k<T0L;k++)
          ;//600ns
			}
			temp=(temp<<1);		 //左移位
		}
	}
}
//-------------------------------------------------------------------------------
//子程序名称:SendOneFrame(unsigned char *ptr)
//功能：发送一帧数据（即发送整个数组的数据）
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的整个数组的数据发送
//-------------------------------------------------------------------------------
void SendOneFrame(unsigned char *ptr)
{
	unsigned char k;

  ZZF_DEBG;
	ResetDataFlow();				 //发送帧复位信号

  ZZF_DEBG;
	for(k=0;k<SNUM;k++)				 //发送一帧数据，SNUM是板子LED的个数
	{
  ZZF_DEBG;
		SendOnePix(&ptr[(3*k)]);
  ZZF_DEBG;
	}

	ResetDataFlow();				 //发送帧复位信号
}

//-------------------------------------------------------------------------------
//子程序名称:SendSameColor(unsigned char *ptr,unsigned char cnt)
//功能：相同颜色的点发送cnt次
//参数：接收的参数是一个指针，指向像素点颜色数组，cnt传递发送个数
//-------------------------------------------------------------------------------
void SendSameColor(unsigned char *ptr,unsigned char cnt)
{
	unsigned char k;

	ResetDataFlow();				 //发送帧复位信号

	for(k=0;k<cnt;k++)				 //发送一帧数据，SNUM是板子LED的个数
	{
		SendOnePix(&ptr[0]);
	}

	ResetDataFlow();				 //发送帧复位信号
}
//-------------------------------------------------------------------------------
//子程序名称:SendOneFrameFrom(unsigned char i,unsigned char *ptr)
//功能：从指定的像素点开始发送一帧数据（即发送整个数组的数据）
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的整帧数据发送
//		i:把数组的第0个像素数据发送到第i个像素点（第0个像素是板上标号为01的像素）
//说明：即原本对应第一个像素的数据会发送到第i个像素点（LED）上
//-------------------------------------------------------------------------------
void SendOneFrameFrom(unsigned char i,unsigned char *ptr)
{
	unsigned char k;

	ResetDataFlow();				 //发送帧复位信号

   	for(k=(SNUM-i);k<SNUM;k++)		 //发送一帧数据
	{
		SendOnePix(&ptr[(3*k)]);
	}
	for(k=0;k<(SNUM-i);k++)
	{
		SendOnePix(&ptr[(3*k)]);
	}

	ResetDataFlow();				 //发送帧复位信号
}

//-------------------------------------------------------------------------------
//子程序名称:SendOneFrameSince(unsigned char i,unsigned char *ptr)
//功能：从第i个像素点的数据开始发送一帧数据（即发送整个数组的数据）
//参数：接收的参数是一个指针，此函数会将此指针指向的地址的整帧数据发送
//		i:把数组的第i个像素数据发送到第1个像素点
//说明：即原本对应第i像素的数据会发送到第1个像素点（LED）上，第i+1个像素点的数据
//		发送到第2个像素上
//-------------------------------------------------------------------------------
void SendOneFrameSince(unsigned char i,unsigned char *ptr)
{
	unsigned char k;

	ResetDataFlow();				 //发送帧复位信号

	for(k=i;k<SNUM;k++)				 //发送一帧数据
	{
		SendOnePix(&ptr[(3*k)]);
	}
	for(k=0;k<i;k++)
	{
		SendOnePix(&ptr[(3*k)]);
	}
	ResetDataFlow();				 //发送帧复位信号
}
/**********************************THE END**********************************/ 

void Switch12LED(int flag)
{
    char s[12][3]={{0}};
    int i;
    if(flag == 0){
        for(i=0;i<12;i++){
            s[i][0]=0;
            s[i][1]=0;
            s[i][2]=0;
        }
    }else if(flag == 1){
        for(i=0;i<12;i++){
            s[i][0]=100;
            s[i][1]=100;
            s[i][2]=100;
        }
    }else{
        printf("error: %s parameter: flag is not 0 or 1\n",__func__);
        return;
    }

    SendOneFrame(s);

}
