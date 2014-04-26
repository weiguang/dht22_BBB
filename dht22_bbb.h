/*
 * =====================================================================================
 *
 *       Filename:  dht22_bbb.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年04月25日 10时25分05秒
 *       Revision:  none
 *       Compiler:  gcc
 *	   Modify:  none, 2014年04月25日
 *         Author:  Jam (), chen2621978@163.com
 *   Organization:  SCAU
 *
 * =====================================================================================
 */
#ifndef DHT22_H
#define DHT22_H

#define GPIO(n,m) (32*n+m) //gpio转换宏

#define DHT_DATA GPIO(1,17)
#define DHTDATAGPIONAME "gpio1_17"
//DHT_DATA 
#define DHT_DATA_HIGH gpio_direction_output(DHT_DATA, 1);
#define DHT_DATA_LOW gpio_direction_output(DHT_DATA, 0);  /*   */



//DATA DIRECTION
#define DHT_DATA_OUT0 gpio_direction_output(DHT_DATA, 0);   /*   */
#define DHT_DATA_OUT1 gpio_direction_output(DHT_DATA, 1);   /*   */
#define DHT_DATA_IN gpio_direction_input(DHT_DATA);
//DATA GET VALUE
#define DHT_GET_VALUE gpio_get_value(DHT_DATA)
int dht_start(void);
volatile unsigned char dht_read_byte(void);
int dht_read_temperature(unsigned char *rbuf, int size);
#endif
