#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
#endif
// ---------------------------------------------------------------------------
#include <cust_adc.h>    	// zhoulidong  add for lcm detect
#define MIN_VOLTAGE (500)		//++++rgk bug-id:no modify by yangjuwei 20140401
#define MAX_VOLTAGE (700)	//++++rgk bug-id:no modify by yangjuwei 20140401

//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)
#define LCM_ID		0x94	//++++rgk bug-id:no modify by yangjuwei 20140401

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

unsigned static int lcm_esd_test = FALSE;      ///only for ESD test

// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util ;

#define SET_RESET_PIN(v)    			(lcm_util.set_reset_pin((v)))

#define UDELAY(n) 				(lcm_util.udelay(n))
#define MDELAY(n) 				(lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)			lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)		lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)			lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)							lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)				lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)							lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE						0

// zhoulidong  add for lcm detect ,read adc voltage
extern int IMM_GetOneChannelValue(int dwChannel, int data[4], int* rawdata);

static LCM_setting_table_V3 lcm_initialization_setting[] = {
	
//,0xSet,0xEXTC
{0x39,0xB9,3,{0xFF,0x83,0x94}},

//,0xSet,0xMIPI
{0x39,0xBA,2,{0x33,0x83}},

//,0xSet,0xPower
{0x39,0xB1,15,{0x6C,0x12,0x12,0x26,0x04,0x11,0xF1,0x81,0x3A,0x54,0x23,0x80,0xC0,0xD2,0x58}},

//,0xSet,0xDisplay
{0x39,0xB2,11,{0x00,0x64,0x0E,0x0D,0x22,0x1C,0x08,0x08,0x1C,0x4D,0x00}},

//,0xSet,0xCYC
{0x39,0xB4,12,{0x00,0xFF,0x51,0x5A,0x59,0x5A,0x03,0x5A,0x01,0x70,0x01,0x70}},

//,0xSet,0xGIP
{0x15,0xBC,1,{0x07}},
{0x39,0xBF,3,{0x41,0x0E,0x01}},

{0x39,0xD3,37,{0x00,0x0F,0x00,0x40,0x07,0x10,0x00,0x08,0x10,0x08,0x00,0x08,0x54,0x15,0x0E,0x05,0x0E,0x02,0x15,0x06,0x05,0x06,0x47,0x44,0x0A,0x0A,0x4B,0x10,0x07,0x07,0x08,0x00,0x00,0x00,0x0A,0x00,0x01}},

//,0xSet,0xForward,0xGIP

{0x39,0xD5,44,{0x1A,0x1A,0x1B,0x1B,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x24,0x25,0x18,0x18,0x26,0x27,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x20,0x21,0x18,0x18,0x18,0x18}},
//,0xSet,0xBackward,0xGIP

{0x39,0xD6,44,{0x1A,0x1A,0x1B,0x1B,0x0B,0x0A,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,0x21,0x20,0x58,0x58,0x27,0x26,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x25,0x24,0x18,0x18,0x18,0x18}},
//,0xSet,0xPanel

{0x15,0xCC,1,{0x09}},

{0x39,0xC0,2,{0x30,0x14}},

//,0xSet,0xTCON,0xOption


{0x39,0xC7,4,{0x00,0xC0,0x40,0xC0}},//20140620
{0x39,0xB6,2,{0x6B,0x6B}},//20140620
{0x39,0xE0,42,{0x00,0x0A,0x0F,0x24,0x3A,0x3F,0x20,0x3B,0x08,0x0D,0x0E,0x16,0x0F,0x12,0x15,0x13,0x15,0x09,0x12,0x12,0x18,0x00,0x0A,0x0F,0x24,0x3A,0x3F,0x20,0x3B,0x08,0x0D,0x0E,0x16,0x0F,0x12,0x15,0x13,0x15,0x09,0x12,0x12,0x18}},

{0x15,0xBD,1,{0x00}},

{0x39,0xC1,43,{0x01,0x00,0x06,0x0C,0x14,0x1D,0x27,0x2F,0x38,0x41,0x49,0x51,0x59,0x61,0x69,0x71,0x79,0x81,0x89,0x91,0x99,0xA1,0xA9,0xB2,0xB9,0xC1,0xCA,0xD1,0xD8,0xE2,0xEA,0xF0,0xF7,0xFF,0x38,0xFC,0x3F,0x0B,0xC1,0x13,0xF1,0x0D,0xC0}},

{0x15,0xBD,1,{0x01}},
{0x39,0xC1,42,{0x00,0x06,0x0C,0x14,0x1D,0x27,0x2F,0x38,0x41,0x49,0x51,0x59,0x61,0x69,0x71,0x79,0x81,0x89,0x91,0x99,0xA1,0xA9,0xB2,0xB9,0xC1,0xCA,0xD1,0xD8,0xE2,0xEA,0xF0,0xF7,0xFF,0x38,0xFC,0x3F,0x0B,0xC1,0x13,0xF1,0x0D,0xC0}},

{0x15,0xBD,1,{0x02}},
{0x39,0xC1,42,{0x00,0x06,0x0C,0x14,0x1D,0x27,0x2F,0x38,0x41,0x49,0x51,0x59,0x61,0x69,0x71,0x79,0x81,0x89,0x91,0x99,0xA1,0xA9,0xB2,0xB9,0xC1,0xCA,0xD1,0xD8,0xE2,0xEA,0xF0,0xF7,0xFF,0x38,0xFC,0x3F,0x0B,0xC1,0x13,0xF1,0x0D,0xC0}},

//,0xSleep,0xOut
{0x05,0x11,0,{}},
{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 200, {}},   
//,0xDisplay,0xON
{0x05,0x29,0,{}},
{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},  
};

