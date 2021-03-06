/*
 * iM-3640 Accelerometer + Compass driver
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA  02110-1301, USA.
 */

#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/errno.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/input.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/types.h>
#include <linux/workqueue.h>
#include <linux/miscdevice.h>
#include <asm/uaccess.h>

#define IM3640_VERSION "1.0.0"

#define IM3640_NAME "im3640"

/* for debugging */
#define DEBUG 1
#define DEBUG_THRESHOLD 0
#define TRACE_FUNC() pr_debug(IM3640_NAME ": <trace> %s()\n", __FUNCTION__)

/*
 * Default parameters
 */
#define SENSOR_DELAY_FASTEST            0       // delay = 10
#define SENSOR_DELAY_GAME               1       // delay = 20
#define SENSOR_DELAY_UI                 2       // delay = 60
#define SENSOR_DELAY_NORMAL             3       // delay = 200

#define IM3640_DEFAULT_DELAY            SENSOR_DELAY_NORMAL
#define IM3640_MAX_DELAY                SENSOR_DELAY_NORMAL
#define IM3640_DEFAULT_THRESHOLD        0

/*
 * Registers
 */
#define IM3640_DEV_ID_REG              0x01
#define IM3640_DEV_ID                  0x51

#define IM3640_REVNUM_REG              0x02
#define IM3640_REVNUM                  0x10

#define IM3640_MAG_ALARM_REG           0x03
#define IM3640_MAG_ALARM_OVERRANGE	0x00
#define IM3640_MAG_ALARM_BAD		0x01
#define IM3640_MAG_ALARM_NOTBAD		0x02
#define IM3640_MAG_ALARM_GOOD		0x03

#define IM3640_AMS_POWER_REG            0x14
#define IM3640_AMS_POWER_ACTIVE         0x00
#define IM3640_AMS_POWER_DOWN           0x01
#define IM3640_AMS_POWER_SAVE           0x02

// a Magnetic Azimuth, Data range : 0~359, Resolution : 1
#define IM3640_AZIMUTH_REGH            0x20
#define IM3640_AZIMUTH_REGL            0x21

// a Magnetic Pitch angle, Data range : -89~+89, Resolution : 1
#define IM3640_PITCH_REGH            0x22
#define IM3640_PITCH_REGL            0x23

// a Magnetic Roll angle , Data range : -89~+89, Resolution : 1
#define IM3640_ROLL_REGH            0x24
#define IM3640_ROLL_REGL            0x25

// X-axis Aceeleration Raw Data , Sensitivity : 256 COUNT/g
#define IM3640_ACC_X_REGH            0x26
#define IM3640_ACC_X_REGL            0x27

// Y-axis Aceeleration Raw Data , Sensitivity : 256 COUNT/g
#define IM3640_ACC_Y_REGH            0x28
#define IM3640_ACC_Y_REGL            0x29

// Z-axis Aceeleration Raw Data , Sensitivity : 256 COUNT/g
#define IM3640_ACC_Z_REGH            0x2A
#define IM3640_ACC_Z_REGL            0x2B

// X-axis Magnetic flux density value , Sensitivity : 2mG/count
#define IM3640_MAG_X_REGH            0x2C
#define IM3640_MAG_X_REGL            0x2D

// Y-axis Magnetic flux density value , Sensitivity : 2mG/count
#define IM3640_MAG_Y_REGH            0x2E
#define IM3640_MAG_Y_REGL            0x2F

// Z-axis Magnetic flux density value , Sensitivity : 2mG/count
#define IM3640_MAG_Z_REGH            0x30
#define IM3640_MAG_Z_REGL            0x31

// Navigation Mode Select
#define IM3640_NAVI_MODE_REG         0x50
#define IM3640_NAVI_MODE_NORMAL      0x00
#define IM3640_NAVI_MODE_NAVI        0x01

// Accelerometer Offset Calibration Mode
#define IM3640_CAL_ACC_REG        0xAA


/* ABS axes parameter range [um/s^2] (for input event) */
#define GRAVITY_EARTH                   9806550
#define ABSMIN_2G                       (-GRAVITY_EARTH * 2)
#define ABSMAX_2G                       (GRAVITY_EARTH * 2)

/*
 * Transformation matrix for chip mounting position
 */
