/*
 *
 * S4412 Dev Board keypad driver 
 *
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/clk.h>

#include <linux/delay.h>
#include <linux/io.h>
#include <mach/hardware.h>
#include <mach/regs-gpio.h>
#include <mach/regs-clock.h>
#include <asm/delay.h>
#include <asm/irq.h>

#include <mach/gpio.h>
#include <asm/gpio.h>
#include <plat/gpio-cfg.h>

// Debug message enable flag
#define	DEBUG_MSG			
#define	DEBUG_PM_MSG

#include "s4412_keypad.h"
#include "s4412_keycode.h"
#include "s4412_keypad_sysfs.h"

#define DEVICE_NAME "s4412-keypad"

s4412_keypad_t	s4412_keypad;

static void	generate_keycode(unsigned short prev_key, unsigned short cur_key, int *key_table);
static void 	s4412_poweroff_timer_run(void);
static void 	s4412_poweroff_timer_handler(unsigned long data);

static int	s4412_keypad_get_data(void);
static void	s4412_keypad_port_init(void);
static void 	s4412_keypad_control(void);

static void	s4412_rd_timer_handler(unsigned long data);

static int      s4412_keypad_open(struct input_dev *dev);
static void     s4412_keypad_close(struct input_dev *dev);

static void     s4412_keypad_release_device(struct device *dev);
static int      s4412_keypad_resume(struct platform_device *dev);
static int      s4412_keypad_suspend(struct platform_device *dev, pm_message_t state);

static void	s4412_keypad_config(unsigned char state);

static int __devinit    s4412_keypad_probe(struct platform_device *pdev);
static int __devexit    s4412_keypad_remove(struct platform_device *pdev);

static int __init       s4412_keypad_init(void);
static void __exit      s4412_keypad_exit(void);

static struct platform_driver s4412_platform_device_driver = {
	.probe          = s4412_keypad_probe,
	.remove         = s4412_keypad_remove,
	.suspend        = s4412_keypad_suspend,
	.resume         = s4412_keypad_resume,
	.driver		= {
		.owner	= THIS_MODULE,
		.name	= DEVICE_NAME,
	},
};

static struct platform_device s4412_platform_device = {
        .name           = DEVICE_NAME,
        .id             = -1,
        .num_resources  = 0,
        .dev    = {
                .release = s4412_keypad_release_device,
        },
};

module_init(s4412_keypad_init);
module_exit(s4412_keypad_exit);

MODULE_AUTHOR("Hanback");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Keypad interface for s4412-Dev board");

static void s4412_poweroff_timer_run(void)
{
	init_timer(&s4412_keypad.poweroff_timer);
	s4412_keypad.poweroff_timer.function = s4412_poweroff_timer_handler;

	switch(s4412_keypad.poweroff_time)	{
		default	:
			s4412_keypad.poweroff_time = 0;
		case	0:
			s4412_keypad.poweroff_timer.expires = jiffies + PERIOD_1SEC;
			break;
		case	1:
			s4412_keypad.poweroff_timer.expires = jiffies + PERIOD_3SEC;
			break;
		case	2:
			s4412_keypad.poweroff_timer.expires = jiffies + PERIOD_5SEC;
			break;
	}
	add_timer(&s4412_keypad.poweroff_timer);
}


extern	void 	SYSTEM_POWER_CONTROL(int power, int val);  // s4412-sysfs.c

static void s4412_poweroff_timer_handler(unsigned long data)
{
	// PS_HOLD Enable (LOW-> HIGH)
	SYSTEM_POWER_CONTROL(0, 0);	// BUCK6 Disable
	SYSTEM_POWER_CONTROL(1, 0);	// P5V Disable
	SYSTEM_POWER_CONTROL(2, 1);	// P12V(VLED) Disable
	printk(KERN_EMERG "%s : setting GPIO_PDA_PS_HOLD low.\n", __func__);
	(*(unsigned long *)(S5P_VA_PMU + 0x330C)) = 0x5200;
}

static void generate_keycode(unsigned short prev_key, unsigned short cur_key, int *key_table)
{
	unsigned short 	press_key, release_key, i;
	
	press_key	= (cur_key ^ prev_key) & cur_key;
	release_key	= (cur_key ^ prev_key) & prev_key;
	
	i = 0;
	while(press_key)	{
		if(press_key & 0x01)	{
			input_report_key(s4412_keypad.driver, key_table[i], KEY_PRESS);
			// POWER OFF PRESS
			if(key_table[i] == KEY_POWER)	s4412_poweroff_timer_run();
		}
		i++;	press_key >>= 1;
	}
	
	i = 0;
	while(release_key)	{
		if(release_key & 0x01)	{
			input_report_key(s4412_keypad.driver, key_table[i], KEY_RELEASE);

			// POWER OFF (LONG PRESS)		
			if(key_table[i] == KEY_POWER)		del_timer(&s4412_keypad.poweroff_timer);
		}
		i++;	release_key >>= 1;
	}
}
#if defined(DEBUG_MSG)
	static void debug_keycode_printf(unsigned short prev_key, unsigned short cur_key, const char *key_table)
	{
		unsigned short 	press_key, release_key, i;
		
		press_key	= (cur_key ^ prev_key) & cur_key;
		release_key	= (cur_key ^ prev_key) & prev_key;
		
		i = 0;
		while(press_key)	{
			if(press_key & 0x01)	printk("PRESS KEY : %s", (char *)&key_table[i * 20]);
			i++;					press_key >>= 1;
		}
		
		i = 0;
		while(release_key)	{
			if(release_key & 0x01)	printk("RELEASE KEY : %s", (char *)&key_table[i * 20]);
			i++;					release_key >>= 1;
		}
	}
#endif

static void s4412_keypad_port_init(void)
{
	if(gpio_request(EXYNOS4_GPX1(3),"GPX1(3)"))	printk("GPX1(3) error!!!\n"); //power
	s3c_gpio_cfgpin(EXYNOS4_GPX1(3), S3C_GPIO_SFN(0));
	gpio_direction_input(EXYNOS4_GPX1(3));	s3c_gpio_setpull(EXYNOS4_GPX1(3), S3C_GPIO_PULL_NONE);

	if(gpio_request(EXYNOS4_GPX0(4),"GPX0(4)"))	printk("GPX0(4) error!!!\n"); //volume up
	s3c_gpio_cfgpin(EXYNOS4_GPX0(4), S3C_GPIO_SFN(0));
	gpio_direction_input(EXYNOS4_GPX0(4));	s3c_gpio_setpull(EXYNOS4_GPX0(4), S3C_GPIO_PULL_DOWN);

	if(gpio_request(EXYNOS4_GPX0(5),"GPX0(5)"))	printk("GPX0(5) error!!!\n"); //volume down
	s3c_gpio_cfgpin(EXYNOS4_GPX0(5), S3C_GPIO_SFN(0));
	gpio_direction_input(EXYNOS4_GPX0(5));	s3c_gpio_setpull(EXYNOS4_GPX0(5), S3C_GPIO_PULL_DOWN);

}

static int s4412_keypad_get_data(void)
{
	int	key_data = 0;
	
	key_data |= gpio_get_value(EXYNOS4_GPX1(3)) ? 0x00 : 0x04; 
	key_data |= gpio_get_value(EXYNOS4_GPX0(4)) ? 0x01 : 0x00;
	key_data |= gpio_get_value(EXYNOS4_GPX0(5)) ? 0x02 : 0x00;

	return	key_data;
}

#if defined(CONFIG_HDMI_HPD)
extern	int s5p_hpd_get_status(void);
#endif

static void s4412_keypad_control(void)
{
	static	unsigned short	prev_keypad_data = 0, cur_keypad_data = 0;

#if defined(CONFIG_HDMI_HPD)
	static	unsigned char	chk_count = 0;
#endif
	// key data process
	cur_keypad_data = s4412_keypad_get_data();

	if(prev_keypad_data != cur_keypad_data)	{
		
		generate_keycode(prev_keypad_data, cur_keypad_data, &S4412_Keycode[0]);

		#if defined(DEBUG_MSG)
			debug_keycode_printf(prev_keypad_data, cur_keypad_data, &S4412_KeyMapStr[0][0]);
		#endif

		prev_keypad_data = cur_keypad_data;

		input_sync(s4412_keypad.driver);
	}

#if defined(CONFIG_HDMI_HPD)
	if(chk_count++ > 100)	{
		if(s5p_hpd_get_status())
		    input_report_switch(s4412_keypad.driver, SW_LID, ON);
		else
		    input_report_switch(s4412_keypad.driver, SW_LID, OFF);

		chk_count = 0;
	}
#endif
}

static void s4412_rd_timer_handler(unsigned long data)
{
    unsigned long flags;

    local_irq_save(flags);

	if(s4412_keypad.wakeup_delay > KEYPAD_WAKEUP_DELAY)	
		s4412_keypad_control();	
	else	
		s4412_keypad.wakeup_delay++;
		
	// Kernel Timer restart
	switch(s4412_keypad.sampling_rate)	{
		default	:
			s4412_keypad.sampling_rate = 0;
		case	0:
			mod_timer(&s4412_keypad.rd_timer,jiffies + PERIOD_10MS);
			break;
		case	1:
			mod_timer(&s4412_keypad.rd_timer,jiffies + PERIOD_20MS);
			break;
		case	2:
			mod_timer(&s4412_keypad.rd_timer,jiffies + PERIOD_50MS);
			break;
	}

    local_irq_restore(flags);
}

static int	s4412_keypad_open(struct input_dev *dev)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	return	0;
}

static void	s4412_keypad_close(struct input_dev *dev)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
}

static void	s4412_keypad_release_device(struct device *dev)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
}

static int	s4412_keypad_resume(struct platform_device *dev)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	s4412_keypad_config(KEYPAD_STATE_RESUME);
	
	// Wakeup dumy key send
	input_report_key(s4412_keypad.driver, KEY_HOME, KEY_PRESS);					
	input_report_key(s4412_keypad.driver, KEY_HOME, KEY_RELEASE);					
	input_sync(s4412_keypad.driver);
	
	s4412_keypad.wakeup_delay = 0;

    return  0;
}

static int	s4412_keypad_suspend(struct platform_device *dev, pm_message_t state)
{
	#if	defined(DEBUG_PM_MSG)
		printk("%s\n", __FUNCTION__);
	#endif

	del_timer(&s4412_keypad.rd_timer);
	
    return  0;
}

static void	s4412_keypad_config(unsigned char state)
{
	if(state == KEYPAD_STATE_BOOT)	{
		s4412_keypad_port_init();	
	}

	/* Scan timer init */
	init_timer(&s4412_keypad.rd_timer);

	s4412_keypad.rd_timer.function = s4412_rd_timer_handler;
	s4412_keypad.rd_timer.expires = jiffies + (HZ/100);

	add_timer(&s4412_keypad.rd_timer);
}

