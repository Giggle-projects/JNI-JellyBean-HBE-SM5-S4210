/*
 * TextLCD Device Driver
 *  Hanback Electronics Co.,ltd
 * File : m2_textlcd.c
 * Date : Oct, 2012
 */ 

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/miscdevice.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include "m2_textlcd.h"

void setcommand(unsigned char command)
{
	int i;
	
	gpio_direction_output(EXYNOS4_GPF2(0),0);
	gpio_direction_output(EXYNOS4_GPF2(2),0);
	
	udelay(1);
	for(i=0; i<8; i++)
	{
		if (command & 0x01)
			gpio_direction_output(EXYNOS4_GPF1(i),1);
		else
			gpio_direction_output(EXYNOS4_GPF1(i),0);
			
		command >>= 1;
	}
	udelay(1);
	
	gpio_direction_output(EXYNOS4_GPF2(2),1);
	udelay(1);	
	gpio_direction_output(EXYNOS4_GPF2(2),0);
	udelay(41);
}

void writebyte(unsigned char cData)
{
	int i;
	
	gpio_direction_output(EXYNOS4_GPF2(0),1);
	gpio_direction_output(EXYNOS4_GPF2(2),0);
	
	udelay(1);
	for(i=0; i<8; i++)
	{
		if (cData & 0x01)
			gpio_direction_output(EXYNOS4_GPF1(i),1);
		else
			gpio_direction_output(EXYNOS4_GPF1(i),0);
			
		cData >>= 1;
	}
	udelay(1);
	
	gpio_direction_output(EXYNOS4_GPF2(2),1);
	udelay(1);	
	gpio_direction_output(EXYNOS4_GPF2(2),0);
	udelay(41);
}

void initialize_textlcd()
{
	setcommand(0x38);
	setcommand(0x0c);
	setcommand(0x01);
	udelay(1960);
	setcommand(0x06);
}

int function_set()
{
	setcommand(0x38);
	return 1;
}

int display_control(int display_enable)
{
	if(display_enable == 0)
		setcommand(0x0c);
	else
		setcommand(0x08);

	return 1;
}

int cursor_control(int cursor_enable)
{
	if(cursor_enable == 0)
		setcommand(0x0f);
	else
		setcommand(0x0e);
		
	return 1;
}

int cursor_shift(int set_shift)
{
	if (set_shift == 0)
		setcommand(0x14);
	else
		setcommand(0x10);
		
	return 1;
}

int entry_mode_set()
{
	setcommand(0x06);
	return 1;
}

int return_home()
{
	setcommand(0x02);
	return 1;
}

int clear_display()
{
	setcommand(0x01);
	return 1;
}

int set_ddram_address(int pos)
{
	if (pos == 0)
		setcommand(0x80);
	else
		setcommand(0xC0);

        return 1;
}

static int textlcd_open(struct inode *minode, struct file *mfile) 
{
	int err,i;
	unsigned char pStr[] = "Hello!!!";
	unsigned char pStr1[] = "Welcome!";

	for(i=0;i<8;i++)
	{
		err = gpio_request(EXYNOS4_GPF1(i), "GPF1");
		if(err)
			printk("m2_textlcd.c failed to request GPF1(%d) \n",i);
		s3c_gpio_setpull(EXYNOS4_GPF1(i), S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPF1(i),0);
	}

	for(i=0;i<3;i++)
	{
		err = gpio_request(EXYNOS4_GPF2(i), "GPF2");
		if(err)
			printk("m2_textlcd.c failed to request GPF2(%d) \n",i);
		s3c_gpio_setpull(EXYNOS4_GPF2(i), S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPF2(i),0);
	}	

	initialize_textlcd();
	
	set_ddram_address(0);
	
	for(i=0; i<8; i++)
		writebyte(pStr[i]);
		
	set_ddram_address(1);	

	for(i=0; i<8; i++)
		writebyte(pStr1[i]);
		
    	return 0;
}

static int textlcd_release(struct inode *minode, struct file *mfile) 
{
	int i;
	for(i=0;i<8;i++)
		gpio_free(EXYNOS4_GPF1(i));
	for(i=0;i<3;i++)
		gpio_free(EXYNOS4_GPF2(i));
        return 0;
}

static int textlcd_ioctl(struct file *file,unsigned int cmd, unsigned char cData) 
{

	switch(cmd) {
        case M2_TEXTLCD_FUNCTION_SET:
                function_set();
                break;
        case M2_TEXTLCD_DISPLAY_ON:
                display_control(0);
                break;
	case M2_TEXTLCD_DISPLAY_OFF:
                display_control(1);
                break;
        case M2_TEXTLCD_DISPLAY_CURSOR_ON:
                cursor_control(0);
                break;
	case M2_TEXTLCD_DISPLAY_CURSOR_OFF:
                cursor_control(1);
                break;
	case M2_TEXTLCD_CURSOR_SHIFT_RIGHT:
        	cursor_shift(0);
                break;
	case M2_TEXTLCD_CURSOR_SHIFT_LEFT:
                cursor_shift(1);
                break;
        case M2_TEXTLCD_ENTRY_MODE_SET:
		entry_mode_set();
                break;
        case M2_TEXTLCD_RETURN_HOME:
                return_home();
                break;
        case M2_TEXTLCD_CLEAR:
    		clear_display();
                break;
        case M2_TEXTLCD_DD_ADDRESS_1:
                set_ddram_address(0);
                break;
	case M2_TEXTLCD_DD_ADDRESS_2:
                set_ddram_address(1);
                break;
        case M2_TEXTLCD_WRITE_BYTE:
                writebyte(cData);
                break;
        default:
                printk("driver : no such command!\n");
                return -ENOTTY;
    }
		
    return 0;
}

static struct file_operations textlcd_fops = {
	.owner		= THIS_MODULE,
        .open		= textlcd_open,
        .unlocked_ioctl	= textlcd_ioctl,
        .release	= textlcd_release,
};

static struct miscdevice text_driver = {
	.fops	= &textlcd_fops,
	.name	= TEXTLCD_NAME,
	.minor 	= MISC_DYNAMIC_MINOR,
};

int textlcd_init(void)
{
	printk("driver : %s DRIVER INIT\n", TEXTLCD_NAME);
	return misc_register(&text_driver);
}

void textlcd_exit(void) 
{
	misc_deregister(&text_driver);
	printk("driver: %s DRIVER EXIT\n", TEXTLCD_NAME);	
}

module_init(textlcd_init);	
module_exit(textlcd_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");	