static void lcm_register()
{
	unsigned int data_array[35];
	data_array[0] = 0x00043902;
	data_array[1] = 0x9483ffb9;
	dsi_set_cmdq(data_array, 2, 1);
	data_array[0] = 0x00113902;
	data_array[1] = 0x008213ba;
	data_array[2] = 0x1000c516;
	data_array[3] = 0x03240fff;
	data_array[4] = 0x20252421;
	data_array[5] = 0x00000008;
	dsi_set_cmdq(data_array, 6, 1);
	data_array[0] = 0x00113902;
	data_array[1] = 0x040001b1;
	data_array[2] = 0x1111018a;
	data_array[3] = 0x3f3f372f;
	data_array[4] = 0xe6011247;
	data_array[5] = 0x000000e2;
	dsi_set_cmdq(data_array, 6, 1);
	data_array[0] = 0x00073902;
	data_array[1] = 0x08c800b2;
	data_array[2] = 0x00220004;
	dsi_set_cmdq(data_array, 3, 1);
	data_array[0] = 0x00173902;
	data_array[1] = 0x320680b4;
	data_array[2] = 0x15320310;
	data_array[3] = 0x08103208;
	data_array[4] = 0x05430433;
	data_array[5] = 0x06430437;
	data_array[6] = 0x00066161;
	dsi_set_cmdq(data_array, 7, 1);
	data_array[0] = 0x00053902;
	data_array[1] = 0x100006bf;
	data_array[2] = 0x00000004;
	dsi_set_cmdq(data_array, 3, 1);
	data_array[0] = 0x00033902;
	data_array[1] = 0x00170cc0;
	dsi_set_cmdq(data_array, 2, 1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000000b6;
	dsi_set_cmdq(data_array, 2, 1);
	data_array[0] = 0x00213902;
	data_array[1] = 0x000000d5;
	data_array[2] = 0x01000a00;
	data_array[3] = 0x0000cc00;
	data_array[4] = 0x88888800;
	data_array[5] = 0x88888888;
	data_array[6] = 0x01888888;
	data_array[7] = 0x01234567;
	data_array[8] = 0x88888823;
	data_array[9] = 0x00000088;
	dsi_set_cmdq(data_array, 10, 1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000009cc;
	dsi_set_cmdq(data_array, 2, 1);
	data_array[0] = 0x00053902;
	data_array[1] = 0x001000c7;
	data_array[2] = 0x00000010;
	dsi_set_cmdq(data_array, 3, 1);
	data_array[0] = 0x002b3902;
	data_array[1] = 0x060400e0;
	data_array[2] = 0x173f332b;
	data_array[3] = 0x0d0e0a34;
	data_array[4] = 0x13111311;
	data_array[5] = 0x04001710;
	data_array[6] = 0x3f332b06;
	data_array[7] = 0x0e0a3417;
	data_array[8] = 0x1113110d;
	data_array[9] = 0x0b171013;
	data_array[10] = 0x0b110717;
	data_array[11] = 0x00110717;
	dsi_set_cmdq(data_array, 12, 1);
	data_array[0] = 0x00023902;
	data_array[1] = 0x000032d4;
	dsi_set_cmdq(data_array, 2, 1);
	data_array[0] = 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(250);
	data_array[0] = 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	MDELAY(50);
}

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
	memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{

	memset(params, 0, sizeof(LCM_PARAMS));
	
	params->type   = LCM_TYPE_DSI;

	params->width  = FRAME_WIDTH;
	params->height = FRAME_HEIGHT;

#if (LCM_DSI_CMD_MODE)
	params->dsi.mode   = CMD_MODE;
#else
	params->dsi.mode   = SYNC_PULSE_VDO_MODE; //SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE; 
#endif
	
	// DSI
	/* Command mode setting */
	//1 Three lane or Four lane
	params->dsi.LANE_NUM				= LCM_FOUR_LANE;
	//The following defined the fomat for data coming from LCD engine.
	params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

	// Video mode setting		
	params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
		
	params->dsi.vertical_sync_active				= 4;
	params->dsi.vertical_backporch					= 12;
	params->dsi.vertical_frontporch					= 15;
	params->dsi.vertical_active_line				= FRAME_HEIGHT; 

	params->dsi.horizontal_sync_active				= 40;
	params->dsi.horizontal_backporch				= 86;
	params->dsi.horizontal_frontporch				= 86;
	params->dsi.horizontal_active_pixel				= FRAME_WIDTH;
	
	//params->dsi.LPX=8; 

	// Bit rate calculation
	params->dsi.PLL_CLOCK = 240;
	//1 Every lane speed
	params->dsi.pll_div1=0;		// div1=0,1,2,3;div1_real=1,2,4,4 ----0: 546Mbps  1:273Mbps
	params->dsi.pll_div2=0;		// div2=0,1,2,3;div1_real=1,2,4,4	
	params->dsi.fbk_div =9;    // fref=26MHz, fvco=fref*(fbk_div+1)*2/(div1_real*div2_real)	


}


static void lcm_init(void)
{
	 
	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(120);

	//lcm_register();
	dsi_set_cmdq_V3(lcm_initialization_setting,sizeof(lcm_initialization_setting)/sizeof(lcm_initialization_setting[0]),1);

	//push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static  LCM_setting_table_V3 lcm_deep_sleep_mode_in_setting[] = {
	// Display off sequence
	{0x05, 0x28, 0, {}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 10, {}},

	// Sleep Mode On
	{0x05, 0x10, 0, {}},
	{REGFLAG_ESCAPE_ID,REGFLAG_DELAY_MS_V3, 120, {}},
};
static void lcm_suspend(void)
{	
		
	dsi_set_cmdq_V3(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting)/sizeof(lcm_deep_sleep_mode_in_setting[0]), 1);
	
	SET_RESET_PIN(1);     
	MDELAY(10);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(100);
}


static void lcm_resume(void)
{
	lcm_init();
	
	//push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}

#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
                       unsigned int width, unsigned int height)
{
	unsigned int x0 = x;
	unsigned int y0 = y;
	unsigned int x1 = x0 + width - 1;
	unsigned int y1 = y0 + height - 1;

	unsigned char x0_MSB = ((x0>>8)&0xFF);
	unsigned char x0_LSB = (x0&0xFF);
	unsigned char x1_MSB = ((x1>>8)&0xFF);
	unsigned char x1_LSB = (x1&0xFF);
	unsigned char y0_MSB = ((y0>>8)&0xFF);
	unsigned char y0_LSB = (y0&0xFF);
	unsigned char y1_MSB = ((y1>>8)&0xFF);
	unsigned char y1_LSB = (y1&0xFF);

	unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);
	
	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);

}
#endif


