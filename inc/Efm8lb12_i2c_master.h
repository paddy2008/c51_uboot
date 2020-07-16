#ifndef __Efm8lb12_i2c_master_h__
#define __Efm8lb12_i2c_master_h__

#include <SI_EFM8LB1_Register_Enums.h>

typedef struct
{
	uint8_t 		  ackPollFlag  : 1;   	//��û���յ�ACK��ʱ���Զ��ط�start�źŵı�־λ
	uint8_t 		  rwFlag       : 1; 	//��д��־λ
	uint8_t 		  busyFlag     : 1;		//����busy��־λ
	uint8_t 		  offAddrFlag  : 1;		//ƫ�Ƶ�ַ���ͽ�����־λ
	uint8_t 		  startFlag    : 1;		//start�źŷ��ͽ�����־λ
	uint8_t 		  rwChangeFlag : 1;		//i2c����ı��־λ��������ʱ�򣬱�����д��ַ��Ȼ��Ÿ���д��ĵ�ַ���ж�������i2c���Ĺ������з���ı�
	uint8_t  		  baseAddr;				//i2c���豸��ַ
	uint8_t  		  len;					//i2c��д�ֽڵĳ��ȣ����256bytes
	uint8_t 		  cnt;					//ƫ�Ƶ�ַ�ķ����ֽڵĸ���,��Ϊ�и��豸��ַ��8bit���е���12bit������16bit����ƫ�Ƶ�ַ����8ibtʱ���ȷ��͸��ֽڣ��ٷ��͵��ֽ�
	uint8_t  		  *buff;				//��дbuffָ��
	uint16_t 		  offAddr;				//����ƫ�Ƶ�ַ
	uint8_t 		  widthFlag    : 1; 	//ƫ�Ƶ�ַ��ȱ�־λ, 0:8bit, 1:����8bitС�ڵ���16bit
	uint16_t 		  freq;					//i2c masterͨ�ŵ�ʱ��Ƶ��
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

