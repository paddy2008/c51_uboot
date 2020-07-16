/*
 * i2c masterģ��ʱ��ĺ����в�Ҫ���log��Ϣ
 * ��Ϊ����Ӱ��i2c master��ʱ��
 */
#include "mcu_sal.h"

#ifdef MCU_I2C_MASTER_MODULE_ENABLED

#if I2C_MASTER_MODE == 0x0

static I2C_MASTER_IMITATE_STRUCT_T _i2c_master_imitate_struct;

static void _i2c_master_imitate_struct_init(void)
{
	_i2c_master_imitate_struct.readBit   = 0;
	_i2c_master_imitate_struct.widthFlag = false;
	_i2c_master_imitate_struct.freq 	 = I2C_MASTER_DEF_FREQUENCY;
	_i2c_master_imitate_struct.half 	 = I2C_MASTER_HAFT_CYCLE;
}

static void _i2c_master_update_width_flag(uint8_t baseAddr)
{
	if(baseAddr == I2C_SLAVE_DEV1_ADDR || baseAddr == I2C_SLAVE_DEV2_ADDR || baseAddr == I2C_SLAVE_DEV3_ADDR) //12bit offsetaddr
	{
		 _i2c_master_imitate_struct.widthFlag = true;
	}
	else
	{
		 _i2c_master_imitate_struct.widthFlag = false;
	}
}

/*
 * ����I2C Start����
 */
static uint8_t _i2c_master_imitate_start(void)
{
	// ����SCL SDA�Ĳ�ͬ��ʼ״̬����SCL SDA����������Ϊ1��׼���ã����������STOP��״̬
	if (SCL == 1)
	{
		if (SDA == 0)
		{
			SCL = 0;
			lib_delay_us(RISE_TIME_DELAY);
			SDA = 1;   //�ڸı�SDA��ֵ��ʱ�򣬱���Ҫ�Ƚ�SCL���ͣ��������Ϊ�������ʱ��Ч��
			lib_delay_us(RISE_TIME_DELAY);
			SCL = 1;
		}
	}
	else
	{
		if (SDA == 1)                  // ʱ����Ϊ�ͣ�������Ϊ��
		{
			SCL = 1;
		}
		else                           // ʱ����Ϊ�ͣ�������Ϊ��
		{
			SDA = 1;
			lib_delay_us(RISE_TIME_DELAY);
			SCL = 1;
		}
	}

	lib_delay_us(RISE_TIME_DELAY);

	if ((SCL == 1) && (SDA == 1))
	{
		lib_delay_us(I2C_MASTER_HOLD_TIME);
		SDA = 0;
		lib_delay_us(I2C_MASTER_HOLD_TIME);
		SCL = 0;                                         // ����SCLʱ����
		lib_delay_us(I2C_MASTER_DELAY);               // Ϊ�˱���SCLΪ�͵�ƽ��ʱ��ԼΪ������
		return RET_OK;
	}

	return RET_FAILED;
}

/*
 * ����I2C Stop����
 */
static uint8_t _i2c_master_imitate_stop(void)
{
	// ����SCL SDA�Ĳ�ͬ��ʼ״̬����SCL=1 SDA=0��ͬʱ�������STOP��״̬
	if (SCL == 0)
	{
		if (SDA == 0)                  // SCL=0 SDA=0
		{
			SCL = 1;
		}
		else                                      // SCL=0 SDA=1
		{
			SDA = 0;
			lib_delay_us(RISE_TIME_DELAY);
			SCL = 1;
		}
	}
	else
	{
		if (SDA == 1)                 // SCL=1 SDA=1
		{
			SCL = 0;
			lib_delay_us(RISE_TIME_DELAY);
			SDA = 0;
			lib_delay_us(RISE_TIME_DELAY);
			SCL = 1;
		}
	}

	lib_delay_us(RISE_TIME_DELAY);        // �ȴ�SCL SDA�ȶ�

	if ((SCL == 1) && (SDA == 0))
	{
		lib_delay_us(I2C_MASTER_HOLD_TIME);
		SDA = 1;
		lib_delay_us(I2C_MASTER_HOLD_TIME);

		if ((SCL == 1) && (SDA == 1))
	    {
	    	return RET_OK; // SCL SDA��Ϊ�� ����
	    }
	    else
	    {
	       return RET_FAILED; // SCL SDA�޷��ﵽ��ֹ״̬�������쳣
	    }
	}

	return RET_FAILED; // SCL SDA�޷��趨Ϊ��ʼ״̬�������쳣
}


