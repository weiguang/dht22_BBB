/*
 * =====================================================================================
 *
 *       Filename:  test_dht.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年04月25日 13时04分48秒
 *       Revision:  none
 *       Compiler:  arm-linux-gcc 4.5.1 
 *	   Modify:  none, 2014年04月25日
 *         Author:  Jam (), chen2621978@163.com
 *   Organization:  SCAU
 *
 * =====================================================================================
 */
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>
#include <asm/ioctl.h>
#include <string.h>

#define MY_IOC_MAGIC 'k' //幻数
#define MY_IOC_MAXNR 5
#define LED_OPEN _IO(MY_IOC_MAGIC, 1)  //定义命令
#define LED_CLOSE _IO(MY_IOC_MAGIC, 2)
#define FAN_OPEN _IO(MY_IOC_MAGIC, 3)
#define FAN_CLOSE _IO(MY_IOC_MAGIC, 4)
#define GET_RH _IOR(MY_IOC_MAGIC, 5, int)

#define FILENAME "/dev/dht220"
	int
main ( int argc, char *argv[] )
{
	int fd = 0, ret = 0, i=0;
	char s[20];
	char rbuf[5]={0};
	fd = open(FILENAME, O_RDWR);
	printf("fd = %d\n", fd);
	while(scanf("%s", s))
	{
		if(s[0] == '0') 
		{
			ret = ioctl(fd, LED_CLOSE);
			printf("Close led! ret = %d\n", ret);
		}
		else if(s[0] == '1') 
		{
			ioctl(fd, LED_OPEN);
			printf("Open led! ret = %d\n", ret);
		}
		else if(s[0] == '2')
                {
                        ret = ioctl(fd, FAN_CLOSE);
                        printf("Close fan! ret = %d\n", ret);
                }
                else if(s[0] == '3')
                {
                        ioctl(fd, FAN_OPEN);
                        printf("Open fan! ret = %d\n", ret);
                }
		//ioctl for read temperature and humidity
		else if (s[0] == 'i')
		{
			
			ret = ioctl(fd, GET_RH, rbuf);
			// driver has checked the CRC
			if (ret == -1) printf("CRC error!");
			//ret = 0, copy to user success.
                        else if (ret && ret != 5) 
				{printf("read err! ret = %d !\n", ret);}
			//rbuf[0] and rbuf[1] is humidity
			//rbuf[2] and rbuf[3] is temperature
			//rbuf[4] is crc. rbuf[4] = rbuf[0]+rbuf[1] + rbuf[2]+rbuf[3]  
                         double shidu = (rbuf[0] * 256 + rbuf[1]) * 0.1 ;
                         double  temperature = (rbuf[2] * 256 + rbuf[3]) * 0.1;
                         printf("shidu:%.1f\%, temperature: %.1f\n",
				shidu, temperature);
		}
		//read temperature and humidity
		else if (s[0] == 'r')
		{
			memset(rbuf, 0 , sizeof(rbuf));
			ret = read(fd, rbuf, 5);
			// driver has checked the CRC
			if (ret == -1) printf("CRC error!");
			else if (ret != 5) 
				{printf("read err! ret = %d !\n", ret);}
			//rbuf[0] and rbuf[1] is humidity
			//rbuf[2] and rbuf[3] is temperature
			//rbuf[4] is crc. rbuf[4] = rbuf[0]+rbuf[1] + rbuf[2]+rbuf[3]  
			double shidu = (rbuf[0] * 256 + rbuf[1]) * 0.1 ;
			double  temperature = (rbuf[2] * 256 + rbuf[3]) * 0.1;
			printf("shidu:%.1f\%, temperature: %.1f\n",shidu, temperature);
		}
		else if (s[0] == 'w')
		{
			;
		}
	 	else //(s[0] == 'c')
		 break; 
	}
	close(fd);
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */

