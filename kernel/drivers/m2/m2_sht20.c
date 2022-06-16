#include <linux/platform_device.h> 
#include <linux/input.h> 
#include <linux/types.h> 
#include <linux/delay.h> 
#include <linux/workqueue.h> 
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <linux/interrupt.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/stat.h>
#include <linux/irq.h>
#include <linux/signal.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/pm.h>
#include <linux/mutex.h>
#include <linux/suspend.h>
#include <linux/poll.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/ioport.h>
#include <asm/io.h>
#include <asm/ioctl.h>
#include <asm/uaccess.h>
#include <linux/delay.h>
#include <linux/mm.h>
#include <asm/cacheflush.h>

#define SHT20_CHIP_ADREESS_WRITE 0x40
#define SHT20_CHIP_ADREESS_READ 0x81
#define SHT20_CHIP_TRIGGER_T_M 0xE3
#define SHT20_CHIP_TRIGGER_RH_M 0xE5
#define SHT20_CHIP_TRIGGER_T_N 0xF3
#define SHT20_CHIP_TRIGGER_RH_N 0xF5
#define SHT20_SOFT_RESET 0xFE

struct sht20_sensor {
        struct i2c_client *client;
        struct device *dev;
        struct input_dev *idev;
};

static struct sht20_sensor *sht20_ptr; 

static void sht20_read_sensor(struct i2c_client *client,
                               u8 *buffer, int length)
{
	int ret;
	char cmd;
	switch(length)
	{
		case 0:
			cmd = SHT20_CHIP_TRIGGER_T_M;
			break;
		case 1:
			cmd = SHT20_CHIP_TRIGGER_RH_M;
			break;
		case 3:
			cmd = SHT20_CHIP_TRIGGER_T_N;
			break;
		case 4:
			cmd = SHT20_CHIP_TRIGGER_RH_N;
			break;
	}
	
	struct i2c_msg msg[] = {
	{
        	.addr = SHT20_CHIP_ADREESS_WRITE,
        	.flags = 0,
        	.len = 1,
        	.buf = &cmd,
        },
        {
        	.addr = SHT20_CHIP_ADREESS_WRITE,
        	.flags = I2C_M_RD,
        	.len = 2,
        	.buf = buffer,
        },
    	};
	
    	ret  = i2c_transfer(client->adapter, msg, 2);
	

}


static void sht20_user_set(struct i2c_client *client,
                               u8 *buffer, int length)
{
	char cmd = 0xE7;
	char cmd1 = 0xE6;
	int ret;
	
	struct i2c_msg msg[] = {
	{
        	.addr = SHT20_CHIP_ADREESS_WRITE,
        	.flags = 0,
        	.len = 1,
        	.buf = &cmd,
        },
        {
        	.addr = SHT20_CHIP_ADREESS_READ,
        	.flags = I2C_M_RD,
        	.len = 1,
        	.buf = buffer,
        },
		{
        	.addr = SHT20_CHIP_ADREESS_WRITE,
        	.flags = 0,
        	.len = 1,
        	.buf = &cmd1,
        },
    	};
	
    	ret = i2c_transfer(client->adapter, msg, 2);
}

static int m2_sht20_softReset(struct i2c_client *client)
{
	
	char cmd = SHT20_SOFT_RESET;
	
	struct i2c_msg msg[] = {
		{
        	.addr = SHT20_CHIP_ADREESS_WRITE,
        	.flags = 0,
        	.len = 1,
        	.buf = &cmd,
        }
	};
	
	i2c_transfer(client->adapter, msg, 1);
}

static int m2_sht20_open(struct inode *minode, struct file *mfile) 
{
	u16 buffer[3];
	struct sht20_sensor *sensor;
	sensor = sht20_ptr;
	
	m2_sht20_softReset(sensor->client);
	
	return 0;
}

static int m2_sht20_release(struct inode *minode, struct file *mfile) 
{
        return 0;
}

static ssize_t m2_sht20_write(struct file *inode, const char*gdata, size_t length, loff_t *off_what) 
{
	return length;
}

static int m2_sht20_ioctl(struct file *file,unsigned int cmd, unsigned char *cData) 
{
	u8 buffer[3];
	u16 sht20_buffer[3];
	int sht_sensor_value;
	struct sht20_sensor *sensor;
	sensor = sht20_ptr;

	sht20_read_sensor(sensor->client, (u8 *)buffer, cmd);
	
	sht20_buffer[0] = buffer[0]*256;
	sht20_buffer[1] = buffer[1] >> 2;
	
	sht_sensor_value = sht20_buffer[0] + sht20_buffer[1];
	

 	return sht_sensor_value;
}

static struct file_operations m2_sht20_fops = {
	.owner		= THIS_MODULE,
        .open		= m2_sht20_open,
        .write		= m2_sht20_write,
        .unlocked_ioctl	= m2_sht20_ioctl,
        .release	= m2_sht20_release,
};

static struct miscdevice m2_sht20_driver = {
	.fops	= &m2_sht20_fops,
	.name	= "m2_sht20",
	.minor 	= MISC_DYNAMIC_MINOR,
};

static int __devinit m2_sht20_probe(struct i2c_client *client,
                                   const struct i2c_device_id *id)
{
    	struct sht20_sensor *sensor;
    	struct input_dev *idev;
	int error;

    	sensor = kzalloc(sizeof(struct sht20_sensor), GFP_KERNEL);
	idev = input_allocate_device();
    	if (!sensor || !idev) {
    		dev_err(&client->dev, "failed to allocate driver data\n");
    		error = -ENOMEM;
    		goto err_free_mem;
    	}
	
	if(!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
	{
		error = -ENODEV;
		goto err_free_mem;
	}
	sensor->client = client;
    	sensor->dev = &client->dev;
    	sensor->idev = idev;
		
	i2c_set_clientdata(client,sensor);
	sht20_ptr = sensor;
	
	error = misc_register(&m2_sht20_driver);
	if (error) {
		dev_err(&client->dev, "failed to register input device\n");
		goto err_mpuirq_failed;
    	}
	return 0;
		
err_mpuirq_failed:
	misc_deregister(&m2_sht20_driver);

err_free_mem:
	input_free_device(idev);
	kfree(sensor);
	return error;
}


static int __devexit m2_sht20_remove(struct i2c_client *client)
{
	misc_deregister(&m2_sht20_driver);
}

static const struct i2c_device_id m2_sht20_ids[] = {
       { "m2_sht20", 0 },
       { }
};
MODULE_DEVICE_TABLE(i2c, m2_sht20_ids);

static struct i2c_driver m2_sht20_i2c_driver = {
        .driver = {
                .name   = "m2_sht20",
                .owner  = THIS_MODULE,

        },
        .probe          = m2_sht20_probe,
        .remove         = __devexit_p(m2_sht20_remove),
        .id_table       = m2_sht20_ids,
};

static int __init m2_sht20_init(void)
{
        return i2c_add_driver(&m2_sht20_i2c_driver);
}
module_init(m2_sht20_init);

static void __exit m2_sht20_exit(void)
{
        i2c_del_driver(&m2_sht20_i2c_driver);
}
module_exit(m2_sht20_exit);

MODULE_AUTHOR("hanback sht20");
MODULE_DESCRIPTION("SHT20 Sensor driver");
MODULE_LICENSE("Dual BSD/GPL");
MODULE_ALIAS("m2_ sht20");

