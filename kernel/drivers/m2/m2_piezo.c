#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/pwm.h>
#include <linux/delay.h>

#define PIEZO_PWM_CH 2
#define PERIOD_NS 38265        /* 1000000000 / 35714 => 28 Khz //38265 => 30Khz*/
#define DUTY_NS 1000*18       /* 1000(1 us) * 18 => 18 us */

static struct pwm_device *piezo_pwm;
static unsigned int piezo_period_ns;
static unsigned int piezo_duty_ns;

static int period_table[] =
{
	956022, 902527, 851788, 803858,
	758725, 716332, 676132, 638162,
	602046, 568181, 536480, 506329,
	477783, 451059, 425713, 401767,
	379218, 358037, 337952, 318979,
	301023, 284090, 268168, 253100
};

#define DRIVER_AUTHOR	"hanback"	      
#define DRIVER_DESC	"m2 piezo driver" 
#define	PIEZO_NAME	"m2_piezo"	

static int piezo_open( struct inode* inode, struct file* file)
{
	int err=0;
	
	err = gpio_request(EXYNOS4_GPD0(2),"GPD0");
	if(err)
		printk("piezo.c failed to request GPD0(2)\n");
	s3c_gpio_cfgpin(EXYNOS4_GPD0(2),S3C_GPIO_SFN(2));

	piezo_period_ns = PERIOD_NS;
	piezo_duty_ns = DUTY_NS;
	piezo_pwm = pwm_request(PIEZO_PWM_CH,"piezo");
	if(IS_ERR(piezo_pwm))
	{
		printk(KERN_ERR "unable to request PWM for PIEZO\n");
		return PTR_ERR(piezo_pwm);
	}
	return 0;
}

static int piezo_release( struct inode* inode, struct file* file)
{
	pwm_config(piezo_pwm,0,piezo_period_ns);
	pwm_disable(piezo_pwm);
	pwm_free(piezo_pwm);
        return 0;
}

static ssize_t piezo_write( struct file *filp, const char *buf, size_t length, loff_t *ofs )
{
	int period;
	int ret;
	ret = copy_from_user(&period,buf,length);
	pwm_config(piezo_pwm,piezo_duty_ns,period_table[period]);
	pwm_enable(piezo_pwm);
	mdelay(500);
	pwm_config(piezo_pwm,0,piezo_period_ns);
        return 0;
}

static struct file_operations piezo_fops =
{
        .owner = THIS_MODULE,
        .open = piezo_open,
        .release = piezo_release,
        .write = piezo_write,
};

static struct miscdevice piezo_driver =
{
        .minor = MISC_DYNAMIC_MINOR,
        .name = PIEZO_NAME,
        .fops = &piezo_fops,
};

int piezo_init(void)
{
	printk("driver : %s DRIVER INIT\n",PIEZO_NAME);
	return misc_register(&piezo_driver);
}

void piezo_exit(void)
{
	misc_deregister(&piezo_driver);
	printk("driver: %s DRIVER EXIT\n", PIEZO_NAME);
}

module_init(piezo_init);	
module_exit(piezo_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");	

