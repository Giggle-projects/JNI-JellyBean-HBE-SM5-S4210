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

int m2_buffer_init(void) {
	int ret,i;
	ret = gpio_request(EXYNOS4_GPA0(3),"GPA0");
	if(ret)
	{
		printk(KERN_ERR "m2_buffer.c failed to request GPA0(3)\n");
	}
	else
	{
		s3c_gpio_setpull(EXYNOS4_GPA0(3),S3C_GPIO_PULL_NONE);
		gpio_direction_output(EXYNOS4_GPA0(3),0);
		for(i=0;i<8;i++)
		{
			gpio_request(EXYNOS4_GPF0(i),"GPF0");
			gpio_request(EXYNOS4_GPF1(i),"GPF1");
			gpio_request(EXYNOS4_GPF2(i),"GPF2");
			gpio_direction_output(EXYNOS4_GPF0(i),0);
			gpio_direction_output(EXYNOS4_GPF1(i),0);
			gpio_direction_output(EXYNOS4_GPF2(i),0);
			gpio_free(EXYNOS4_GPF0(i));
			gpio_free(EXYNOS4_GPF1(i));
			gpio_free(EXYNOS4_GPF2(i));
			if(i<6)
			{
				gpio_request(EXYNOS4_GPF3(i),"GPF3");
				gpio_direction_output(EXYNOS4_GPF3(i),0);
				gpio_free(EXYNOS4_GPF3(i));
			}
		}
	}
	return 0;
}

void m2_buffer_exit(void) 
{
	gpio_free(EXYNOS4_GPA0(3));
}

module_init(m2_buffer_init);	
module_exit(m2_buffer_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_LICENSE("Dual BSD/GPL");	 
