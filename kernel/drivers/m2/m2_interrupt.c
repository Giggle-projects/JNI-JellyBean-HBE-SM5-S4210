#include <linux/init.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/ioport.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/gpio.h>
#include <linux/sched.h>
#include <plat/gpio-cfg.h>
#include <mach/irqs.h>
#include <asm/io.h>
#include <asm/uaccess.h>

#define DRIVER_AUTHOR           	"Hanback"		
#define DRIVER_DESC			"GPIO Button Interrupt Test Program"	
#define INT_BUTTON_NAME 		"m2_int_button"		
#define INT_BUTTON_MODULE_VERSION 	"GPIO Button Interrupt V1.0"	
#define IRQ_BUTTON_1			IRQ_EINT(1)
#define IRQ_BUTTON_2			IRQ_EINT(12)

unsigned long irq_num = 0;
static irqreturn_t button_interrupt(int irq, void *dev_id);
static DECLARE_WAIT_QUEUE_HEAD(wait_queue);	

static irqreturn_t button_interrupt(int irq, void *dev_id)
{
	irq_num = irq;
	wake_up_interruptible(&wait_queue);
	return IRQ_HANDLED;
}

int int_button_open(struct inode *minode, struct file *mfile)
{
	int res;

	res = request_irq(IRQ_BUTTON_1,button_interrupt,IRQF_TRIGGER_FALLING,"GPIO",NULL);
	if(res < 0)
		printk(KERN_ERR "%s : Request for IRQ %d failed\n", __FUNCTION__,IRQ_BUTTON_1);
	res = request_irq(IRQ_BUTTON_2,button_interrupt,IRQF_TRIGGER_FALLING,"GPIO",NULL);
	if(res < 0)
		printk(KERN_ERR "%s : Request for IRQ %d failed\n", __FUNCTION__,IRQ_BUTTON_2);

	return 0;
}

int int_button_release(struct inode *minode, struct file *mfile)
{
	free_irq(IRQ_BUTTON_1,NULL);
	free_irq(IRQ_BUTTON_2,NULL);
	return 0;
}

ssize_t int_button_read(struct file *inode, char *gdata, size_t length, loff_t *off_what)
{
	char msg[100];
	int ret=0;

	interruptible_sleep_on(&wait_queue);

	switch(irq_num) {
		case IRQ_BUTTON_1 :
			sprintf(msg,"S1 Button pushed.");
			break;
		case IRQ_BUTTON_2 :
			sprintf(msg,"S2 Button pushed.");
			break;
	}

	ret=copy_to_user(gdata,&msg,sizeof(msg));

	if(ret<0) return -1;

	return length;
}

struct file_operations int_button_fops = {
	.owner		= THIS_MODULE,
	.open		= int_button_open,
	.read		= int_button_read,
	.release	= int_button_release,
};

static struct miscdevice int_button_driver = {
	.fops = &int_button_fops,
	.name = INT_BUTTON_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};

int int_button_init(void)
{
	int err;

	err = gpio_request(EXYNOS4_GPX0(1), "GPX0");
	if(err) {
		printk(KERN_ERR "%s:gpio request error(%d)!!\n", __FUNCTION__, err);
	}
	else 
	{
		s3c_gpio_cfgpin(EXYNOS4_GPX0(1),S3C_GPIO_SFN(0));
		s3c_gpio_setpull(EXYNOS4_GPX0(1), S3C_GPIO_PULL_UP);
		gpio_free(EXYNOS4_GPX0(1));
	}

	err = gpio_request(EXYNOS4_GPX1(4), "GPX0");
        if(err) {
                printk(KERN_ERR "%s:gpio request error(%d)!!\n", __FUNCTION__, err);
        }       
        else    
        {       
                s3c_gpio_cfgpin(EXYNOS4_GPX1(4),S3C_GPIO_SFN(0));
                s3c_gpio_setpull(EXYNOS4_GPX1(4), S3C_GPIO_PULL_UP);
                gpio_free(EXYNOS4_GPX1(4));
        } 

	printk("driver : %s DRIVER_INIT\n",INT_BUTTON_NAME);
	return misc_register(&int_button_driver);
}

void int_button_exit(void) 
{
	misc_deregister(&int_button_driver);
	gpio_free(EXYNOS4_GPX0(1));
	gpio_free(EXYNOS4_GPX1(4));
	printk("driver: %s DRIVER EXIT\n", INT_BUTTON_NAME);
}

module_init(int_button_init);		
module_exit(int_button_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC); 
MODULE_LICENSE("Dual BSD/GPL");	 

