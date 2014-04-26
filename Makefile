
ifneq ($(KERNELRELEASE),)
obj-m :=dht22.c dht22.o

else
	#KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	#KERNELDIR ?= /lib/modules/2.6.12-h1940/build
	KERNELDIR ?= /lib/modules/3.8.6/build
	PWD := $(shell pwd)
	CROSS_COMPILE=/usr/local/arm/4.5.1/bin/arm-none-linux-gnueabi- 
	ARCH=arm 
	
default:
	$(MAKE) -C $(KERNELDIR) ARCH=arm  CROSS_COMPILE=/usr/local/arm/4.5.1/bin/arm-none-linux-gnueabi-   M=$(PWD) modules
endif

clean:
	rm -f *.ko *.order *.symvers *.mod.* *.o  2>/dev/null