/*
 * i2c reset
 *
 */
static void _i2c_master_imitate_reset(void)
{
	while(!SDA)  //if SDA is pull low by i2c slave
	{
	   SCL = 1;

	   lib_delay_us(RISE_TIME_DELAY);  //����ʱ�ӣ���i2c slave�ͷ�

	   SCL = 0;

	   lib_delay_us(RISE_TIME_DELAY);
	}

	_i2c_master_imitate_stop();  //����stop signal, �������ͨ��
}

/*
 *  ��ʼ��I2C����
 */
static uint8_t _i2c_master_imitate_init(void)
{
	_i2c_master_imitate_reset();

	SCL = 0;
	SDA = 0;

	lib_delay_us(RISE_TIME_DELAY);

	SCL = 1;
	SDA = 1;

	lib_delay_us(RISE_TIME_DELAY);            // �ȴ�ʱ�����������

	if ((SCL == 1) && (SDA == 1))
	{
		return RET_OK;                                // ��ʼ���ɹ�
	}

	lib_log("_i2c_master_imitate_init failed");
	return RET_FAILED;                                    // �ܽ�״̬���ԣ���ʼ���쳣
}


/*
 * ����һ��bit���ݵ�������
 */
# if 0
static uint8_t _i2c_master_imitate_write_bit(uint8_t value)
{
	uint8_t timercount =  SCL_STRETCH_TIMEOUT;              // ���ó�ʱ�ȴ�ʱ�䣬ʱ�����ڣ�8
	uint16_t cnt = 0;

	SDA = value;                          		 // ���һ��bit���ݵ�SDA������

	lib_delay_us(RISE_TIME_DELAY);              // Ҫ��ȴ�ʱ�����2us

	if (SDA != value)                              // ���SDA�Ƿ���Ŀ��ֵһ�£���һ���򱨴�
	{
		return RET_FAILED;
	}

	SCL = 1;                                      // ����ʱ����

	lib_delay_us(RISE_TIME_DELAY);              // Ҫ��ȴ�ʱ�����2us

	while(!SCL)                                    // ���SCL״̬�Ƿ�Ϊ�ߣ���Ϊ����ȴ�����ִ��while��ʱ�����ڣ�3
	{
		lib_delay_us(I2C_MASTER_HAFT_CYCLE);

		if (timercount-- == 0)                    // ����8��I2C��������SCLû�лָ����򱨴�
		{
	         return RET_FAILED;
		}
	}

	lib_delay_us(I2C_MASTER_DELAY);              // ��ʱ�����ƴ�������

	SCL = 0;                                        // ����SCLʱ����

	lib_delay_us((I2C_MASTER_DELAY > FUNC_CALL_ERROR_TIME) ? (I2C_MASTER_DELAY - FUNC_CALL_ERROR_TIME) : 0);     // ��ʱ�����ƴ�������

	return RET_OK;                                  // ����ִ����
}
#endif

/*
 * ����һ��bit���ݵ������ϵĺ궨��
 * ����bit��������ú궨�壬��Ϊ�������û��漰��������ѹջ��ջ���Լ���������ֵ��
 * ��Щָ����ۼƺ�ʱ������1��us���ң��Ͼ������ʱ���Ǿ�ȷ��us����ƫ��һ��
 * us��ʵ�ʵ�Ƶ�ʶ��нϴ�仯
 * ע��������while(!SCL);
 * ����ʹ����ʱ��������ʱ����׼ȷ����Ϊ����������ʱ1us������ʵ����Ӳ���ﵽ�ߵ�ƽ���
 * ʱ������Ӳ�������ģ��е���500ns,�е���1000ns,���ʵ����Ҫ800ns���ߣ�����
 * ������ʱ1000ns����ô�����200ns�ͱ���˸ߵ�ƽ�ı���ʱ���ˣ��Ӷ��������ڲ�׼��
 * �����������SCL�������ˣ���ôSDA�϶���ֵ����ˣ���Ϊ����SCL����֮ǰ
 * lib_delay_us(I2C_MASTER_HAFT_CYCLE - 1)�����һ��Ŀ��������������ɺ�forѭ����ָ��ĺ�ʱʱ�䣬����ĵ��׼�����ʱ��������ͨ�������õ�
 */
