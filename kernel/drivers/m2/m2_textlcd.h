/*
 * TextLCD Device Driver
 *  Hanback Electronics Co.,ltd
 * File : m2_textlcd.h
 * Date : Oct, 2012
 */ 

#define DRIVER_AUTHOR           "Hanback"
#define DRIVER_DESC             "m2_textlcd test program"	

#define TEXTLCD_NAME   		"m2_textlcd"		
#define TEXTLCD_MODULE_VERSION  "TEXT LCD PORT V1.0"	

#define TEXTLCD_RS		EXYNOS4_GPF2(0)
#define TEXTLCD_RW		EXYNOS4_GPF2(1)
#define TEXTLCD_EN		EXYNOS4_GPF2(2)

#define TEXTLCD_DATA0		EXYNOS4_GPF1(0)
#define TEXTLCD_DATA1		EXYNOS4_GPF1(1)
#define TEXTLCD_DATA2		EXYNOS4_GPF1(2)
#define TEXTLCD_DATA3		EXYNOS4_GPF1(3)
#define TEXTLCD_DATA4		EXYNOS4_GPF1(4)
#define TEXTLCD_DATA5		EXYNOS4_GPF1(5)
#define TEXTLCD_DATA6		EXYNOS4_GPF1(6)
#define TEXTLCD_DATA7		EXYNOS4_GPF1(7)

#define M2_TEXTLCD_BASE		0x56
#define M2_TEXTLCD_FUNCTION_SET    _IO(M2_TEXTLCD_BASE,0x31)

#define M2_TEXTLCD_DISPLAY_ON _IO(M2_TEXTLCD_BASE,0x32)
#define M2_TEXTLCD_DISPLAY_OFF _IO(M2_TEXTLCD_BASE,0x33)
#define M2_TEXTLCD_DISPLAY_CURSOR_ON _IO(M2_TEXTLCD_BASE,0x34)
#define M2_TEXTLCD_DISPLAY_CURSOR_OFF _IO(M2_TEXTLCD_BASE,0x35)

#define M2_TEXTLCD_CURSOR_SHIFT_RIGHT    _IO(M2_TEXTLCD_BASE,0x36)
#define M2_TEXTLCD_CURSOR_SHIFT_LEFT    _IO(M2_TEXTLCD_BASE,0x37)

#define M2_TEXTLCD_ENTRY_MODE_SET  _IO(M2_TEXTLCD_BASE,0x38)
#define M2_TEXTLCD_RETURN_HOME     _IO(M2_TEXTLCD_BASE,0x39)
#define M2_TEXTLCD_CLEAR           _IO(M2_TEXTLCD_BASE,0x3a)

#define M2_TEXTLCD_DD_ADDRESS_1	_IO(M2_TEXTLCD_BASE,0x3b)
#define M2_TEXTLCD_DD_ADDRESS_2	_IO(M2_TEXTLCD_BASE,0x3c)
#define M2_TEXTLCD_WRITE_BYTE      _IO(M2_TEXTLCD_BASE,0x3d)

static int textlcd_usage = 0;

void setcommand(unsigned char command);
void writebyte(unsigned char cData);
void initialize_textlcd(void);
int function_set();
int display_control(int display_enable);
int cursor_control(int cursor_enable);
int cursor_shift(int set_shift);
int entry_mode_set();
int return_home(void);
int clear_display(void);
int set_ddram_address(int pos);
