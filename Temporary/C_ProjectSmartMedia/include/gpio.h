#ifndef _GPIO_H_
#define _GPIO_H_

#if 0
#define ZZF_DEBG printf("ZZF_DEBG %s:%d\n",__func__,__LINE__);
#else
#define ZZF_DEBG
#endif

int led_gpio_init();
int led_gpio_set(int data);

#endif
