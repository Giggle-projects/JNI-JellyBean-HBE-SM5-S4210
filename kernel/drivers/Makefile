#
# Makefile for misc devices that really don't fit anywhere else.
#

obj-$(CONFIG_INPUT_IM3640_I2C)	  	 += im3640.o
obj-$(CONFIG_MPU_SENSORS_MPU3050)	 += mpu-dev.o
obj-$(CONFIG_VIBRATOR_HANBACK) 		 += vibrator.o
obj-$(CONFIG_PROXIMITY_SENSOR)		 += proximity.o
ifeq ($CONFIG_MACH_SM7S4412),y)
obj-y                           += m2/
endif
obj-y                           += fpga/

obj-$(CONFIG_KEYPAD_FPGA)	+= hanback_fpga_keypad.o
obj-$(CONFIG_KEYPAD_XXX_S4210)	+= s4210_keypad.o s4210_keypad_sysfs.o
obj-$(CONFIG_KEYPAD_SM7S4412)		 += s4412_keypad.o s4412_keypad_sysfs.o
obj-$(CONFIG_BOARD_ID_SM7S4412)		 += s4412_board_id.o
ifeq ($(CONFIG_MACH_SM7S4412),y)
obj-y					 += buffer.o
endif