#define WRITE_BIT(value) \
{   		 	 \
	SDA = value; \
	SCL = 1; 	 \
	while(!SCL); \
	lib_delay_us(_i2c_master_imitate_struct.half); \
	SCL = 0;  	\
	while(SCL);	\
	lib_delay_us((_i2c_master_imitate_struct.half > 1) ? (_i2c_master_imitate_struct.half - 1) : 0); \
}

/*
 * ����һ��bit����
 */
#if 0
static uint8_t _i2c_master_imitate_read_bit(uint8_t *value)
{
	uint8_t timercount = SCL_STRETCH_TIMEOUT;   // ���ó�ʱ�ȴ�ʱ�䣬ʱ�����ڣ�8

	SDA = 1;                      		// ��SDA�趨Ϊ����ģʽ

	lib_delay_us(RISE_TIME_DELAY); 	// �ȴ�SDA��·�ȶ�

	SCL = 1;                      		// ����ʱ����

	lib_delay_us(RISE_TIME_DELAY);   // �ȴ�SCL��·�ȶ�

	while(!SCL)                 		// ���SCL״̬�Ƿ�Ϊ�ߣ���Ϊ����ȴ�����ִ��while��ʱ�����ڣ�3
	{
		lib_delay_us(I2C_MASTER_HAFT_CYCLE);

		if (timercount-- == 0)       	// ����8��I2C��������SCLû�лָ����򱨴�
		{
			return RET_FAILED;
		}
	}

	*value = SDA;

	lib_delay_us(I2C_MASTER_DELAY);

	SCL = 0;

	lib_delay_us((I2C_MASTER_DELAY > FUNC_CALL_ERROR_TIME) ? (I2C_MASTER_DELAY - FUNC_CALL_ERROR_TIME) : 0);

	return RET_OK;
}
#endif


/*
 * ����һ��bit���ݵĺ궨��
 * ����һ��bit��������ú궨�壬��Ϊ�������û��漰��������ѹջ��ջ���Լ���������ֵ��
 * ��Щָ����ۼƺ�ʱ������1��us���ң��Ͼ������ʱ���Ǿ�ȷ��us����ƫ��һ��
 * us��ʵ�ʵ�Ƶ�ʶ��нϴ�仯
 * ע��������while(!SCL);
 * ����ʹ����ʱ��������ʱ����׼ȷ����Ϊ����������ʱ1us������ʵ����Ӳ���ﵽ�ߵ�ƽ���
 * ʱ������Ӳ�������ģ��е���500ns,�е���1000ns,���ʵ����Ҫ800ns���ߣ�����
 * ������ʱ1000ns����ô�����200ns�ͱ���˸ߵ�ƽ�ı���ʱ���ˣ��Ӷ��������ڲ�׼��
 * �����������SCL�������ˣ���ôSDA�϶���ֵ����ˣ���Ϊ����SCL����֮ǰ
 * lib_delay_us(I2C_MASTER_HAFT_CYCLE - 1)�����һ��Ŀ���������������while(!SCL);_i2c_master_imitate_struct.readBit = SDA;�ĺ�ʱʱ��
 * lib_delay_us(I2C_MASTER_HAFT_CYCLE - 2)�����һ��Ŀ��������������ɺ�forѭ����ָ��ĺ�ʱʱ�䣬����ĵ��׼�����ʱ��������ͨ�������õ�
 */

