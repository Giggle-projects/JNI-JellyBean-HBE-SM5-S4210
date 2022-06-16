#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/platform_device.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <asm/fcntl.h>
#include <linux/ioport.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <plat/adc.h>

#define ADC_CHANNEL  1 

static struct s3c_adc_client *client;

#define DRIVER_AUTHOR	"hanback"	      
#define DRIVER_DESC	"cds program" 

#define	CDS_NAME	"cds"	

struct cds_data {
        atomic_t enable;                
        atomic_t delay;                
        int	data;  	
        struct input_dev *input;
};
static struct cds_data *cds_ptr;

static int cds_open( struct inode* inode, struct file* file)
{
	return 0;
}

static int cds_release( struct inode* inode, struct file* file)
{
        return 0;
}

static ssize_t cds_read( struct file *inode, char *gdata, size_t length, loff_t *off_what )
{
	int ret, value;

	value = s3c_adc_read(client,ADC_CHANNEL);

	ret=copy_to_user(gdata,&value,4);

	if(ret<0) return -1;

    	return 0;
}

static struct file_operations cds_fops =
{
        .owner = THIS_MODULE,
        .open = cds_open,
        .release = cds_release,
        .read = cds_read,
};

static struct miscdevice cds_misc =
{
        .minor = MISC_DYNAMIC_MINOR,
        .name = CDS_NAME,
        .fops = &cds_fops,
};

static int cds_probe(struct platform_device *pdev)
{
	struct cds_data *cds;
        int err = 0;
	client = s3c_adc_register(pdev, NULL, NULL, 0);
	if(IS_ERR(client))
	{
		printk(KERN_INFO "m2_cds.c failed to register adc \n");
		return -1;
	}
	platform_set_drvdata(pdev,client);
	cds = kzalloc(sizeof(struct cds_data), GFP_KERNEL);
	if(!cds) {
		return -ENOMEM;
	}
	err = misc_register(&cds_misc);
        if (err < 0)
                goto error_1;
        cds_ptr = cds;
	
	return 0;
error_1:
	kfree(cds);
	return err;
}

static int cds_remove(struct platform_device *pdev)
{
	struct cds_data *cds = platform_get_drvdata(pdev);
	s3c_adc_release(client);
        misc_deregister(&cds_misc);
        return 0;
}

static int cds_suspend(struct platform_device *pdev, pm_message_t mesg)
{
        return 0;
}

static int cds_resume(struct platform_device *pdev)
{
        return 0;
}

static struct platform_driver cds_driver = {
	.probe		= cds_probe,
	.remove		= cds_remove,
	.suspend	= cds_suspend,
	.resume		= cds_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= "cds",
	},
};

static int __init cds_init(void)
{
	printk("driver: %s DRIVER INIT\n",CDS_NAME);
	return platform_driver_register(&cds_driver);
}

static void __exit cds_exit(void)
{
	platform_driver_unregister(&cds_driver);
	printk("driver: %s DRIVER EXIT\n", CDS_NAME);
}

module_init(cds_init);	
module_exit(cds_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");	