static int __devinit    s4412_keypad_probe(struct platform_device *pdev)
{
    int 	key, code, rc;

	// struct init
	memset(&s4412_keypad, 0x00, sizeof(s4412_keypad_t));
	
	// create sys_fs
	if((rc = s4412_keypad_sysfs_create(pdev)))	{
		printk("%s : sysfs_create_group fail!!\n", __FUNCTION__);
		return	rc;
	}

	s4412_keypad.driver = input_allocate_device();
	
    if(!(s4412_keypad.driver))	{
		printk("ERROR! : %s input_allocate_device() error!!! no memory!!\n", __FUNCTION__);
		s4412_keypad_sysfs_remove(pdev);
		return -ENOMEM;
    }

	set_bit(EV_KEY, s4412_keypad.driver->evbit);
//	set_bit(EV_REP, s4412_keypad.driver->evbit);	// Repeat Key

	set_bit(EV_SW, s4412_keypad.driver->evbit);
	set_bit(SW_LID & SW_MAX, s4412_keypad.driver->swbit);

	for(key = 0; key < MAX_KEYCODE_CNT; key++){
		code = S4412_Keycode[key];
		if(code<=0)
			continue;
		set_bit(code & KEY_MAX, s4412_keypad.driver->keybit);
	}

	s4412_keypad.driver->name 	= DEVICE_NAME;
	s4412_keypad.driver->phys 	= "s4412-keypad/input0";
    	s4412_keypad.driver->open 	= s4412_keypad_open;
    	s4412_keypad.driver->close	= s4412_keypad_close;
	
	s4412_keypad.driver->id.bustype	= BUS_HOST;
	s4412_keypad.driver->id.vendor 	= 0x16B4;
	s4412_keypad.driver->id.product 	= 0x0701;
	s4412_keypad.driver->id.version 	= 0x0001;

	s4412_keypad.driver->keycode = S4412_Keycode;

	if(input_register_device(s4412_keypad.driver))	{
		printk("s4412 keypad input register device fail!!\n");

		s4412_keypad_sysfs_remove(pdev);
		input_free_device(s4412_keypad.driver);	return	-ENODEV;
	}

	s4412_keypad_config(KEYPAD_STATE_BOOT);

	printk("--------------------------------------------------------\n");
	printk("S4412 Keypad driver initialized!! Ver 1.0\n");
	printk("--------------------------------------------------------\n");

    return 0;
}