static unsigned int lcm_compare_id(void)
{
	int array[4];
	char buffer[5];
	char id_high=0;
	char id_low=0;
	int id=0;

	SET_RESET_PIN(1);
	SET_RESET_PIN(0);
	MDELAY(10);
	SET_RESET_PIN(1);
	MDELAY(200);

	array[0] = 0x00043902;
	array[1] = 0x9483FFB9;
	dsi_set_cmdq(array, 2, 1);	

	array[0] = 0x00023700;// read id return two byte,version and id
	dsi_set_cmdq(array, 1, 1);
	
	read_reg_v2(0xF4, buffer, 2);
	id = buffer[0]; //we only need ID
	
	#ifdef BUILD_LK
		printf("hx8394 uboot %s \n", __func__);
		printf("%s id = 0x%08x \n", __func__, id);
	#else
		printk("hx8394 kernel %s \n", __func__);
		printk("%s id = 0x%08x \n", __func__, id);
	#endif
	   
	return (id == LCM_ID) ? 1 : 0;
}

// zhoulidong  add for lcm detect (start)
static int rgk_lcm_compare_id(void)
{
	int data[4] = {0,0,0,0};
	int res = 0;
	int rawdata = 0;
	int lcm_vol = 0;

#ifdef AUXADC_LCM_VOLTAGE_CHANNEL
	res = IMM_GetOneChannelValue(AUXADC_LCM_VOLTAGE_CHANNEL,data,&rawdata);
	if(res < 0)
	{ 
	#ifdef BUILD_LK
		printf("[adc_uboot]: get data error\n");
	#endif
		return 0;
	}
#endif

	lcm_vol = data[0]*1000+data[1]*10;

#ifdef BUILD_LK
	printf("[adc_uboot]: lcm_vol= %d\n",lcm_vol);
#endif
	
	if (lcm_vol>=MIN_VOLTAGE &&lcm_vol <= MAX_VOLTAGE && lcm_compare_id())
	{
		return 1;
	}

	return 0;
}

