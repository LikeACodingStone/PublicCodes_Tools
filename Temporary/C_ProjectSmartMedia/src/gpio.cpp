#include <stdio.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "gpio.h"

#define uint32_t unsigned int

#define GPIO_CONF_ARG_DDR 0x01c2086c
#define GPIO_CONF_ARG_DATA 0x77771177
#define GPIO_DATA_ARG_DDR 0x01c2087c
#define GPIO_DATA_BIT 2


void *virt_addr;
unsigned width = 8 * sizeof(int);

int xopen3(const char *pathname, int flags, int mode)
{
    int ret;

    ret = open(pathname, flags, mode);
    if (ret < 0) {
        printf("can't open '%s'", pathname);
    }
    return ret;
}

int xopen(const char *pathname, int flags)
{
    return xopen3(pathname, flags, 0666);
}

	void *virt_addr;
int led_gpio_init()
{
  ZZF_DEBG;
	void *map_base;
	off_t target = GPIO_CONF_ARG_DDR;
  uint32_t writeval = GPIO_CONF_ARG_DATA;
  unsigned width=32;
	unsigned page_size, mapped_size, offset_in_page;
	int fd;
	fd = xopen("/dev/mem", O_RDWR | O_SYNC);
	mapped_size = page_size = getpagesize();
	offset_in_page = (unsigned)target & (page_size - 1);
	if (offset_in_page + width > page_size) {
		/* This access spans pages.
		 * Must map two pages to make it possible: */
		mapped_size *= 2;
	}
	map_base = mmap(NULL,
			mapped_size,
			PROT_READ | PROT_WRITE,
			MAP_SHARED,
			fd,
			target & ~(off_t)(page_size - 1));
	if (map_base == MAP_FAILED)
		printf("mmap failed");

	  virt_addr = (char*)map_base + offset_in_page;
	  *(volatile uint32_t*)virt_addr = writeval;

  /////////////////////////////

	target = GPIO_DATA_ARG_DDR;
	offset_in_page = (unsigned)target & (page_size - 1);
	if (offset_in_page + width > page_size) {
		/* This access spans pages.
		 * Must map two pages to make it possible: */
		mapped_size *= 2;
	}
	  virt_addr = (char*)map_base + offset_in_page;
    return 0;

}

/********************
 *arg　data:    0
 *              1
 ********************/
int led_gpio_set(int data)
{
  uint32_t writeval;
    writeval = *(volatile uint32_t*)virt_addr;
  // printf("old writeval = 0x%x\n",writeval);
    if(data == 0){
        *(volatile uint32_t*)virt_addr = writeval & ~(0x1<<GPIO_DATA_BIT) ;
    }else if(data == 1){
        *(volatile uint32_t*)virt_addr = writeval | (0x1<<GPIO_DATA_BIT) ;
    }else{
        printf("data is not 0 or 1!\n");
        return -1;
    }
   // writeval = *(volatile uint32_t*)virt_addr;
//    printf("new writeval = 0x%x\n",writeval);
  return 0;
}
