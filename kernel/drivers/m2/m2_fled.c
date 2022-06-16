#include <linux/miscdevice.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <asm/ioctl.h>
#include <asm/uaccess.h>
#include <mach/gpio.h>
#include <plat/gpio-cfg.h>
#include <linux/pwm.h>

/* Prototypes */
#define FLED_PWM_CH 3
#define PERIOD_NS 390        /* 1000000000 / 35714 => 28 Khz //38265 => 30Khz*/
#define DUTY_NS 195       /* 1000(1 us) * 18 => 18 us */

#define DRIVER_AUTHOR   "hanback"             
#define DRIVER_DESC     "m2 full color led driver" 
#define FLED_NAME       "m2_fled"

#define FLED_IO			0x54
#define FLED1_WRITE_ENABLE	_IO(FLED_IO, 0x31)
#define FLED2_WRITE_ENABLE	_IO(FLED_IO, 0x32)
#define FLED3_WRITE_ENABLE	_IO(FLED_IO, 0x33)

static struct pwm_device *fled_pwm;
static unsigned int fled_period_ns;
static unsigned int fled_duty_ns;
static int fled_select_flag=0;
static uint16_t din_data[3][16]={0};

void allDinWriteData(uint16_t data)
{
	int j;
	for(j=0;j<16;j++)
	{
		if(data&0x1){
			gpio_direction_output(EXYNOS4_GPF2(5),1);
			gpio_direction_output(EXYNOS4_GPF2(6),1);
			gpio_direction_output(EXYNOS4_GPF2(7),1);
		}
		else{
			gpio_direction_output(EXYNOS4_GPF2(5),0);
			gpio_direction_output(EXYNOS4_GPF2(6),0);
			gpio_direction_output(EXYNOS4_GPF2(7),0);
		}
		gpio_direction_output(EXYNOS4_GPF2(3),1);
		ndelay(100);
		gpio_direction_output(EXYNOS4_GPF2(3),0);
		ndelay(100);
		data = data >> 1;
	}
}

void din1WriteData(uint16_t data)
{
	int i;
	for(i=0;i<16;i++)
	{
		if(data&0x1)
			gpio_direction_output(EXYNOS4_GPF2(5),1);
		else
			gpio_direction_output(EXYNOS4_GPF2(5),0);
		gpio_direction_output(EXYNOS4_GPF2(3),1);
		ndelay(100);
		gpio_direction_output(EXYNOS4_GPF2(3),0);
		ndelay(100);
		data = data >> 1;
	}
}

void din2WriteData(uint16_t data)
{
        int i;
        for(i=0;i<16;i++)
        {
                if(data&0x1)
                        gpio_direction_output(EXYNOS4_GPF2(6),1);
                else
                        gpio_direction_output(EXYNOS4_GPF2(6),0);
                gpio_direction_output(EXYNOS4_GPF2(3),1);
                ndelay(100);
                gpio_direction_output(EXYNOS4_GPF2(3),0);
                ndelay(100);
                data = data >> 1;
        }
}

void din3WriteData(uint16_t data)
{
        int i;
        for(i=0;i<16;i++)
        {
                if(data&0x1)
                        gpio_direction_output(EXYNOS4_GPF2(7),1);
                else
                        gpio_direction_output(EXYNOS4_GPF2(7),0);
                gpio_direction_output(EXYNOS4_GPF2(3),1);
                ndelay(100);
                gpio_direction_output(EXYNOS4_GPF2(3),0);
                ndelay(100);
                data = data >> 1;
        }
}

void reset_command(void)
{
	int i=0;
	for(i=3;i<8;i++)
		gpio_direction_output(EXYNOS4_GPF2(i),0);
	gpio_direction_output(EXYNOS4_GPF2(3),1);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(3),0);
	gpio_direction_output(EXYNOS4_GPF2(4),1);
	ndelay(100);
	allDinWriteData(0x0001);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),0);
	allDinWriteData(0x0000);
}

void writeConfigurationRegister(void)
{
	gpio_direction_output(EXYNOS4_GPF2(3),1);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(3),0);
	gpio_direction_output(EXYNOS4_GPF2(4),1);
	allDinWriteData(0x0034);
	gpio_direction_output(EXYNOS4_GPF2(4),0);
	ndelay(100);
	allDinWriteData(0x041f);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),1);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),0);
	ndelay(100);
}