static int __devexit s4412_keypad_remove(struct platform_device *pdev)
{
	s4412_keypad_sysfs_remove(pdev);

	input_unregister_device(s4412_keypad.driver);
	
	del_timer(&s4412_keypad.rd_timer);
	
	// Free GPIOs
	gpio_free(EXYNOS4_GPX1(3));
	gpio_free(EXYNOS4_GPX0(4));
	gpio_free(EXYNOS4_GPX0(5));
	
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	return  0;
}

static int __init s4412_keypad_init(void)
{
	int ret = platform_driver_register(&s4412_platform_device_driver);
	
	#if	defined(DEBUG_MSG)
		printk("%s\n", __FUNCTION__);
	#endif
	
	if(!ret)        {
		ret = platform_device_register(&s4412_platform_device);
		
		#if	defined(DEBUG_MSG)
			printk("platform_driver_register %d \n", ret);
		#endif
		
		if(ret)	platform_driver_unregister(&s4412_platform_device_driver);
	}
	return ret;
}

static void __exit s4412_keypad_exit(void)
{
	#if	defined(DEBUG_MSG)
		printk("%s\n",__FUNCTION__);
	#endif
	
	platform_device_unregister(&s4412_platform_device);
	platform_driver_unregister(&s4412_platform_device_driver);
}

