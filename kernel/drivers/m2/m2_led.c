/*
 * 7bit-LED Device Driver
 *  Hanback Electronics Co.,ltd
 * File : led.c
 * Date : October,2012
 */ 

#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#define DRIVER_AUTHOR			"hanback"		
#define DRIVER_DESC			"Led program"	

#define LED_NAME  	  		"m2_led"	
#define LED_MODULE_VERSION		"LED V1.0"	

int led_open(struct inode *minode, struct file *mfile) 
{
	int err,i;

	for (i=0; i<7; i++)
	{
		err = gpio_request(EXYNOS4_GPK3(i), "Led");
			
        	if (err)
			printk("led.c failed to request GPK3(%d) \n",i);

		s3c_gpio_setpull(EXYNOS4_GPK3(i), S3C_GPIO_PULL_NONE);
        	gpio_direction_output(EXYNOS4_GPK3(i), 0);
	}

	return 0;
}

int led_release(struct inode *minode, struct file *mfile) 
{	
	int i;
	for (i=0; i<7; i++)
		gpio_free(EXYNOS4_GPK3(i));

	return 0;
}

static int led_ioctl(struct inode *inode, unsigned char cmd[], size_t length, loff_t *off_what)
{
	gpio_direction_output(EXYNOS4_GPK3(0), (unsigned int) cmd[0]);
	gpio_direction_output(EXYNOS4_GPK3(1), (unsigned int) cmd[1]);
	gpio_direction_output(EXYNOS4_GPK3(2), (unsigned int) cmd[2]);
	gpio_direction_output(EXYNOS4_GPK3(3), (unsigned int) cmd[3]);
	gpio_direction_output(EXYNOS4_GPK3(4), (unsigned int) cmd[4]);
	gpio_direction_output(EXYNOS4_GPK3(5), (unsigned int) cmd[5]);
	gpio_direction_output(EXYNOS4_GPK3(6), (unsigned int) cmd[6]);

	return 0;
}

ssize_t led_write_byte(struct file *inode, unsigned char *gdata, size_t length, loff_t *off_what) 
{
	int ret;
	unsigned char buf[8];
	ret = copy_from_user(buf,gdata,length);
	
	gpio_direction_output(EXYNOS4_GPK3(0), (unsigned int) buf[0]);
	gpio_direction_output(EXYNOS4_GPK3(1), (unsigned int) buf[1]);
	gpio_direction_output(EXYNOS4_GPK3(2), (unsigned int) buf[2]);
	gpio_direction_output(EXYNOS4_GPK3(3), (unsigned int) buf[3]);
	gpio_direction_output(EXYNOS4_GPK3(4), (unsigned int) buf[4]);
	gpio_direction_output(EXYNOS4_GPK3(5), (unsigned int) buf[5]);
	gpio_direction_output(EXYNOS4_GPK3(6), (unsigned int) buf[6]);
	
	return length;
}

static struct file_operations led_fops = {
	.owner		= THIS_MODULE, 
	.open		= led_open,
	.unlocked_ioctl	= led_ioctl,
	.write		= led_write_byte,
	.release	= led_release,
};

static struct miscdevice led_driver = {
	.fops	= &led_fops,
	.name = LED_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int led_init(void) {
	
	printk("driver: %s DRIVER INIT\n",LED_NAME);
	return misc_register(&led_driver);
}

void led_exit(void) 
{
	misc_deregister(&led_driver);
        printk("driver: %s DRIVER EXIT\n",LED_NAME);
}

module_init(led_init);	
module_exit(led_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_LICENSE("Dual BSD/GPL");	 