static const int im3640_position_map[][3][3] = {
	{{ 0, -1,  0}, { 1,  0,  0}, { 0,  0,  1}}, /* top/upper-left */
	{{ 1,  0,  0}, { 0,  1,  0}, { 0,  0,  1}}, /* top/upper-right */
	{{ 0,  1,  0}, {-1,  0,  0}, { 0,  0,  1}}, /* top/lower-right */
	{{-1,  0,  0}, { 0, -1,  0}, { 0,  0,  1}}, /* top/lower-left */
	{{ 0,  1,  0}, { 1,  0,  0}, { 0,  0, -1}}, /* bottom/upper-right */
	{{-1,  0,  0}, { 0,  1,  0}, { 0,  0, -1}}, /* bottom/upper-left */
	{{ 0, -1,  0}, {-1,  0,  0}, { 0,  0, -1}}, /* bottom/lower-left */
	{{ 1,  0,  0}, { 0, -1,  0}, { 0,  0, -1}}, /* bottom/lower-right */
};

struct sensor_data {
	int	accel[3]; //x,y,z
	int	compass[3]; //x,y,z
	int	ori[3]; //azimuth,pitch,roll
};

/*
 * driver private data
 */
struct im3640_data {
	atomic_t enable;                /* attribute value */
	atomic_t delay;                /* attribute value */
	atomic_t position;                /* attribute value */
//	atomic_t threshold;                /* attribute value */
	struct sensor_data last;       /* last measured data */
	struct mutex enable_mutex;
	struct mutex data_mutex;
	struct i2c_client *client;
	struct input_dev *input;
	struct delayed_work work;
};

static struct im3640_data *im3640_ptr;

#define IM3640IO			       	0x50
#define IM3640_IOCTL_ENABLE		_IO(IM3640IO, 0x31)
#define IM3640_IOCTL_DISABLE		_IO(IM3640IO, 0x32)
#define IM3640_IOCTL_IS_ENABLE		_IOR(IM3640IO, 0x33, int)
#define IM3640_IOCTL_DELAY_GET		_IOR(IM3640IO, 0x34, int)
#define IM3640_IOCTL_DELAY_SET		_IOW(IM3640IO, 0x35, int)
#define IM3640_IOCTL_DATA		_IOR(IM3640IO, 0x36, int[9])

static int im3640_device_open( struct inode*, struct file* );
static int im3640_device_release( struct inode*, struct file* );
static long im3640_device_ioctl( struct file*, unsigned int, unsigned long ); // update park for unlocked_ioctl 
static ssize_t im3640_device_read( struct file *filp, char *buf, size_t count, loff_t *ofs );
static ssize_t im3640_device_write( struct file *filp, const char *buf, size_t count, loff_t *ofs );
static unsigned int im3640_device_poll( struct file *filp, struct poll_table_struct *pwait );

static struct file_operations im3640_device_fops =
{
	.owner = THIS_MODULE,
	.open = im3640_device_open,
	.unlocked_ioctl = im3640_device_ioctl, //update park for unlocked_ioctl
	.release = im3640_device_release,
	.read = im3640_device_read,
	.write = im3640_device_write,
	.poll = im3640_device_poll,
};

static struct miscdevice im3640_misc =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = IM3640_NAME,
	.fops = &im3640_device_fops,
};

#define delay_to_jiffies(d) ((d)?msecs_to_jiffies(d):1)
#define actual_delay(d)     (jiffies_to_msecs(delay_to_jiffies(d)))