void writeDisplayData(void)
{
	int i=0;
	for(i=3;i<8;i++)
		gpio_direction_output(EXYNOS4_GPF2(i),0);
	gpio_direction_output(EXYNOS4_GPF2(3),1);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(3),0);
	gpio_direction_output(EXYNOS4_GPF2(4),1);
	ndelay(100);
	din1WriteData(0x0084);
	gpio_direction_output(EXYNOS4_GPF2(4),0);
	ndelay(100);
	for(i=0;i<16;i++)
	{	
		din1WriteData(din_data[0][i]);
		ndelay(100);
		gpio_direction_output(EXYNOS4_GPF2(4),1);
		ndelay(100);
		gpio_direction_output(EXYNOS4_GPF2(4),0);
	}
	
	for(i=3;i<8;i++)
                gpio_direction_output(EXYNOS4_GPF2(i),0);
        gpio_direction_output(EXYNOS4_GPF2(3),1);
        ndelay(100);
        gpio_direction_output(EXYNOS4_GPF2(3),0);
        gpio_direction_output(EXYNOS4_GPF2(4),1);
        ndelay(100);
        din2WriteData(0x0084);
        gpio_direction_output(EXYNOS4_GPF2(4),0);
        ndelay(100);
        for(i=0;i<16;i++)
        {       
                din2WriteData(din_data[1][i]);
                ndelay(100);
                gpio_direction_output(EXYNOS4_GPF2(4),1);
                ndelay(100);
                gpio_direction_output(EXYNOS4_GPF2(4),0);
        }  
	
	for(i=3;i<8;i++)
                gpio_direction_output(EXYNOS4_GPF2(i),0);
        gpio_direction_output(EXYNOS4_GPF2(3),1);
        ndelay(100);
        gpio_direction_output(EXYNOS4_GPF2(3),0);
        gpio_direction_output(EXYNOS4_GPF2(4),1);
        ndelay(100);
        din3WriteData(0x0084);
        gpio_direction_output(EXYNOS4_GPF2(4),0);
        ndelay(100);
        for(i=0;i<16;i++)
        {       
                din3WriteData(din_data[2][i]);
                ndelay(100);
                gpio_direction_output(EXYNOS4_GPF2(4),1);
                ndelay(100);
                gpio_direction_output(EXYNOS4_GPF2(4),0);
        }  
}

void globalLatchCommand(void)
{
	int i;
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),1);
	ndelay(100);
	for(i=0;i<8;i++)
	{
		gpio_direction_output(EXYNOS4_GPF2(3),1);
		ndelay(100);
		gpio_direction_output(EXYNOS4_GPF2(3),0);
		ndelay(100);
	}
	gpio_direction_output(EXYNOS4_GPF2(3),1);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),0);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),1);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),0);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),1);
	ndelay(100);
	gpio_direction_output(EXYNOS4_GPF2(4),0);
	ndelay(200);
	gpio_direction_output(EXYNOS4_GPF2(3),0);
	ndelay(100);
}

static int fled_open( struct inode* inode, struct file* file)
{
	return 0;
}

static int fled_release( struct inode* inode, struct file* file)
{
	int i;
	for(i=3;i<8;i++)
	{
		gpio_direction_output(EXYNOS4_GPF2(i),0);
		gpio_free(EXYNOS4_GPF2(i));
	}
        return 0;
}

static int fled_ioctl( struct file* file, unsigned int cmd, unsigned long arg)
{
        switch(cmd)
        {
        case FLED1_WRITE_ENABLE:
         	fled_select_flag = 0;
                break;
        case FLED2_WRITE_ENABLE:
		fled_select_flag = 1;
                break;
        case FLED3_WRITE_ENABLE:
		fled_select_flag = 2;
                break;
        default:
                return -ENOTTY;
        }

        return 0;
}

static ssize_t fled_write( struct file *filp, const short *gdata, size_t length, loff_t *ofs )
{
	int ret,j;
	unsigned short buf[12]={0};
	ret = copy_from_user(buf,gdata,length);
	for(j=0;j<12;j++){
		if(buf[j] == 0)
			din_data[fled_select_flag][j] = 0x0;
		else
			din_data[fled_select_flag][j] = buf[j];
	}
	din_data[fled_select_flag][12] = 0x0;
	din_data[fled_select_flag][13] = 0x0;
	din_data[fled_select_flag][14] = 0x0;
	din_data[fled_select_flag][15] = 0x0;

	writeDisplayData();
	globalLatchCommand();
        return 0;
}

static struct file_operations fled_fops =
{
        .owner = THIS_MODULE,
        .open = fled_open,
        .unlocked_ioctl = fled_ioctl,
        .release = fled_release,
        .write = fled_write,
};

static struct miscdevice fled_driver =
{
        .minor = MISC_DYNAMIC_MINOR,
        .name = FLED_NAME,
        .fops = &fled_fops,
};

int fled_init(void)
{
	int i,err;
	printk("driver : %s DRIVER INIT\n",FLED_NAME);

	//pwm request
        fled_period_ns = PERIOD_NS;
        fled_duty_ns = DUTY_NS;
        fled_pwm = pwm_request(FLED_PWM_CH,"fled");
        if(IS_ERR(fled_pwm))
        {
                printk(KERN_ERR "unable to request PWM for FLED\n");
                return PTR_ERR(fled_pwm);
        }
        pwm_config(fled_pwm,fled_duty_ns,fled_period_ns);
        pwm_enable(fled_pwm);	

	for(i=3;i<8;i++)
        {
                err = gpio_request(EXYNOS4_GPF2(i),"GPF2");
                if(err)
                {
                        printk(KERN_ERR "m2_fled.c failed to request GPF2\n");
                        return err;
                }
                else
                        gpio_direction_output(EXYNOS4_GPF2(i),0);
        }

	reset_command();
        writeConfigurationRegister();
	writeDisplayData();
	globalLatchCommand();

	return misc_register(&fled_driver);
}

void fled_exit(void)
{
        pwm_config(fled_pwm,0,fled_period_ns);
        pwm_disable(fled_pwm);
        pwm_free(fled_pwm);

	misc_deregister(&fled_driver);
	printk("driver: %s DRIVER EXIT\n", FLED_NAME);
}

module_init(fled_init);	
module_exit(fled_exit);	

MODULE_AUTHOR(DRIVER_AUTHOR);	
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("Dual BSD/GPL");	
