#include "mcu_sal.h"

#ifdef MCU_MDIO_MODULE_ENABLED

uint8_t delayVar  = 1;
uint8_t readBit = 1;

#define WRITE_BIT(value) \
{   		 	 \
	MDC  = 0; \
	MDIO = value; \
	delay_1us; \
	MDC  = 1; \
	DELAY_PIN = delayVar;\
	delay_1us; \
}

//MDC���ߵ�300ns���Ժ���ܶ�ȡMDIO
#define READ_BIT() \
{	\
	MDC = 0; \
	delayVar = DELAY_PIN;\
	delay_1us;\
	MDC = 1; \
	delay_300ns; \
	readBit = MDIO;	\
	delay_200ns; \
}

#define SET_TURN_AROUND() \
{ \
	MDC  = 0; \
	delay_1us; \
	MDC = 1; \
	delay_1us; \
}


#if 0
#define READ_BIT1() \
{	\
	MDC = 0; \
	delayVar = DELAY_PIN;\
	delay_4us;\
	MDC = 1; \
	delay_72_nop; \
	readBit = MDIO;	\
	delay_2us; \
}
#endif

/*
 *  ��ʼ��I2C����
 */
uint8_t mdio_init(void)
{
	MDC  = 0;
	MDIO = 0;

	lib_delay_us(MDIO_DELAY);

	MDC  = 1;
	MDIO = 1;

	lib_delay_us(MDIO_DELAY);            // �ȴ�ʱ�����������

	if (MDC && MDIO)
	{
		return RET_OK;                                // ��ʼ���ɹ�
	}

	lib_log("mdio init failed, MDC = %bu, MDIO = %bu", MDC ? 1: 0, MDIO ? 1: 0);
	return RET_FAILED;                                    // �ܽ�״̬���ԣ���ʼ���쳣
}


/*
MDIO����һ������,MDIO ���뱻����Ϊ���ģʽ.
value:Ҫ���͵�����
bits:���ݵ�λ��
*/
#if 0
static void _mdio_send_data(uint32_t value ,uint8_t bits)
{
   int16_t i = 0;

   for(i = bits - 1; i >= 0; i--)  //�ȴ��͸�bit
   {
    	WRITE_BIT((value >> i) & 1);
   }
}
#else

#define _mdio_send_data(value, bits) \
{  \
   int16_t i = 0; \
   for(i = bits - 1; i >= 0; i--)  \
   {	\
		WRITE_BIT((value >> i) & 1);\
   }	\
} \

#endif

/*
MDIO��ȡһ������,MDIO ���뱻����Ϊ����ģʽ.
bits:��ȡ���ݵ�λ��
*/
static uint32_t _mdio_recv_data(uint8_t bits)
{
	uint32_t value = 0;
	int16_t i = 0;

	for(i = bits - 1; i >= 0; i--)
	{
		value <<= 1;
		READ_BIT();
		value |= readBit;
	}

    return value;
}

static void _mdio_release_bus()
{
	READ_BIT();  //release MDIO bus
	MDC = 1;
	MDIO = 1;  //there must be setted 1 in order to release MDIO bus because the last bit may be 0 on MDIO bus line
}

static void _mdio_send_reg_addr(uint32_t phyAddr, uint32_t devAddr, uint16_t regAddr)
{
	int16_t i = 0;

	ENTER_CRITICAL();

	/*����32bit��1�����֡ǰ׺���Ǳ���ģ�ĳЩ�����оƬ��MDIO������û�������*/
	for(i = 0; i < 32; i++)
	{
		WRITE_BIT(1);
	}

	/*
	 * ���Ϳ�ʼλ(01),�Ͷ�������(10),д������(01)
	 * Clause 45 ��������ʼλ��(00),(11)Ϊ��,(01)Ϊд
	 */

#if MDIO_CLAUSE == MDIO_CLAUSE45
	 //���Ϳ�ʼ�ź�
	 WRITE_BIT(0);
	 for(i = 0; i < 1; i++);
	 WRITE_BIT(0);
	 for(i = 0; i < 1; i++);

	 //send write operation,write operation code is 01
	 WRITE_BIT(0);
	 for(i = 0; i < 1; i++);
	 WRITE_BIT(0);
	 for(i = 0; i < 1; i++);
#else
	 WRITE_BIT(0);
	 WRITE_BIT(1);

	 //send read operation,write operation code is 10
	 WRITE_BIT(0);
	 WRITE_BIT(1);
#endif

	_mdio_send_data(phyAddr, 5);
	_mdio_send_data(devAddr, 5);

	/* send the turnaround (10) */
	WRITE_BIT(1);
	for(i = 0; i < 1; i++);
	WRITE_BIT(0);
	for(i = 0; i < 1; i++);

	_mdio_send_data(regAddr, 16);

	EXIT_CRITICAL();
}

