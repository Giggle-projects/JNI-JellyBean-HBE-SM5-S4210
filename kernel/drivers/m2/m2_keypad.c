/*
 * Keypad Device Driver
 *  Hanback Electronics Co.,ltd
 * File : m2_keypad.c
 * Date : October,2012
 */ 

#include <linux/list.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/timer.h>
#include <linux/signal.h>
#include <linux/input.h>
#include <linux/sched.h>

#define DRIVER_AUTHOR			"hanback"		
#define DRIVER_DESC			"Keypad program"	

#define KEYPAD_NAME  	  		"m2_keypad"	
#define KEYPAD_MODULE_VERSION		"KEYPAD V1.0"	
#define TIMER_INTERVAL			20

static int value;
static struct timer_list mytimer;
static struct input_dev *idev;
#if 1
unsigned short keycode[16] = {
	  2,  3,   4,  127, // 1 , 2 , 3 , search
	  5,  6,   7,  158, // 4 , 5 , 6 , back
	  8,  9,  10,  102, // 7 , 8 , 9 , home 
	227, 11, 228,  229, // * , 0 , # , menu
};
#else
unsigned char keycode[16] = {
          2,  3,   4,  13, // 1 , 2 , 3 , search
          5,  6,   7,  14, // 4 , 5 , 6 , back
          8,  9,  10,  15, // 7 , 8 , 9 , home 
          1, 11,  12,  16, // * , 0 , # , menu
};
#endif

void mypollingfunction(unsigned long data);

int keypad_open(struct inode *minode, struct file *mfile) 
{
	int err,i;

	for (i=0; i<8; i++)
	{
		err = gpio_request(EXYNOS4212_GPM4(i), "keypad");
			
        	if (err)
			printk("keypad.c failed to request GPM4(%d) \n",i);

		s3c_gpio_setpull(EXYNOS4212_GPM4(i), S3C_GPIO_PULL_NONE);
		if(i>3)
			s3c_gpio_cfgpin(EXYNOS4212_GPM4(i), S3C_GPIO_SFN(0));
		else
		{
			s3c_gpio_cfgpin(EXYNOS4212_GPM4(i), S3C_GPIO_SFN(1));
			gpio_direction_output(EXYNOS4212_GPM4(i),0);
		}
	}
	value = 0;
	init_timer(&mytimer);
	mytimer.expires = get_jiffies_64() + TIMER_INTERVAL;
	mytimer.function = &mypollingfunction;
	add_timer(&mytimer);
	return 0;
}

int keypad_release(struct inode *minode, struct file *mfile) 
{	
	int i;
	for (i=0; i<8; i++)
		gpio_free(EXYNOS4212_GPM4(i));

	del_timer(&mytimer);
	return 0;
}

void mypollingfunction(unsigned long data)
{
	int i;
	unsigned int row_val;
	
	for(i=0; i<4; i++) {
		gpio_direction_output(EXYNOS4212_GPM4(i),1);
		if(gpio_get_value(EXYNOS4212_GPM4(4))==(0x01))
			value = keycode[i*4]; 
		if(gpio_get_value(EXYNOS4212_GPM4(5))==(0x01))
			value = keycode[(i*4)+1]; 
		if(gpio_get_value(EXYNOS4212_GPM4(6))==(0x01))
			value = keycode[(i*4)+2]; 
		if(gpio_get_value(EXYNOS4212_GPM4(7))==(0x01))
			value = keycode[(i*4)+3]; 
		if(value != 0) goto stop_poll;
		gpio_direction_output(EXYNOS4212_GPM4(i),0);
	}
	
	stop_poll:
		if(value > 0)
		{
			input_report_key(idev,value,1);
			input_report_key(idev,value,0);
			input_sync(idev);
			row_val = 0;
			value = 0;
			gpio_direction_output(EXYNOS4212_GPM4(i),0);
			mdelay(100);
		}
		mytimer.expires = get_jiffies_64() + TIMER_INTERVAL;
		add_timer(&mytimer);
}

ssize_t keypad_read(struct file *inode, char *gdata, size_t length, loff_t *off_what)
{
	int ret;
	ret = copy_to_user(gdata, &value, 1);
	if(ret<0)
		return -1;
	return length;
}

int keypad_init(void) {
	int i=0,ret;
	
	idev = input_allocate_device();
	set_bit(EV_KEY,idev->evbit);
	set_bit(EV_KEY,idev->keybit);

	for(i = 0; i < 30; i++)
		set_bit(i & KEY_MAX,idev->keybit);

	idev->name              = "keypad";
	idev->id.vendor         = 0x1002;
	idev->id.product        = 0x1002;
	idev->id.bustype        = BUS_HOST;
	idev->phys              = "keypad/input";
	idev->open              = keypad_open;
	idev->close             = keypad_release;

	ret = input_register_device(idev);

	if(ret < 0) {
		printk(KERN_WARNING"Can't get any major\n");
		return ret;
	}

	printk("driver: %s DRIVER INIT\n",KEYPAD_NAME);
	return 0;
}

void keypad_exit(void) {
        printk("driver: %s DRIVER EXIT\n",KEYPAD_NAME);
	input_unregister_device(idev);
}

module_init(keypad_init);	
module_exit(keypad_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_LICENSE("Dual BSD/GPL");	 