// zhoulidong  add for lcm detect (end)


// zhoulidong add for eds(start)
static unsigned int lcm_esd_check(void)
{
#ifdef BUILD_LK
	//printf("lcm_esd_check()\n");
#else
	//printk("lcm_esd_check()\n");
#endif 
#ifndef BUILD_LK
	char  buffer[3];
	int   array[4];

	array[0] = 0x00013700;
	dsi_set_cmdq(array, 1, 1);

	read_reg_v2(0x0a, buffer, 1);
	if(buffer[0]==0x1c)
	{
		//#ifdef BUILD_LK
		//printf("%s %d\n FALSE", __func__, __LINE__);
		//#else
		//printk("%s %d\n FALSE", __func__, __LINE__);
		//#endif
		return FALSE;
	}
	else
	{	
		//#ifdef BUILD_LK
		//printf("%s %d\n FALSE", __func__, __LINE__);
		//#else
		//printk("%s %d\n FALSE", __func__, __LINE__);
		//#endif		 
		return TRUE;
	}
#endif

}

static unsigned int lcm_esd_recover(void)
{
	
	#ifdef BUILD_LK
		printf("lcm_esd_recover()\n");
	#else
		printk("lcm_esd_recover()\n");
	#endif	
	
	lcm_init();	

	return TRUE;
}
// zhoulidong add for eds(end)

LCM_DRIVER hx8394_dsi_vdo_truly_cmi_hd720_ips_lcm_drv = 
{
	.name			= "hx8394_dsi_vdo_truly_cmi_hd720_ips",
	.set_util_funcs		= lcm_set_util_funcs,
	.get_params		= lcm_get_params,
	.init			= lcm_init,
	.suspend		= lcm_suspend,
	.resume			= lcm_resume,
	.compare_id		= rgk_lcm_compare_id,
	.esd_check		= lcm_esd_check,
	.esd_recover		= lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
	//.set_backlight	= lcm_setbacklight,
	//.update		= lcm_update,
#endif
};