/* register functions */
#define im3640_read_bits(p,r) \
	((i2c_smbus_read_byte_data((p)->client, r##_REG) & r##_MASK) >> r##_SHIFT)

#define im3640_update_bits(p,r,v) \
        i2c_smbus_write_byte_data((p)->client, r##_REG, \
                ((i2c_smbus_read_byte_data((p)->client,r##_REG) & ~r##_MASK) | ((v) << r##_SHIFT)))

/*
 * Device dependant operations
 */

static int im3640_read_2byte(struct im3640_data *im3640, int reg, s16 *val)
{
        struct i2c_client *client = im3640->client;
	u8 buf;

        if (i2c_smbus_read_i2c_block_data(client, reg, 1, &buf) != 1)
        {
                dev_err(&client->dev,"I2C block read error: addr=0x%02x, len=%d\n",reg, 1);
		return -1;
        }
	*val = (int)(buf << 8);
        if (i2c_smbus_read_i2c_block_data(client, reg+1, 1, &buf) != 1)
        {
                dev_err(&client->dev,"I2C block read error: addr=0x%02x, len=%d\n",reg, 1);
		return -1;
        }
	*val |= buf;
	return 0;
}

static void im3640_measure(struct im3640_data *im3640, struct sensor_data *sensor)
{
	int pos = atomic_read(&im3640->position);
        int i,j;
	s16 val;
	int raw[3],data[3];

	for(i=0;i<3;i++) raw[i] = data[i] = 0;
//acc
	if(!im3640_read_2byte(im3640,IM3640_ACC_X_REGH,&val))
	{
		raw[0] = val;
	}
	if(!im3640_read_2byte(im3640,IM3640_ACC_Y_REGH,&val))
	{
		raw[1] = val;
	}
	if(!im3640_read_2byte(im3640,IM3640_ACC_Z_REGH,&val))
	{
		raw[2] = val;
	}
	for(i=0;i<3;i++) {
		data[i] = 0;
		for(j=0;j<3;j++) data[i] += raw[j] * im3640_position_map[pos][i][j];
	}
	mutex_lock(&im3640->data_mutex);
	for(i=0;i<3;i++) sensor->accel[i] = data[i] ;
	mutex_unlock(&im3640->data_mutex);
//magetic
	if(!im3640_read_2byte(im3640,IM3640_MAG_X_REGH,&val))
	{
		raw[0] = val;
	}
	if(!im3640_read_2byte(im3640,IM3640_MAG_Y_REGH,&val))
	{
		raw[1] = val;
	}
	if(!im3640_read_2byte(im3640,IM3640_MAG_Z_REGH,&val))
	{
		raw[2] = val;
	}
	for(i=0;i<3;i++) {
		data[i] = 0;
		for(j=0;j<3;j++) data[i] += raw[j] * im3640_position_map[pos][i][j];
//		data[i] = raw[i];
	}
	mutex_lock(&im3640->data_mutex);
	for(i=0;i<3;i++) sensor->compass[i] = data[i] ;
	mutex_unlock(&im3640->data_mutex);
//orientation
	if(!im3640_read_2byte(im3640,IM3640_AZIMUTH_REGH,&val))
	{
		raw[0] = val;
	}
	if(!im3640_read_2byte(im3640,IM3640_PITCH_REGH,&val))
	{
		raw[1] = val;
	}
	if(!im3640_read_2byte(im3640,IM3640_ROLL_REGH,&val))
	{
		raw[2] = val;
	}
	// azimuth
	data[0] = (raw[0] + 90) % 360;
	// pitch
	data[1] = (-1)*raw[2];
	if((sensor->accel[2] <= 0) && (sensor->accel[1] >= 0)) {
		data[1] = (-1)*(180+data[1]);
	} else if((sensor->accel[2] < 0) && (sensor->accel[1] < 0)) {
		data[1] = 180-data[1];
	}
	// roll
	data[2] = (-1)*raw[1];
	
	mutex_lock(&im3640->data_mutex);
	for(i=0;i<3;i++) sensor->ori[i] = data[i] ;
	mutex_unlock(&im3640->data_mutex);
}


static void im3640_work_func(struct work_struct *work)
{
	struct im3640_data *im3640 = container_of((struct delayed_work *)work,
						  struct im3640_data, work);
	struct sensor_data sensor;
	unsigned long delay = delay_to_jiffies(atomic_read(&im3640->delay));

	im3640_measure(im3640, &sensor);

	// Acceleration
	input_report_abs(im3640->input, ABS_X, sensor.accel[0]);
	input_report_abs(im3640->input, ABS_Y, sensor.accel[1]);
	input_report_abs(im3640->input, ABS_Z, sensor.accel[2]);

	// Magnetic field
	input_report_abs(im3640->input, ABS_RX, sensor.compass[0]);
	input_report_abs(im3640->input, ABS_RY, sensor.compass[1]);
	input_report_abs(im3640->input, ABS_RZ, sensor.compass[2]);

	// Orientation
	input_report_abs(im3640->input, ABS_HAT0X, sensor.ori[0]);
	input_report_abs(im3640->input, ABS_HAT0Y, sensor.ori[1]);
	input_report_abs(im3640->input, ABS_BRAKE, sensor.ori[2]);
	input_sync(im3640->input);

//printk(KERN_INFO "KERNEL Debug --- acc[%d,%d,%d], compass[%d,%d,%d], ori[%d,%d,%d]\r\n",sensor.accel[0], sensor.accel[1],sensor.accel[2],sensor.compass[0],sensor.compass[1],sensor.compass[2],sensor.ori[0],sensor.ori[1],sensor.ori[2]);
	mutex_lock(&im3640->data_mutex);
	im3640->last = sensor;
	mutex_unlock(&im3640->data_mutex);

	schedule_delayed_work(&im3640->work, delay);
}

/*
 * Input device interface
 */
static int im3640_input_init(struct im3640_data *im3640)
{
	struct input_dev *dev;
	int err;

	dev = input_allocate_device();
	if (!dev)
		return -ENOMEM;

	dev->name = "im3640";
	dev->id.bustype = BUS_I2C;

	set_bit(EV_ABS, dev->evbit);
	set_bit(EV_SYN, dev->evbit);

	//Orientation
	input_set_abs_params(dev, ABS_RX, 0, 359, 0, 0);
	input_set_abs_params(dev, ABS_RY, -89, 89, 0, 0);
	input_set_abs_params(dev, ABS_RZ, -89, 89, 0, 0);

	// Acceleration
	input_set_abs_params(dev, ABS_X, ABSMIN_2G, ABSMAX_2G, 0, 0);
	input_set_abs_params(dev, ABS_Y, ABSMIN_2G, ABSMAX_2G, 0, 0);
	input_set_abs_params(dev, ABS_Z, ABSMIN_2G, ABSMAX_2G, 0, 0);

	// Magnetic field
	input_set_abs_params(dev, ABS_HAT0X, -20480, 20479, 0, 0);
	input_set_abs_params(dev, ABS_HAT0Y, -20480, 20479, 0, 0);
	input_set_abs_params(dev, ABS_BRAKE, -20480, 20479, 0, 0);

	input_set_drvdata(dev, im3640);

	err = input_register_device(dev);
	if (err < 0) {
		input_free_device(dev);
		return err;
	}
	im3640->input = dev;

	return 0;
}

static void im3640_input_fini(struct im3640_data *im3640)
{
        struct input_dev *dev = im3640->input;

        input_unregister_device(dev);
        input_free_device(dev);
}

static void im3640_set_enable(struct device *dev, int enable)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct im3640_data *im3640 = i2c_get_clientdata(client);
	int delay = atomic_read(&im3640->delay);

	mutex_lock(&im3640->enable_mutex);

	if (enable)
	{                   /* enable if state will be changed */
		if (!atomic_cmpxchg(&im3640->enable, 0, 1))
		{
//JNJ			im3640_power_up(im3640);
			schedule_delayed_work(&im3640->work,
					      delay_to_jiffies(delay) + 1);
		}
	}
	else
	{                        /* disable if state will be changed */
		if (atomic_cmpxchg(&im3640->enable, 1, 0))
		{
			cancel_delayed_work_sync(&im3640->work);
//JNJ			im3640_power_down(im3640);
		}
	}
	atomic_set(&im3640->enable, enable);

	mutex_unlock(&im3640->enable_mutex);
}

static int im3640_get_enable(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct im3640_data *im3640 = i2c_get_clientdata(client);

	return atomic_read(&im3640->enable);
}

static int im3640_get_delay(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct im3640_data *im3640 = i2c_get_clientdata(client);

	return atomic_read(&im3640->delay);
}

static void im3640_set_delay(struct device *dev, int delay)
{
	int delay_ms;
	struct i2c_client *client = to_i2c_client(dev);
	struct im3640_data *im3640 = i2c_get_clientdata(client);

	atomic_set(&im3640->delay, delay);

	mutex_lock(&im3640->enable_mutex);

	if (im3640_get_enable(dev))
	{
		delay_ms = delay; 
		cancel_delayed_work_sync(&im3640->work);
		schedule_delayed_work(&im3640->work,
				      delay_to_jiffies(delay_ms) + 1);
	}

	mutex_unlock(&im3640->enable_mutex);
}

static int im3640_get_position(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct im3640_data *im3640 = i2c_get_clientdata(client);

	return atomic_read(&im3640->position);
}

static void im3640_set_position(struct device *dev, int position)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct im3640_data *im3640 = i2c_get_clientdata(client);

	atomic_set(&im3640->position, position);
}

/*
 * sysfs device attributes
 */
static ssize_t im3640_enable_show(struct device *dev,
				  struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "1\n");
}

static ssize_t im3640_enable_store(struct device *dev,
				   struct device_attribute *attr,
				   const char *buf, size_t count)
{
	unsigned long enable = simple_strtoul(buf, NULL, 10);

	if ((enable == 0) || (enable == 1)){
	   im3640_set_enable(dev, enable);
	}

	return count;
}

static ssize_t im3640_delay_show(struct device *dev,
				 struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", im3640_get_delay(dev));
}

static ssize_t im3640_delay_store(struct device *dev,
				  struct device_attribute *attr,
				  const char *buf, size_t count)
{
	unsigned long delay = simple_strtoul(buf, NULL, 10);

	im3640_set_delay(dev, delay);

	return count;
}

static ssize_t im3640_wake_store(struct device *dev,
				 struct device_attribute *attr,
				 const char *buf, size_t count)
{
	struct input_dev *input = to_input_dev(dev);
	static atomic_t serial = ATOMIC_INIT(0);

	input_report_abs(input, ABS_MISC, atomic_inc_return(&serial));

	return count;
}

static ssize_t im3640_data_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	struct input_dev *input = to_input_dev(dev);
	struct im3640_data *im3640 = input_get_drvdata(input);
	struct sensor_data sensor;

	mutex_lock(&im3640->data_mutex);
	sensor = im3640->last;
	mutex_unlock(&im3640->data_mutex);

	return sprintf(buf, "%d %d %d\n%d %d %d\n%d %d %d\n", sensor.ori[0], sensor.ori[1],sensor.ori[2], sensor.accel[0], sensor.accel[1], sensor.accel[2], sensor.compass[0], sensor.compass[1], sensor.compass[2]);
}

static DEVICE_ATTR(enable, S_IRUGO|S_IWUSR|S_IWGRP,
		   im3640_enable_show, im3640_enable_store);
static DEVICE_ATTR(delay, S_IRUGO|S_IWUSR|S_IWGRP,
		   im3640_delay_show, im3640_delay_store);
static DEVICE_ATTR(wake, S_IWUSR|S_IWGRP,
		   NULL, im3640_wake_store);
static DEVICE_ATTR(data, S_IRUGO,
		   im3640_data_show, NULL);

static struct attribute *im3640_attributes[] = {
	&dev_attr_enable.attr,
	&dev_attr_delay.attr,
	&dev_attr_wake.attr,
	&dev_attr_data.attr,
	NULL
};

static struct attribute_group im3640_attribute_group = {
	.attrs = im3640_attributes
};

/* im3640 file operation */
static int im3640_device_open( struct inode* inode, struct file* file)
{
	return 0;
}

static int im3640_device_release( struct inode* inode, struct file* file)
{
	return 0;
}

static long im3640_device_ioctl( struct file* file, unsigned int cmd, unsigned long arg) //update park for unlocked_ioctl
{
	void __user *argp = (void __user *)arg;
	int ret=0;
	int kbuf=0;
	int axis_data[9];
	switch( cmd )
	{
	case IM3640_IOCTL_ENABLE:
		im3640_set_enable(&im3640_ptr->client->dev, 1);
		break;
	case IM3640_IOCTL_DISABLE:
//JNJ		im3640_set_enable(&im3640_ptr->client->dev, 0);
		break;
	case IM3640_IOCTL_IS_ENABLE:
		kbuf = im3640_get_enable(&im3640_ptr->client->dev);
		if(copy_to_user(argp, &kbuf, sizeof(kbuf)))
			return -EFAULT;
		break;
	case IM3640_IOCTL_DELAY_GET:
		kbuf = im3640_get_delay(&im3640_ptr->client->dev);
		if(copy_to_user(argp, &kbuf, sizeof(kbuf)))
			return -EFAULT;
		break;
	case IM3640_IOCTL_DELAY_SET:
		if(copy_from_user(&kbuf, argp, sizeof(kbuf)))
			return -EFAULT;
		im3640_set_delay(&im3640_ptr->client->dev, kbuf);
		break;
	case IM3640_IOCTL_DATA:
		axis_data[0] = im3640_ptr->last.accel[0];
		axis_data[1] = im3640_ptr->last.accel[1];
		axis_data[2] = im3640_ptr->last.accel[2];
		axis_data[3] = im3640_ptr->last.compass[0];
		axis_data[4] = im3640_ptr->last.compass[1];
		axis_data[5] = im3640_ptr->last.compass[2];
		axis_data[6] = im3640_ptr->last.ori[0];
		axis_data[7] = im3640_ptr->last.ori[1];
		axis_data[8] = im3640_ptr->last.ori[2];
		if(copy_to_user(argp, &axis_data, sizeof(axis_data)))
			return -EFAULT;
		break;
	default:
		return -ENOTTY;
	}

	return ret;
}

static ssize_t im3640_device_read( struct file *filp, char *buf, size_t count, loff_t *ofs )
{
	return 0;
}

static ssize_t im3640_device_write( struct file *filp, const char *buf, size_t count, loff_t *ofs )
{
	return 0;
}

static unsigned int im3640_device_poll( struct file *filp, struct poll_table_struct *pwait )
{
	return 0;
}

/*
 * I2C client
 */
static int im3640_detect(struct i2c_client *client, struct i2c_board_info *info)
{
	int id;

	id = i2c_smbus_read_byte_data(client, IM3640_DEV_ID_REG);
	if (id != IM3640_DEV_ID) {
		printk("IM3640 Device ID read failed, %d\r\n",id);
		return -ENODEV;
	}

	return 0;
}

static int im3640_set_default_mode(struct i2c_client *client)
{
	i2c_smbus_write_byte_data(client, IM3640_AMS_POWER_REG, IM3640_AMS_POWER_ACTIVE);
	i2c_smbus_write_byte_data(client, IM3640_NAVI_MODE_REG, IM3640_NAVI_MODE_NORMAL);

	return 0;
}

static int im3640_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct im3640_data *im3640;
	int err;

	/* setup private data */
	im3640 = kzalloc(sizeof(struct im3640_data), GFP_KERNEL);
	if (!im3640)
	{
		err = -ENOMEM;
		goto error_0;
	}
	mutex_init(&im3640->enable_mutex);
	mutex_init(&im3640->data_mutex);

	/* setup i2c client */
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C))
	{
		err = -ENODEV;
		goto error_1;
	}
	i2c_set_clientdata(client, im3640);
	im3640->client = client;

	/* detect and init hardware */
	if ((err = im3640_detect(client, NULL)))
		goto error_1;

	im3640_set_default_mode(client);

	im3640_set_delay(&client->dev, 100);
	im3640_set_position(&client->dev, CONFIG_SENSORS_IM3640_POSITION);
	//added park for ics
	//#if defined(CONFIG_PHONE_MODE)
		//im3640_set_position(&client->dev, 4);
	//#else
	//	im3640_set_position(&client->dev, 5);
	//#endif

	//dev_info(&client->dev, "%s found\n", id->name);

	/* setup driver interfaces */
	INIT_DELAYED_WORK(&im3640->work, im3640_work_func);

	err = im3640_input_init(im3640);
	if (err < 0)
		goto error_1;

	err = sysfs_create_group(&im3640->input->dev.kobj, &im3640_attribute_group);
	if (err < 0)
		goto error_2;

	err = misc_register(&im3640_misc);
	if (err < 0)
		goto error_3;

	im3640_ptr = im3640;

printk("******* IM3640 proved *********\r\n");
	return 0;

error_3:
	sysfs_remove_group(&im3640->input->dev.kobj, &im3640_attribute_group);
error_2:
	im3640_input_fini(im3640);
error_1:
	kfree(im3640);
error_0:
	return err;
}

