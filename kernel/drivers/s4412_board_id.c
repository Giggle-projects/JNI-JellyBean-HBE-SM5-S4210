/*
 * SM7 Serial ID Device Driver
 *  Hanback Electronics Co.,ltd
 * File : s4412_board_id.c
 * Date : October,2012
 */ 


#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/mm.h>
#include <linux/list.h>
#include <asm/io.h>
#include <asm/uaccess.h>
#include <asm/cacheflush.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <mach/regs-gpio.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <plat/gpio-cfg.h>


#define DRIVER_AUTHOR			"hanback"		
#define DRIVER_DESC			"Led program"	

#define SID_NAME  	  		"serial_id"	
#define SID_MODULE_VERSION		"serial id V1.0"	

static unsigned char serial_id[8];

int sid_open(struct inode *minode, struct file *mfile) 
{
	int cnt = 0,i,j,err;
	unsigned char data = 0x33,read_data = 0;

        err = gpio_request(EXYNOS4_GPK1(1), "GPK1");
        if(err)
                printk(KERN_ERR "s4412_board_id.c failed to request GPK1(1) \n");

	s3c_gpio_setpull(EXYNOS4_GPK1(1), S3C_GPIO_PULL_NONE);

//init	
#if 1
Reset:
	gpio_direction_output(EXYNOS4_GPK1(1),1);
	udelay(20);
	gpio_direction_output(EXYNOS4_GPK1(1),0);
	udelay(500);
	gpio_direction_output(EXYNOS4_GPK1(1),1);
	udelay(80);
	while(gpio_get_value(EXYNOS4_GPK1(1)) && cnt< 30)
	{
		cnt++;
		udelay(400);
		goto Reset;
	}

	if(cnt < 30)
	{
		udelay(500);
		for(i=0;i<8;i++)
		{
			gpio_direction_output(EXYNOS4_GPK1(1),0);
			udelay(1);
		#if 0
			if(data&0x1)
			{
				gpio_direction_output(EXYNOS4_GPK1(1),1);
				//gpio_direction_input(EXYNOS4_GPK1(1));
			}
			udelay(70);
			gpio_direction_output(EXYNOS4_GPK1(1),1);
			//gpio_direction_input(EXYNOS4_GPK1(1));
			udelay(2);
		#endif
		#if 1
			udelay(1);
			if(data & 0x1)
			{
				//udelay(1);//10
				gpio_direction_output(EXYNOS4_GPK1(1),1);
				//gpio_direction_input(EXYNOS4_GPK1(1));
				udelay(70);//80
			}
			else
			{
				udelay(70);//80
				gpio_direction_output(EXYNOS4_GPK1(1),1);
				//gpio_direction_input(EXYNOS4_GPK1(1));
				//udelay(10);
			}
			udelay(2);
		#endif
			data >>= 1;
		}
		for(j=0;j<8;j++)
		{
			for(i=0;i<8;i++)
			{
				read_data >>= 1;
				gpio_direction_output(EXYNOS4_GPK1(1),0);
				udelay(1);
				gpio_direction_output(EXYNOS4_GPK1(1),1);
				gpio_direction_input(EXYNOS4_GPK1(1));
				udelay(10);
				if(gpio_get_value(EXYNOS4_GPK1(1)))
				{
					read_data |= 0x80;
				}
				udelay(60);
			}
			serial_id[j] = read_data;
		}
		for(i=0;i<8;i++)
			printk(KERN_INFO "Debug --- %x \n",serial_id[i]);
	}
#endif
	return 0;
}

int sid_release(struct inode *minode, struct file *mfile) 
{
	gpio_free(EXYNOS4_GPK1(1));	
	return 0;
}

static struct file_operations sid_fops = {
	.owner	= THIS_MODULE, 
	.open	= sid_open,
	.release= sid_release,
};

static struct miscdevice sid_driver = {
	.fops	= &sid_fops,
	.name	= SID_NAME,
	.minor	= MISC_DYNAMIC_MINOR,
};

int sid_init(void) {
#if 0
	int err;
	
	err = gpio_request(EXYNOS4_GPK1(1), "GPK1");
	if(err)
		printk(KERN_ERR "s4412_board_id.c failed to request GPK1(1) \n");
	else
	{
		s3c_gpio_setpull(EXYNOS4_GPK1(1), S3C_GPIO_PULL_NONE);
		s3c_gpio_cfgpin(EXYNOS4_GPK1(1), S3C_GPIO_INPUT);
		gpio_direction_input(EXYNOS4_GPK1(1));
	}
#endif	
	printk("driver: %s DRIVER INIT\n",SID_NAME);
	return misc_register(&sid_driver);
}

void sid_exit(void) 
{
#if 0
	gpio_free(EXYNOS4_GPK1(1));
#endif
	misc_deregister(&sid_driver);
        printk("driver: %s DRIVER EXIT\n",SID_NAME);
}

module_init(sid_init);	
module_exit(sid_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_LICENSE("Dual BSD/GPL");	 
