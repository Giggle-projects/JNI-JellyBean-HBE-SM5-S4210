/*
 * 7-Segment Device Driver
 * Hanback Electronics Co.,ltd
 * File : m2_segment.c
 */ 

#include <linux/miscdevice.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/delay.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>

#define DRIVER_AUTHOR		"hanback"		
#define DRIVER_DESC		"7-Segment program"

#define	SEGMENT_NAME		"m2_segment"
#define SEGMENT_MODULE_VERSION	"SEGMENT V1.0"

unsigned int ibuf[8];

int seg_open(struct inode *inode, struct file *filp)
{
	int err,i;

	for(i=0;i<8;i++)
	{
		err = gpio_request(EXYNOS4_GPF0(i),"GPF0");
		if(err)
			printk("segment.c failed to request GPF0(%d)\n",i);
		s3c_gpio_setpull(EXYNOS4_GPF0(i), S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPF0(i),0);
	}

	for(i=0;i<6;i++)
	{
		err = gpio_request(EXYNOS4_GPF3(i),"GPF3");
		if(err)
			printk("segment.c failed to request GPF3(%d)\n",i);
		s3c_gpio_setpull(EXYNOS4_GPF3(i), S3C_GPIO_PULL_UP);
		gpio_direction_output(EXYNOS4_GPF3(i),1);
	}
	
	return 0; 
}

int seg_release (struct inode *inode, struct file *filp)
{

	int i;
	
	for(i=0; i<8; i++)
		gpio_free(EXYNOS4_GPF0(i));
	
	for(i=0; i<6; i++)
		gpio_free(EXYNOS4_GPF3(i));
	
	return 0;
}

int Getsegmentcode_base (int x)
{
	unsigned int i;

	for (i = 0; i < 8; i++)
		ibuf[i] = 0;

	switch (x) {
		case 0 : 
			for (i=0; i<6; i++) ibuf[i] = 1;
			break;
			
		case 1 : ibuf[1] = 1; ibuf[2] = 1; break;
		
		case 2 : 
			for (i=0; i<2; i++) ibuf[i] = 1;
			for (i=3; i<5; i++) ibuf[i] = 1;
			ibuf[6] = 1;
			break;
			
		case 3 : 
			for (i=0; i<4; i++) ibuf[i] = 1;
			ibuf[6] = 1;
			break;
			
		case 4 :
			for (i=1; i<3; i++) ibuf[i] = 1;
			for (i=5; i<7; i++) ibuf[i] = 1;
			break;
			
		case 5 : 
			ibuf[0] = 1;
			for (i=2; i<4; i++) ibuf[i] = 1;
			for (i=5; i<7; i++) ibuf[i] = 1;
			break;
			
		case 6 : 
			for (i=2; i<7; i++) ibuf[i] = 1;
			break;
			
		case 7 : 
			for (i=0; i<3; i++) ibuf[i] = 1;
			ibuf[5] = 1;
			break;

		case 8 : 
			for (i=0; i<7; i++) ibuf[i] = 1;
			break;
			
		case 9 : 
			for (i=0; i<4; i++) ibuf[i] = 1;
			for (i=5; i<7; i++) ibuf[i] = 1;
			break;
		
		case 10 :
			for (i=0; i<3; i++) ibuf[i] = 1;
			for (i=4; i<8; i++) ibuf[i] = 1;
			break;
		case 11 :
			for (i=0; i<8; i++) ibuf[i] = 1;
			break;
		case 12 :
			for (i=3; i<6; i++) ibuf[i] = 1;
			ibuf[0] = 1;
			ibuf[7] = 1;
			break;
		case 13 :
			ibuf[7] = 1;
			for (i=0; i<6; i++) ibuf[i] = 1;
			break;
		case 14 :
			for (i=3; i<8; i++) ibuf[i] = 1;
			ibuf[0] = 1;
			break;
		case 15 :
			for (i=4; i<8; i++) ibuf[i] = 1;
			ibuf[0] = 1;
			break;
			
		default : 
			for (i=0; i<8; i++) ibuf[i] = 1;
			break;
	}
	return 0;
}

ssize_t seg_write(struct file *inode, const char *gdata, size_t length, loff_t *off_what)
{
	unsigned int ret,i;
	unsigned char data[6];
	ret=copy_from_user(data,gdata,6);

	Getsegmentcode_base((unsigned int)data[0]);

	gpio_direction_output(EXYNOS4_GPF3(0), 0);
	gpio_direction_output(EXYNOS4_GPF3(1), 1);
	for(i=0;i<8;i++) {
		gpio_direction_output(EXYNOS4_GPF0(i), (unsigned int) ibuf[i]);
	}
	mdelay(5);
	
	Getsegmentcode_base((unsigned int)data[1]);

        gpio_direction_output(EXYNOS4_GPF3(0), 1);
        gpio_direction_output(EXYNOS4_GPF3(1), 0);
        for(i=0;i<8;i++) {
                gpio_direction_output(EXYNOS4_GPF0(i), (unsigned int) ibuf[i]);
        }
        mdelay(5);

	Getsegmentcode_base((unsigned int)data[2]);

        gpio_direction_output(EXYNOS4_GPF3(1), 1);
        gpio_direction_output(EXYNOS4_GPF3(2), 0);
        for(i=0;i<8;i++) {
                gpio_direction_output(EXYNOS4_GPF0(i), (unsigned int) ibuf[i]);
        }
        mdelay(5);

	Getsegmentcode_base((unsigned int)data[3]);

        gpio_direction_output(EXYNOS4_GPF3(2), 1);
        gpio_direction_output(EXYNOS4_GPF3(3), 0);
        for(i=0;i<8;i++) {
                gpio_direction_output(EXYNOS4_GPF0(i), (unsigned int) ibuf[i]);
        }
        mdelay(5);

	Getsegmentcode_base((unsigned int)data[4]);

        gpio_direction_output(EXYNOS4_GPF3(3), 1);
        gpio_direction_output(EXYNOS4_GPF3(4), 0);
        for(i=0;i<8;i++) {
                gpio_direction_output(EXYNOS4_GPF0(i), (unsigned int) ibuf[i]);
        }
        mdelay(5);

	Getsegmentcode_base((unsigned int)data[5]);

	gpio_direction_output(EXYNOS4_GPF3(4), 1);
	gpio_direction_output(EXYNOS4_GPF3(5), 0);
	for(i=0;i<8;i++){
		gpio_direction_output(EXYNOS4_GPF0(i), (unsigned int) ibuf[i]);
	}
	mdelay(1);
	gpio_direction_output(EXYNOS4_GPF3(5), 1);
	return length;
}

struct file_operations seg_fops = 
{
	.owner		= THIS_MODULE,
	.open		= seg_open,
	.write		= seg_write,
	.release	= seg_release,
};

static struct miscdevice seg_driver = {
	.fops	= &seg_fops,
	.name = SEGMENT_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int seg_init(void)
{
	printk("driver: %s DRIVER INIT\n",SEGMENT_NAME);
	return misc_register(&seg_driver);
}

void seg_exit(void)
{
	misc_deregister(&seg_driver);
        printk("driver: %s DRIVER EXIT\n",SEGMENT_NAME);
}

module_init(seg_init);
module_exit(seg_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");	