uint32_t mdio_read(uint32_t phyAddr, uint32_t devAddr, uint32_t regAddr)
{
	uint8_t i = 0;
	uint32_t value = 0;

	_mdio_send_reg_addr(phyAddr, devAddr, regAddr);

	//lib_mdio_output(TYPE_MDIO);

	ENTER_CRITICAL();
	/*����32bit��1�����֡ǰ׺���Ǳ���ģ�ĳЩ�����оƬ��MDIO������û�������*/
	for(i = 0; i < 32; i++)
	{
		WRITE_BIT(1);
	}

	/*
	 * ���Ϳ�ʼλ(01),�Ͷ�������(10),д������(01)
	 * Clause 45 ��������ʼλ��(00),(11)Ϊ��,(01)Ϊд
	 */
	 //���Ϳ�ʼ�ź�
#if MDIO_CLAUSE == MDIO_CLAUSE45
	 WRITE_BIT(0);
	 for(i = 0; i < 1; i++);
	 WRITE_BIT(0);
	 for(i = 0; i < 1; i++);

	 //send read operation,read operation code is 11
	 WRITE_BIT(1);
	 for(i = 0; i < 1; i++);
	 WRITE_BIT(1);
	 for(i = 0; i < 1; i++);
#else
	 WRITE_BIT(0);
	 WRITE_BIT(1);

	 //send read operation,read operation code is 10
	 WRITE_BIT(1);
	 WRITE_BIT(0);
#endif

	 _mdio_send_data(phyAddr, 5);
	 _mdio_send_data(devAddr, 5);

	 /* �е�PHYоƬ����Ҫ���bit
	  * WRITE_BIT(1);
	  * for(i = 0; i < 1; i++)
	  */

	 READ_BIT();

	/*check the turnaround bit: the PHY should be driving it to zero */
	if(readBit != 0)
	{
		/* PHY didn't driver TA low -- flush any bits it may be trying to send*/
		for(i = 0; i < 32; i++)
		{
			READ_BIT();
		}
		lib_log("PHY not ACK in MDIO \r\n");
		return 0x5555;
	}

	value = _mdio_recv_data(16);
	_mdio_release_bus();
	EXIT_CRITICAL();
    return value;
}

void mdio_write(uint32_t phyAddr, uint32_t devAddr, uint16_t regAddr, uint32_t value)
{
	uint8_t i = 0;

	_mdio_send_reg_addr(phyAddr, devAddr, regAddr);

	lib_mdio_output(TYPE_MDIO);

	/*����32bit��1�����֡ǰ׺���Ǳ���ģ�ĳЩ�����оƬ��MDIO������û�������*/
	for(i = 0; i < 32; i++)
	{
		WRITE_BIT(1);
	}

	/*
	 * ���Ϳ�ʼλ(01),�Ͷ�������(10),д������(01)
	 * Clause 45 ��������ʼλ��(00),(11)Ϊ��,(01)Ϊд
	 */

#if MDIO_CLAUSE == MDIO_CLAUSE45
	 //���Ϳ�ʼ�ź�
	 WRITE_BIT(0);
	 WRITE_BIT(0);

	 //send write operation,read operation code is 01
	 WRITE_BIT(0);
	 WRITE_BIT(1);
#else
	 WRITE_BIT(0);
	 WRITE_BIT(1);

	 //send read operation,read operation code is 10
	 WRITE_BIT(0);
	 WRITE_BIT(1);
#endif

	_mdio_send_data(phyAddr, 5);
	_mdio_send_data(devAddr, 5);

	/* send the turnaround (10) */
	WRITE_BIT(1);
	WRITE_BIT(0);

	_mdio_send_data(value, 16);
	_mdio_release_bus();
	lib_mdio_input(TYPE_MDIO);
}

#endif
