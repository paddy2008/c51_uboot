#ifndef __Efm8lb12_i2c_master_h__
#define __Efm8lb12_i2c_master_h__

#include <SI_EFM8LB1_Register_Enums.h>

typedef struct
{
	uint8_t 		  ackPollFlag  : 1;   	//当没有收到ACK的时候，自动重发start信号的标志位
	uint8_t 		  rwFlag       : 1; 	//读写标志位
	uint8_t 		  busyFlag     : 1;		//总线busy标志位
	uint8_t 		  offAddrFlag  : 1;		//偏移地址发送结束标志位
	uint8_t 		  startFlag    : 1;		//start信号发送结束标志位
	uint8_t 		  rwChangeFlag : 1;		//i2c方向改变标志位，当读的时候，必须先写地址，然后才根据写入的地址进行读，所以i2c读的过程中有方向改变
	uint8_t  		  baseAddr;				//i2c的设备地址
	uint8_t  		  len;					//i2c读写字节的长度，最大256bytes
	uint8_t 		  cnt;					//偏移地址的发送字节的个数,因为有个设备地址是8bit，有的是12bit，还有16bit，当偏移地址大于8ibt时，先发送高字节，再发送低字节
	uint8_t  		  *buff;				//读写buff指针
	uint16_t 		  offAddr;				//保存偏移地址
	uint8_t 		  widthFlag    : 1; 	//偏移地址宽度标志位, 0:8bit, 1:大于8bit小于等于16bit
	uint16_t 		  freq;					//i2c master通信的时钟频率
}I2C_MASTER_STRUCT_T;

#define I2C_WRITE   (0x00)
#define I2C_READ    (0x01)

// Status vector - top 4 bits only
#define  SMB_MTSTA      (0xE0)           // (MT) start transmitted,MT replace Master and Transmitter
#define  SMB_MTDB       (0xC0)           // (MT) data byte transmitted
#define  SMB_MRDB       (0x80)           // (MR) data byte received

extern void 	i2c_master_hw_reset(void);
extern void     i2c_master_hw_init(void);
extern void     i2c_master_hw_set_freq(uint16_t freq);
extern uint16_t i2c_master_hw_get_freq(void);
extern uint8_t  i2c_master_hw_read(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len);
extern uint8_t  i2c_master_hw_write(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len);


#endif