static int im3640_remove(struct i2c_client *client)
{
	struct im3640_data *im3640 = i2c_get_clientdata(client);

	misc_deregister(&im3640_misc);
	sysfs_remove_group(&im3640->input->dev.kobj, &im3640_attribute_group);
	im3640_input_fini(im3640);
	kfree(im3640);

	return 0;
}

static int im3640_suspend(struct i2c_client *client, pm_message_t mesg)
{
	return 0;
}

static int im3640_resume(struct i2c_client *client)
{
	return 0;
}

static const struct i2c_device_id im3640_id[] =
{
	{IM3640_NAME, 0},
	{},
};

MODULE_DEVICE_TABLE(i2c, im3640_id);

struct i2c_driver im3640_driver =
{
	.driver = {
		.name = "im3640",
		.owner = THIS_MODULE,
	},
	.probe = im3640_probe,
	.remove = im3640_remove,
	.suspend = im3640_suspend,
	.resume = im3640_resume,
	.id_table = im3640_id,
};

/*
 * Module init and exit
 */
static int __init im3640_init(void)
{
	return i2c_add_driver(&im3640_driver);
}
module_init(im3640_init);

static void __exit im3640_exit(void)
{
	i2c_del_driver(&im3640_driver);
}
module_exit(im3640_exit);

MODULE_DESCRIPTION("iM-3640 accel+compass driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(IM3640_VERSION);

