/*
 * =====================================================================================
 *
 *       Filename:  dht22_bbb.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年04月25日 10时38分36秒
 *       Revision:  none
 *       Compiler:  gcc
 *	   Modify:  none, 2014年04月25日
 *         Author:  Jam (), chen2621978@163.com
 *   Organization:  SCAU
 *
 * =====================================================================================
 */
#include <linux/delay.h>
//#include <sched.h>
#include <linux/gpio.h>
#include "dht22_bbb.h"


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dht_start
 *  Description:  
 * =====================================================================================
 */
	int 
dht_start ( )
{
	int count = 0;
	DHT_DATA_LOW;
	mdelay(18);
	DHT_DATA_HIGH;
	udelay(40);
	// check ack
	DHT_DATA_IN;
	if (DHT_GET_VALUE)
	{
		//no ack
		return -1;
	}
	while (!DHT_GET_VALUE && count++ <=9)
	{
		udelay(10);
	}
	//check timeou
	if (count > 9)
	{
		return -1;
	}
	count = 0;
	while (DHT_GET_VALUE && count++ <= 9)
	{
		udelay(10);
	}
	if (count > 9) return -1;
	return 1;
}		/* -----  end of function dht_start  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dht_read_byte
 *  Description:  
 * =====================================================================================
 */
volatile  unsigned char dht_read_byte(void)
{
        volatile  unsigned char count,value = 0;
        int i =0, m = 0;
	DHT_DATA_IN;
        for (i = 0 ;i < 8 ; i++)
        {
                value = value << 1;
                count = 0;                
		//delay 50us
                while (!DHT_GET_VALUE && count++ <= 6)
		{
			udelay(10);
		}
		//printk("delay 50s later, count=%d, data= %d\n", 
		//count, DHT_GET_VALUE);
		// 20-28us high is 0, 70us high is 1
                udelay(30);		
                m = DHT_GET_VALUE;
		if (m)
		{
			// value = 1
			value++;
			// wait other 40 us
			count = 0;
			while (DHT_GET_VALUE && count++ <= 5)
			{
				udelay(10);
			}
		}
	//printk("%d ", m);       
	}
	//printk(" value = %x\n", value);
	return value;
}		/* -----  end of function dht_read_byte  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  dht_read_temperature
 *  Description:  
 *  	 return:  the byte of read:success; ; -1:crc error
 * =====================================================================================
 */
	int
dht_read_temperature ( unsigned char *rbuf, int size)
{
	int i = 0, crc = 0;
	//DHT_DATA_IN;
	for (i = 0; i < size; i++)
	{
		rbuf[i] = dht_read_byte();
		//if (rbuf[i] == 255 ) return i;
		printk("rbuf[%d]=%d ", i, rbuf[i]);
		if (i != size - 1)	crc+=rbuf[i];
	}
	//check crc
	if (rbuf[size-1] != crc) return -1;
	printk("crc = %d\n", crc);
	DHT_DATA_HIGH;
	return i;
}		/* -----  end of function dht_read_temperature  ----- */
