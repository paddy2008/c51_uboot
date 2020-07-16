#ifndef __EFM8LB1_cmd_h__
#define __EFM8LB1_cmd_h__

#include <SI_EFM8LB1_Register_Enums.h>

#define CMD_STR_MAX_LENGTH  	(20)
#define RW_MAX_PARAS 			(16)

#define MSA_LOWER_PAGE_FLASH_AREA  (86)

SI_SBIT (LPMODE, SFR_P1, 4);
SI_SBIT (RESET, SFR_P1, 5);
SI_SBIT (INTL, SFR_P1, 6);
SI_SBIT (MODPRS, SFR_P1, 7);
SI_SBIT (MODSEL, SFR_P2, 0);

#define  LINE_FEED   		(0xA)   //换行符
#define  BACKSPACE          (0x8)   //退格符
#define  CARRIAGE_RETURN    (0xD)   //回车符
#define  BLANKSPACE			(0x20)  //空格符
#define  ESCAPE      		(0x1B)  //键盘上的向上箭头符
#define  UNDERLINE 	        (0x5F)  //下划线
#define  INVALID			(0xFF)


#define  OUTPUT_ENABLE      (0x1)
#define  OUTPUT_DISABLE     (0x0)

#define  CHANNEL_REVERSE(order,ch)  ((order) == (channel_reverse))?(3 - (ch)):(ch)

#define CHECK_DDMI_CALI_VCC_OFFSET(x)    ((((x) >= -2) && ((x) <= 2)) ? 1: 0)    //temp_offset range
#define CHECK_DDMI_CALI_TEMP_OFFSET(x)   ((((x) >= -40) && ((x) <= 40)) ? 1: 0)	 //vcc_offset range
#define CHECK_DDMI_CALI_BIAS_OFFSET(x)   ((((x) >= -4) && ((x) <= 128)) ? 1: 0)	 //bias_offset range
#define CHECK_DDMI_CALI_POWER_OFFSET(x)  ((((x) >= -15) && ((x) <= 5)) ? 1: 0)	 //power_offset range
#define CHECK_DDMI_CALI_RSSI_OFFSET(x)   ((((x) >= 0) && ((x) <= 1280))? 1: 0)	 //rssi_offset range
#define CHECK_DDMI_CALI_TXBIAS_OFFSET(x) ((((x) >= 0) && ((x) <= 16))? 1: 0)	 //tx_bias_offset range

typedef enum
{
	channel_normal,
	channel_reverse
}ORDER;

typedef  enum
{
	AOC_101,
	CWDM4_206,
	CWDM4_207,
	LR4_301,
	UNKNOW,
}MODULTE_TYPE;

typedef enum
{
	DISABLE_LAESER = 0x0,
	ENABLE_LAESER  = 0x1,
};

typedef enum
{
	ACCESS_BY_FLASH_CMD   = 0x0,
	ACCESS_BY_OPRW_CMD	  = 0x1,
	ACCESS_BY_SET_GET_CMD = 0x2,
	ACCESS_BY_OTHER_CMD	  = 0x3,
	UNKNOW_COMMAND			   ,
}CMD_ACCESS_TYPE_T;

typedef enum
{
	 READ_CMD,
	 WRITE_CMD,
	 UNKNOW_CMD,
}CMD_RW_TYPE_T;

typedef enum
{
	MACOM_31044_RXCDR_DEV_ADDR 	 = 0x16,
	MACOM_31045_TXCDR_DEV_ADDR 	 = 0x18,
	SEMTECH_2108_TXCDR_DEV_ADDR  = 0xA8,
	SEMTECH_2109_RXCDR_DEV_ADDR  = 0xB8,
	SEMTECH_2105_RXCDR_DEV_ADDR  = 0x24,
	TOSA_4820_TX_DEV_ADDR  		 = 0x20,
}CDR_DEV_ADDR;



typedef struct {
	uint8_t       *readData;
	uint8_t       *writeData;
	uint16_t       rwByteNum;
	uint8_t        devAddr;
	uint8_t        i2c_dbg_mode;  //0:normal i2c mode,1:dbg i2c mode
	uint16_t  	   offsetAddr;
	CMD_RW_TYPE_T  rwType;
}I2C_DATA_T;

void 		 cmd_init();
uint8_t   	 smb_read_multi_byte_test(uint8_t BaseAddr,uint16_t OffsetAddr,uint8_t *Pdata,uint16_t Length);

#endif

