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

#define DRIVER_AUTHOR           "Hanback"
#if defined(CONFIG_MACH_SM5S4210)
#define DRIVER_DESC             "HBE-SM5-S4210 Vibrator"
#else
#define DRIVER_DESC             "HBE-SM7-S4412 Vibrator"
#endif
#define VIB_NAME                "vibdev"

static int vib_open(struct inode *minode, struct file *mfile)
{
        int err;

        err = gpio_request(EXYNOS4_GPC0(4), "GPC0(4)");
        if (err)
                printk("#### vibdev failed to request GPC0_4 ####\n");

        s3c_gpio_setpull(EXYNOS4_GPC0(4), S3C_GPIO_PULL_NONE);
        gpio_direction_output(EXYNOS4_GPC0(4), 0);

        return 0;
}

static int vib_release(struct inode *minode, struct file *mfile)
{
        gpio_free(EXYNOS4_GPC0(4));
        return 0;
}

static int vib_ioctl(struct file *flip, unsigned int cmd, unsigned long arg)
{
        if(cmd !=0 ) {
                gpio_direction_output(EXYNOS4_GPC0(4), 1);
                mdelay(cmd);
        }

        gpio_direction_output(EXYNOS4_GPC0(4), 0);

        return 0;
}

struct file_operations vib_fops = {
	.owner	= THIS_MODULE,
	.open = vib_open,
	.release = vib_release,
	.unlocked_ioctl = vib_ioctl,
};


static struct miscdevice vib_driver = {
	.fops	= &vib_fops,
	.name = VIB_NAME,
	.minor = MISC_DYNAMIC_MINOR,
};


int vib_init(void)
{
        printk("driver: %s DRIVER INIT\n",VIB_NAME);
	return misc_register(&vib_driver);
}

void vib_exit(void)
{
	misc_deregister(&vib_driver);
        printk("driver: %s DRIVER EXIT\n",VIB_NAME);
}

module_init(vib_init);
module_exit(vib_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");

