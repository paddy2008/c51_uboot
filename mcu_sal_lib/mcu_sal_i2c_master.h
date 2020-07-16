#ifndef __MCU_SAL_I2C_MASTER_H__
#define __MCU_SAL_I2C_MASTER_H__

#include "mcu_sal_def.h"

#ifdef MCU_I2C_MASTER_MODULE_ENABLED

typedef struct
{
	uint8_t 		  half;					//������ڵ�ʱ��
	uint8_t  		  readBit	   : 1;		//�����ȡ��һ��bit
	uint8_t 		  widthFlag    : 1; 	//ƫ�Ƶ�ַ��ȱ�־λ, 0:8bit, 1:����8bitС�ڵ���16bit
	uint16_t 		  freq;					//i2c masterͨ�ŵ�ʱ��Ƶ��
}I2C_MASTER_IMITATE_STRUCT_T;


/****************************************************************************
����SDA�趨�󣬾���һ�������أ������趨һ����������ʱRISE_TIME_DELAY΢��
������BIT�����У�SDA������ǰһ��Rise_Time_Delay׼����
Ϊ�˲��ζԳƣ�ǰ��һ��BIT������Ϻ���Ҫ����ʱ����һ��Rise_Time_Delay
SCL�ĸߵ�ƽ����ʱ����պ���Ҫʱһ��������
���ں����ĵ��û�����������ִ��Ҳ������һЩʱ�䣬����ʵ�ʵ�SCL�͵�ƽʱ�佫����
�ߵ�ƽʱ��һЩ������ʱ��Ƶ�ʻ���С���趨ʱ��Ŀ�����ʣ�
I2C_Master_HoldTime��ָSTART STOP״̬ʱ��SCL SDA�仯ǰ�ı���ʱ��

��Ϊһ�����ڰ���һ���ߵ�ƽ����ʱ�䣬һ���͵�ƽ����ʱ�䣬һ�������ص�ʱ�䣬һ���½���ʱ��,���Լ򵥵���Ϊ�ߵ�ƽ����ʱ��͵�
��ƽ����ʱ����ȣ�������ʱ����½���ʱ����ȣ����ڰ�����ڵ�ʱ��Ϊ 500 / I2C_MASTER_DEF_FREQUENCY�����Ե�Ƶ��
��400khz��ʱ�������ʽ����Ľ��Ϊ1.25us,����ﲻ�������ʱ����ô�ǲ�����֧��400khz�ģ�Ŀǰʵ�ֵ����ֻ֧��200khz
ʹ��GPIO��ģ��i2c����Ҫ�ľ�����ʱ������׼ȷ�ԣ���Ϊ��ֱ�ӿ�����ʱ��ı仯��
*****************************************************************************/
#define   FUNC_CALL_ERROR_TIME		   (2)
#define   RISE_TIME_DELAY              (1)
#define   I2C_MASTER_HOLD_TIME         (int)((1 * 1000 / (I2C_MASTER_DEF_FREQUENCY)  - 2 * (RISE_TIME_DELAY)) / 2)  //�ߵ�ƽ��͵�ƽ�ĳ���ʱ��
#define   I2C_MASTER_DELAY             (500 / I2C_MASTER_DEF_FREQUENCY - RISE_TIME_DELAY)
#define   I2C_MASTER_HAFT_CYCLE        ((int)( 500 / I2C_MASTER_DEF_FREQUENCY))  //������ڵ�ʱ����һ�����ڵ���1/I2C_MASTER_DEF_FREQUENCY, ��ΪI2C_MASTER_DEF_FREQUENCY�ĵ�λ��khz,����1/I2C_MASTER_DEF_FREQUENCY *1000 �õ��ĵ�λ����us,���ҵ���һ�����ڵ�ʱ����
#define   SCL_STRETCH_TIMEOUT          (8)                        // SCL����ӳ�8�������ڣ���4������

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