#define READ_BIT() \
{	\
	SDA = 1;	\
	SCL = 1;	\
	while(!SCL);\
	_i2c_master_imitate_struct.readBit = SDA;	\
	lib_delay_us((_i2c_master_imitate_struct.half > 1) ? (_i2c_master_imitate_struct.half - 1) : 0);\
	SCL = 0;	\
	while(SCL);	\
	lib_delay_us((_i2c_master_imitate_struct.half > 2) ? (_i2c_master_imitate_struct.half - 2) : 0);\
}

/*
 * ����һ��byte����,����������
 */
uint8_t i2c_master_imitate_send_byte(uint8_t value)
{
	uint8_t i = 0;

	for(i = 0; i < 8; i++)
	{
		WRITE_BIT(MY_GET_BIT(value, 7 - i));  //�ȴ��͸�bit
	}

	READ_BIT();

	if(_i2c_master_imitate_struct.readBit == 1)  //NACK
	{
		return RET_FAILED;
	}
	return RET_OK;
}

/*
 * ����һ��byte���ݣ�����Ӧ��
 */
uint8_t i2c_master_imitate_recv_byte (uint8_t ack_out, uint8_t *value)
{
	uint8_t i = 0;

	*value = 0;

	for(i = 0; i < 8; i++)
	{
		READ_BIT();
		*value |= (_i2c_master_imitate_struct.readBit << (7 - i)); //�Ƚ��ո�bit
	}

	WRITE_BIT(ack_out);

	return RET_OK;
}


static uint8_t _i2c_master_send_offset_addr(uint8_t baseAddr, uint16_t offsetAddr)
{
	_i2c_master_update_width_flag(baseAddr);

	// д������д����ӵ�ַ
	if( _i2c_master_imitate_struct.widthFlag)  //indicate offsetAddr width is 8bit
	{
		if (RET_OK != i2c_master_imitate_send_byte(U16_GET_MSB(offsetAddr)))  //�ȷ��͸��ֽ�
		{
			_i2c_master_imitate_struct.widthFlag = false;
		   _i2c_master_imitate_stop();
		   return RET_FAILED;
		}

		if (RET_OK != i2c_master_imitate_send_byte(U16_GET_LSB(offsetAddr))) //�ٷ��͸��ֽ�
		{
			_i2c_master_imitate_struct.widthFlag = false;
		   _i2c_master_imitate_stop();
		   return RET_FAILED;
		}
	}
	else  //indicate offsetAddr width > 8bit
	{
		if (RET_OK != i2c_master_imitate_send_byte(U16_GET_LSB(offsetAddr)))
		{
			_i2c_master_imitate_struct.widthFlag = false;
		   _i2c_master_imitate_stop();
		   return RET_FAILED;
		}
	}

	_i2c_master_imitate_struct.widthFlag = false;

	return  RET_OK;
}

/*
 * ����ַ���ֽڶ�����ӻ�оƬ����һ���ֽڵ�ַ����ȡһ���ֽ�����
 */
uint8_t i2c_master_imitate_read_byte (uint8_t baseAddr,uint16_t offsetAddr, uint8_t *buff)
{

	// ����START�ź�
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ���豸��ַ����SLA+WѰַ
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//����ƫ�Ƶ�ַ
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// ����RESTART�ź�
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ���豸��ַ����SLA+RѰַ
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr | 0x01)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ��ȡһ���ֽڵ����ݣ�������NACK
	if (RET_OK != i2c_master_imitate_recv_byte(1, buff))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ����STOP�ź�
	if (RET_OK != _i2c_master_imitate_stop() )
	{
		return RET_FAILED;
	}

	return RET_OK;
}

/*
 * ���ֽڶ�����ӻ�оƬ����һ���ֽڵ�ַ����ȡ���ɸ��ֽ�����
 */
