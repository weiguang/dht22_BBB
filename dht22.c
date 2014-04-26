/*
 * =====================================================================================
 *
 *       Filename:  gpio_driver.c
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2014年03月28日 12时37分59秒
 *       Revision:  none
 *       Compiler:  arm-linux-gcc 4.5.1
 *	   Modify:  none, 2014年03月28日
 *         Author:  Jam (), chen2621978@163.com
 *   Organization:  SCAU
 *
 * =====================================================================================
 */

#include<linux/kernel.h>	//printk
#include<linux/init.h>
#include<linux/module.h>	//build module
#include<linux/sched.h>	//struct task_struct
#include<linux/types.h>	//dev_t
#include<linux/fs.h>	//register_chrdev_region
#include<linux/kdev_t.h>	//MAJOR,MINOR
#include<linux/errno.h>	//err
#include<linux/cdev.h>	//cdev
//#include<linux/device.h>
//#include<linux/mm.h>
#include<linux/slab.h>// kmalloc
#include <linux/gpio.h>
#include <asm/ioctl.h>
#include <asm/uaccess.h>


#include "dht22_bbb.c"

#define MY_IOC_MAGIC 'k' //幻数
#define	MY_IOC_MAXNR 6
#define LED_OPEN _IO(MY_IOC_MAGIC, 1)  //定义命令
#define LED_CLOSE _IO(MY_IOC_MAGIC, 2)
#define FAN_OPEN _IO(MY_IOC_MAGIC, 3)
#define FAN_CLOSE _IO(MY_IOC_MAGIC, 4)
#define GET_RH _IOR(MY_IOC_MAGIC, 5, int)


#define DEV_NAME "dht22"
//#define MAJOR major  //动态分配
#define DEV_NUMBER 1 //支持的设备数
#define BUFFERSIZE 100 //缓存大小


//#define GPIO(n,m) (32*n+m) //gpio转换宏

#define LEDGPIO GPIO(1,28) //LEDGPIO
#define LEDGPIONAME "gpio1_28"

#define FANGPIO GPIO(1,16)	//FANGPIO
#define FANGPIONAME "gpio1_16"


//自定义的设备数据结构
struct my_dev{
        struct cdev cdev; /* cdev*/
        unsigned char data[BUFFERSIZE] ; /* data*/
};


int my_open(struct inode *inode, struct file *file);
int my_release(struct inode *inode, struct file * file);
long my_ioctl (struct file *filp, unsigned int cmd, unsigned long arg);
ssize_t my_read (struct file *filp, char __user *rbuf, size_t count, loff_t *f_pos);
ssize_t my_write (struct file * filp, const char __user  *wbuf, size_t count, loff_t *f_pos);

//文件操作函数结构体
struct file_operations my_fops = {
	.owner = THIS_MODULE,
	.open = my_open,
	.release = my_release,
	.read = my_read,
	.write = my_write,
	.unlocked_ioctl = my_ioctl,
};

//全局变量
struct my_dev *my_cdevp;
dev_t dev;
int major=0; //动态分配

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  my_open
 *  Description:  
 * =====================================================================================
 */
int my_open(struct inode *inode, struct file *file)
{
	int ret = 0;
	//printk("gpio open!\n");
	
	ret = gpio_request(LEDGPIO, LEDGPIONAME);
	if (ret < 0)
	{
		printk("gpio %d reguest false: %d", LEDGPIO, ret);
		return ret;
	}
	ret = gpio_request(FANGPIO, FANGPIONAME);
        if (ret < 0)
        {
                printk("gpio reguest false: %d", ret);
                return ret;
        }
	ret = gpio_request(DHT_DATA, DHTDATAGPIONAME);
        if (ret < 0)
        {
                printk("gpio %s  reguest false: %d", DHTDATAGPIONAME, ret);
                return ret;
        }
        ret = gpio_direction_output(DHT_DATA, 0);
	ret = gpio_direction_output(FANGPIO, 0);
	//printk("gpio open reguest : %d\n", ret);
	ret = gpio_direction_output(LEDGPIO, 1);
	return ret;
}		/* -----yy  end of function my_open  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  my_release
 *  Description:  
 * =====================================================================================
 */
int my_release(struct inode *inode, struct file *file)
{
	gpio_free(LEDGPIO);
	gpio_free(FANGPIO);
	gpio_free(DHT_DATA);
	printk(KERN_NOTICE "gpio close!\n");
	return 0;
}		/* -----  end of function my_release  ----- */
 


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  my_read
 *  Description:  
 * =====================================================================================
 */
ssize_t my_read (struct file *filp, char __user *rbuf, size_t count, loff_t *f_pos)
{
	char krbuf[30] = {0};
	char kwbuf[10] = {0x03, 0x00, 0x04};
	int ret = 0;
	if (dht_start() == -1)
	{
		 printk("Start error!");
	}
	ret = dht_read_temperature(krbuf, 5);
	if (ret != 5) 
	{
		printk("read %d byte error! \n", ret); 
		return ret;
	}
	printk("read ret = %d ! | %d, %d, %d, %d |\n", 
		ret, krbuf[0], krbuf[1], krbuf[2], krbuf[3]);
	//krbuf[0] = krbuf[0] * 256 + krbuf[1];
	//krbuf[1] = krbuf[2] * 256 + krbuf[3];  
	if ((ret = copy_to_user(rbuf, krbuf, count)) != 0) 
	{
		printk("copy_to_user error! ret = %d !\n", ret);
		return -EFAULT;
	}
	//printk("read return ret = %d!\n", ret);
	//mdelay(500);
	return count;
}		/* -----  end of function my_read  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  my_write
 *  Description:  
 * =====================================================================================
 */
