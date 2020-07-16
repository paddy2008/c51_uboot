#ifndef __MCU_SAL_I2C_MASTER_H__
#define __MCU_SAL_I2C_MASTER_H__

#include "mcu_sal_def.h"

#ifdef MCU_I2C_MASTER_MODULE_ENABLED

typedef struct
{
	uint8_t 		  half;					//半个周期的时长
	uint8_t  		  readBit	   : 1;		//保存读取的一个bit
	uint8_t 		  widthFlag    : 1; 	//偏移地址宽度标志位, 0:8bit, 1:大于8bit小于等于16bit
	uint16_t 		  freq;					//i2c master通信的时钟频率
}I2C_MASTER_IMITATE_STRUCT_T;


/****************************************************************************
由于SDA设定后，均有一个上升沿，所以设定一个上升沿延时RISE_TIME_DELAY微秒
由于在BIT传送中，SDA都会提前一个Rise_Time_Delay准备；
为了波形对称，前面一个BIT传送完毕后，需要将延时减少一个Rise_Time_Delay
SCL的高电平保持时间则刚好需要时一个半周期
由于函数的调用或者其他代码执行也会消耗一些时间，所以实际的SCL低电平时间将长于
高电平时间一些，整体时钟频率会略小于设定时钟目标速率；
I2C_Master_HoldTime特指START STOP状态时候，SCL SDA变化前的保持时间

因为一个周期包含一个高电平持续时间，一个低电平持续时间，一个上升沿的时间，一个下降沿时间,可以简单的认为高电平持续时间和低
电平持续时间相等，上升沿时间和下降沿时间相等，由于半个周期的时间为 500 / I2C_MASTER_DEF_FREQUENCY，所以当频率
是400khz的时候，这个公式计算的结果为1.25us,如果达不到这个延时，那么是不可能支持400khz的，目前实现的最大只支持200khz
使用GPIO口模拟i2c最重要的就是延时函数的准确性，因为它直接控制着时序的变化。
*****************************************************************************/
#define   FUNC_CALL_ERROR_TIME		   (2)
#define   RISE_TIME_DELAY              (1)
#define   I2C_MASTER_HOLD_TIME         (int)((1 * 1000 / (I2C_MASTER_DEF_FREQUENCY)  - 2 * (RISE_TIME_DELAY)) / 2)  //高电平或低电平的持续时间
#define   I2C_MASTER_DELAY             (500 / I2C_MASTER_DEF_FREQUENCY - RISE_TIME_DELAY)
#define   I2C_MASTER_HAFT_CYCLE        ((int)( 500 / I2C_MASTER_DEF_FREQUENCY))  //半个周期的时长，一个周期等于1/I2C_MASTER_DEF_FREQUENCY, 因为I2C_MASTER_DEF_FREQUENCY的单位是khz,所以1/I2C_MASTER_DEF_FREQUENCY *1000 得到的单位就是us,并且等于一个周期的时长，
#define   SCL_STRETCH_TIMEOUT          (8)                        // SCL最多延迟8个半周期，即4个周期

extern void i2c_master_init(void);
extern void i2c_master_reset(void);
extern void i2c_master_set_freq(uint16_t freq);

extern uint16_t i2c_master_get_freq(void);
extern uint8_t  i2c_master_write(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len);
extern uint8_t  i2c_master_write_byte(uint8_t baseAddr,uint16_t offsetAddr,uint8_t value);
extern uint8_t  i2c_master_read(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len);
extern uint8_t  i2c_master_read_byte(uint8_t baseAddr,uint16_t offsetAddr, uint8_t *value);

#endif

#endif