uint8_t i2c_master_imitate_read(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len)
{
	uint8_t i = 0;

	// ����START�ź�
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ���豸��ַ����SLA+WѰַ
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//����ƫ�Ƶ�ַ
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// ����RESTART�ź�
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ���豸��ַ����SLA+RѰַ
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr | 0x01)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ��ȡһ���ֽڵ����ݣ�������ACK
	for(i = 0; i < len - 1; i ++)
	{
		if (RET_OK != i2c_master_imitate_recv_byte(0, &buff[i]))
		{
			_i2c_master_imitate_stop();
			return RET_FAILED;
		}
	}

	//�����һ���ֽڵ����ݣ�����NACK
	if (RET_OK != i2c_master_imitate_recv_byte(1, &buff[len - 1]))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ����STOP�ź�
	if (RET_OK != _i2c_master_imitate_stop() )
	{
		return RET_FAILED;
	}

	return RET_OK;
}

/*
 * ����ַ���ֽ�д����ӻ�оƬ����һ���ֽڵ�ַ��д��һ���ֽ�����
 */
uint8_t i2c_master_imitate_write_byte (uint8_t baseAddr,uint16_t offsetAddr, uint8_t value)
{
	_i2c_master_update_width_flag(baseAddr);

	// ����START�ź�
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ���豸��ַ����SLA+WѰַ
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//����ƫ�Ƶ�ַ
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// д������
	if (RET_OK != i2c_master_imitate_send_byte(value))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ����STOP�ź�
	if (RET_OK != _i2c_master_imitate_stop())
	{
		return RET_FAILED;
	}

	return RET_OK;
}

uint8_t i2c_master_imitate_write(uint8_t baseAddr, uint16_t offsetAddr, uint8_t *buff, uint8_t len)
{
	uint8_t i = 0;
	// ����START�ź�
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// ���豸��ַ����SLA+WѰַ
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//����ƫ�Ƶ�ַ
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// д������
	for (i = 0; i < len; i ++)
	{
		if (RET_OK != i2c_master_imitate_send_byte(buff[i]))
		{
			_i2c_master_imitate_stop();
			return RET_FAILED;
		}
	}

	// ����STOP�ź�
	if (RET_OK != _i2c_master_imitate_stop())
	{
		return RET_FAILED;
	}

	return RET_OK;
}
#endif

void i2c_master_init(void)
{
#if I2C_MASTER_MODE
	lib_i2c_master_init();
#else
	_i2c_master_imitate_struct_init();
	_i2c_master_imitate_init();
#endif
}

uint8_t i2c_master_read(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len)
{
#if I2C_MASTER_MODE
	return lib_i2c_master_read(baseAddr, offsetAddr, buff, len);
#else
	return i2c_master_imitate_read(baseAddr, offsetAddr, buff, len);
#endif
}

uint8_t i2c_master_read_byte(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff)
{
#if I2C_MASTER_MODE
	return lib_i2c_master_read(baseAddr, offsetAddr, buff, 1);
#else
	return i2c_master_imitate_read(baseAddr, offsetAddr, buff, 1);
#endif
}

uint8_t i2c_master_write(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len)
{
#if I2C_MASTER_MODE
	return lib_i2c_master_write(baseAddr, offsetAddr, buff, len);
#else
	return i2c_master_imitate_write(baseAddr, offsetAddr, buff, len);
#endif
}

uint8_t i2c_master_write_byte(uint8_t baseAddr,uint16_t offsetAddr, uint8_t value)
{
#if I2C_MASTER_MODE
	return lib_i2c_master_write(baseAddr, offsetAddr, &value, 1);
#else
	return i2c_master_imitate_write(baseAddr, offsetAddr, &value, 1);
#endif
}

uint16_t i2c_master_get_freq(void)
{
#if I2C_MASTER_MODE
	return lib_i2c_master_get_freq();
#else
	return (uint16_t)_i2c_master_imitate_struct.freq;
#endif
}

void i2c_master_set_freq(uint16_t freq)
{
#if I2C_MASTER_MODE
	lib_i2c_master_set_freq(freq);
#else
	_i2c_master_imitate_struct.freq = freq;
#endif
}

void i2c_master_reset(void)
{
#if I2C_MASTER_MODE
	lib_i2c_master_reset();
#else
	_i2c_master_imitate_reset();
#endif
}

#endif