ssize_t my_write (struct file * filp, const char __user *wbuf, size_t count, loff_t *f_pos)
{
	char kwbuf[20] = {0};
	int ret = 0;
	printk("write start!\n");
	if ((ret = copy_from_user(kwbuf, wbuf, count)))
	{
		printk("copy_from_user error! ret = %d !\n", ret);
                return -EFAULT;
	}
	printk("write: %s, ret = %d !\n", kwbuf, ret);
	mdelay(1);
	return ret;	 
}		/* -----  end of function my_write  ----- */

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  my_ioctl
 *  Description:  
 * =====================================================================================
 */
long my_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	char krbuf[10] = {0};
	//printk("My ioctl!\n");
	if (_IOC_TYPE(cmd) != MY_IOC_MAGIC) return -EINVAL;
	if (_IOC_NR(cmd) > MY_IOC_MAXNR) return -EINVAL;
	switch(cmd)
	{
		case LED_OPEN :
		ret = gpio_direction_output(LEDGPIO, 1);
		break;
		case LED_CLOSE :
		ret = gpio_direction_output(LEDGPIO, 0);
		break;
		case FAN_OPEN :           
                ret = gpio_direction_output(FANGPIO, 1);
                break;   
                case FAN_CLOSE :          
                ret = gpio_direction_output(FANGPIO, 0);
                break;
		case GET_RH:
       			if (dht_start() == -1)
        		{
                		 printk("Start error!");
       			}
        		ret = dht_read_temperature(krbuf, 5);
        		if (ret != 5)
        		{
                		printk("read %d byte error! \n", ret);
                		return ret;
        		}
        		printk("read ret = %d ! | %d, %d, %d, %d |\n",
                	ret, krbuf[0], krbuf[1], krbuf[2], krbuf[3]);
                        if ((ret = copy_to_user((unsigned char*)arg, krbuf,5)) != 0)
                        {
                       		printk("copy_to_user error! ret = %d !\n", ret);
                        	return -EFAULT;
                        }
		//mdelay(500);
		break;
		default:
		printk( "ioctl range error!\n");	
	}
	return ret;
}		/* -----  end of function my_ioctl  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  my_step_cdev
 *  Description:  
 * 		初始化并注册cdev,就是注册我们自己的字符设备
 * =====================================================================================
 */
static int my_step_cdev(struct my_dev * idev, int index)
{
	int err;
	//printk("my_step!!!\n");
	dev_t  devno = MKDEV(major, index);
	cdev_init(&idev->cdev, &my_fops);
	idev->cdev.owner = THIS_MODULE;
	idev->cdev.ops = &my_fops;/* */
	err = cdev_add(&idev->cdev, devno, DEV_NUMBER);
	if (err)
	printk(KERN_NOTICE " Error %d adding acull%d", err, index);
	return err;
}		/* -----  end of function my_step_cdev  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  char_dev_init
 *  Description:  在注册设备时，如果major是0，表示动态分配，否则静态分配。
 * =====================================================================================
 */

static int __init char_dev_init(void)
{
	int err = 0;
        printk(KERN_ALERT "Start! pid=%d,cmane=%s\n",
		current->pid,current->comm);
	//注册设备
	if (major)
	{	
		dev = MKDEV(major,0); 
		err = register_chrdev_region(dev, DEV_NUMBER, DEV_NAME);
	}
	else 
	{
		err = alloc_chrdev_region(&dev, 0, DEV_NUMBER, DEV_NAME);
		major = MAJOR(dev);
	}
	if (err < 0)
	{
		printk(KERN_ALERT "%s: Cant not get major %d\n",DEV_NAME,
		major);
		return err;
	}
	else
	{
		printk(KERN_ALERT "Device name = %s, major = %d, minor = %d\n",
			DEV_NAME, MAJOR(dev), MINOR(dev));	
	}
	//分配空间
	my_cdevp = kmalloc(sizeof(struct my_dev), GFP_KERNEL);
	if (!my_cdevp)
	{
		err = -ENOMEM;
		goto fail_malloc;
	}
	memset(my_cdevp, 0, sizeof(struct my_dev));
	my_step_cdev(my_cdevp, 0);

        return 0;

fail_malloc:
	unregister_chrdev_region(dev, 1);
	return err;

}		/* -----  end of function char_dev_init  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  char_dev_exit
 *  Description:  
 * =====================================================================================
 */
static void __exit char_dev_exit(void)
{	
	cdev_del(&my_cdevp->cdev);
	kfree(my_cdevp);
	printk(KERN_ALERT "%s: unregister now ! major : %d\n", DEV_NAME, major);
	unregister_chrdev_region(dev, DEV_NUMBER);        
}	/* -----  end of function char_dev_exit  ----- */

MODULE_LICENSE("Dual BSD/GPL");	//license
MODULE_AUTHOR("Jam");	//AURHOR
module_init(char_dev_init);
module_exit(char_dev_exit);


