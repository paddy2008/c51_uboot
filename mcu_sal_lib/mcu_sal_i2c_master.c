/*
 * i2c master模拟时序的函数中不要添加log信息
 * 因为它会影响i2c master的时序
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
 * 发送I2C Start波形
 */
static uint8_t _i2c_master_imitate_start(void)
{
	// 根据SCL SDA的不同初始状态，将SCL SDA的引脚拉高为1，准备好，并避免出现STOP的状态
	if (SCL == 1)
	{
		if (SDA == 0)
		{
			SCL = 0;
			lib_delay_us(RISE_TIME_DELAY);
			SDA = 1;   //在改变SDA的值的时候，必须要先将SCL拉低，否则会认为这个数据时有效的
			lib_delay_us(RISE_TIME_DELAY);
			SCL = 1;
		}
	}
	else
	{
		if (SDA == 1)                  // 时钟线为低，数据线为高
		{
			SCL = 1;
		}
		else                           // 时钟线为低，数据线为低
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
		SCL = 0;                                         // 拉低SCL时钟线
		lib_delay_us(I2C_MASTER_DELAY);               // 为了保持SCL为低电平的时间约为半周期
		return RET_OK;
	}

	return RET_FAILED;
}

/*
 * 发送I2C Stop波形
 */
static uint8_t _i2c_master_imitate_stop(void)
{
	// 根据SCL SDA的不同初始状态，将SCL=1 SDA=0，同时避免出现STOP的状态
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

	lib_delay_us(RISE_TIME_DELAY);        // 等待SCL SDA稳定

	if ((SCL == 1) && (SDA == 0))
	{
		lib_delay_us(I2C_MASTER_HOLD_TIME);
		SDA = 1;
		lib_delay_us(I2C_MASTER_HOLD_TIME);

		if ((SCL == 1) && (SDA == 1))
	    {
	    	return RET_OK; // SCL SDA均为高 正常
	    }
	    else
	    {
	       return RET_FAILED; // SCL SDA无法达到终止状态，报告异常
	    }
	}

	return RET_FAILED; // SCL SDA无法设定为初始状态，报告异常
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

	   lib_delay_us(RISE_TIME_DELAY);  //发送时钟，让i2c slave释放

	   SCL = 0;

	   lib_delay_us(RISE_TIME_DELAY);
	}

	_i2c_master_imitate_stop();  //发送stop signal, 结束这次通信
}

/*
 *  初始化I2C总线
 */
static uint8_t _i2c_master_imitate_init(void)
{
	_i2c_master_imitate_reset();

	SCL = 0;
	SDA = 0;

	lib_delay_us(RISE_TIME_DELAY);

	SCL = 1;
	SDA = 1;

	lib_delay_us(RISE_TIME_DELAY);            // 等待时钟上升沿完成

	if ((SCL == 1) && (SDA == 1))
	{
		return RET_OK;                                // 初始化成功
	}

	lib_log("_i2c_master_imitate_init failed");
	return RET_FAILED;                                    // 管脚状态不对，初始化异常
}


/*
 * 发送一个bit数据到总线上
 */
# if 0
static uint8_t _i2c_master_imitate_write_bit(uint8_t value)
{
	uint8_t timercount =  SCL_STRETCH_TIMEOUT;              // 设置超时等待时间，时钟周期：8
	uint16_t cnt = 0;

	SDA = value;                          		 // 输出一个bit数据到SDA数据线

	lib_delay_us(RISE_TIME_DELAY);              // 要求等待时间大于2us

	if (SDA != value)                              // 检查SDA是否与目标值一致，不一致则报错
	{
		return RET_FAILED;
	}

	SCL = 1;                                      // 拉高时钟线

	lib_delay_us(RISE_TIME_DELAY);              // 要求等待时间大于2us

	while(!SCL)                                    // 检查SCL状态是否为高，不为高则等待，不执行while，时钟周期：3
	{
		lib_delay_us(I2C_MASTER_HAFT_CYCLE);

		if (timercount-- == 0)                    // 超过8个I2C总线周期SCL没有恢复，则报错
		{
	         return RET_FAILED;
		}
	}

	lib_delay_us(I2C_MASTER_DELAY);              // 延时，控制传送速率

	SCL = 0;                                        // 拉低SCL时钟线

	lib_delay_us((I2C_MASTER_DELAY > FUNC_CALL_ERROR_TIME) ? (I2C_MASTER_DELAY - FUNC_CALL_ERROR_TIME) : 0);     // 延时，控制传送速率

	return RET_OK;                                  // 函数执行完
}
#endif

/*
 * 发送一个bit数据到总线上的宏定义
 * 发送bit数据最好用宏定义，因为函数调用会涉及到参数的压栈入栈，以及函数返回值，
 * 这些指令的累计耗时估计在1个us左右，毕竟这里的时序都是精确到us级别，偏差一个
 * us，实际的频率都有较大变化
 * 注意这句代码while(!SCL);
 * 这里使用延时函数来延时并不准确，因为假设这里延时1us，但是实际上硬件达到高电平这个
 * 时间是有硬件决定的，有的是500ns,有的是1000ns,如果实际需要800ns拉高，但是
 * 我们延时1000ns，那么多出的200ns就变成了高电平的保持时间了，从而导致周期不准，
 * 另外如果这里SCL被拉高了，那么SDA肯定赋值完成了，因为它在SCL拉高之前
 * lib_delay_us(I2C_MASTER_HAFT_CYCLE - 1)这里减一的目的是这个宏调用完成后for循环中指令的耗时时间，这个的到底减多少时间误差可以通过测量得到
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
 * 接收一个bit数据
 */
#if 0
static uint8_t _i2c_master_imitate_read_bit(uint8_t *value)
{
	uint8_t timercount = SCL_STRETCH_TIMEOUT;   // 设置超时等待时间，时钟周期：8

	SDA = 1;                      		// 将SDA设定为高阻模式

	lib_delay_us(RISE_TIME_DELAY); 	// 等待SDA线路稳定

	SCL = 1;                      		// 拉高时钟线

	lib_delay_us(RISE_TIME_DELAY);   // 等待SCL线路稳定

	while(!SCL)                 		// 检查SCL状态是否为高，不为高则等待，不执行while，时钟周期：3
	{
		lib_delay_us(I2C_MASTER_HAFT_CYCLE);

		if (timercount-- == 0)       	// 超过8个I2C总线周期SCL没有恢复，则报错
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
 * 接收一个bit数据的宏定义
 * 接收一个bit数据最好用宏定义，因为函数调用会涉及到参数的压栈入栈，以及函数返回值，
 * 这些指令的累计耗时估计在1个us左右，毕竟这里的时序都是精确到us级别，偏差一个
 * us，实际的频率都有较大变化
 * 注意这句代码while(!SCL);
 * 这里使用延时函数来延时并不准确，因为假设这里延时1us，但是实际上硬件达到高电平这个
 * 时间是有硬件决定的，有的是500ns,有的是1000ns,如果实际需要800ns拉高，但是
 * 我们延时1000ns，那么多出的200ns就变成了高电平的保持时间了，从而导致周期不准，
 * 另外如果这里SCL被拉高了，那么SDA肯定赋值完成了，因为它在SCL拉高之前
 * lib_delay_us(I2C_MASTER_HAFT_CYCLE - 1)这里减一的目的是如下两句代码while(!SCL);_i2c_master_imitate_struct.readBit = SDA;的耗时时间
 * lib_delay_us(I2C_MASTER_HAFT_CYCLE - 2)这里减一的目的是这个宏调用完成后for循环中指令的耗时时间，这个的到底减多少时间误差可以通过测量得到
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
 * 发送一个byte数据,仅仅是数据
 */
uint8_t i2c_master_imitate_send_byte(uint8_t value)
{
	uint8_t i = 0;

	for(i = 0; i < 8; i++)
	{
		WRITE_BIT(MY_GET_BIT(value, 7 - i));  //先传送高bit
	}

	READ_BIT();

	if(_i2c_master_imitate_struct.readBit == 1)  //NACK
	{
		return RET_FAILED;
	}
	return RET_OK;
}

/*
 * 接收一个byte数据，返回应答
 */
uint8_t i2c_master_imitate_recv_byte (uint8_t ack_out, uint8_t *value)
{
	uint8_t i = 0;

	*value = 0;

	for(i = 0; i < 8; i++)
	{
		READ_BIT();
		*value |= (_i2c_master_imitate_struct.readBit << (7 - i)); //先接收高bit
	}

	WRITE_BIT(ack_out);

	return RET_OK;
}


static uint8_t _i2c_master_send_offset_addr(uint8_t baseAddr, uint16_t offsetAddr)
{
	_i2c_master_update_width_flag(baseAddr);

	// 写入数据写入的子地址
	if( _i2c_master_imitate_struct.widthFlag)  //indicate offsetAddr width is 8bit
	{
		if (RET_OK != i2c_master_imitate_send_byte(U16_GET_MSB(offsetAddr)))  //先发送高字节
		{
			_i2c_master_imitate_struct.widthFlag = false;
		   _i2c_master_imitate_stop();
		   return RET_FAILED;
		}

		if (RET_OK != i2c_master_imitate_send_byte(U16_GET_LSB(offsetAddr))) //再发送高字节
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
 * 单地址单字节读，向从机芯片发送一个字节地址，读取一个字节数据
 */
uint8_t i2c_master_imitate_read_byte (uint8_t baseAddr,uint16_t offsetAddr, uint8_t *buff)
{

	// 发送START信号
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 对设备地址进行SLA+W寻址
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//发送偏移地址
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// 发送RESTART信号
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 对设备地址进行SLA+R寻址
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr | 0x01)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 读取一个字节的数据，并返回NACK
	if (RET_OK != i2c_master_imitate_recv_byte(1, buff))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 发送STOP信号
	if (RET_OK != _i2c_master_imitate_stop() )
	{
		return RET_FAILED;
	}

	return RET_OK;
}

/*
 * 多字节读，向从机芯片发送一个字节地址，读取若干个字节数据
 */
uint8_t i2c_master_imitate_read(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len)
{
	uint8_t i = 0;

	// 发送START信号
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 对设备地址进行SLA+W寻址
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//发送偏移地址
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// 发送RESTART信号
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 对设备地址进行SLA+R寻址
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr | 0x01)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 读取一个字节的数据，并返回ACK
	for(i = 0; i < len - 1; i ++)
	{
		if (RET_OK != i2c_master_imitate_recv_byte(0, &buff[i]))
		{
			_i2c_master_imitate_stop();
			return RET_FAILED;
		}
	}

	//读最后一个字节的数据，返回NACK
	if (RET_OK != i2c_master_imitate_recv_byte(1, &buff[len - 1]))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 发送STOP信号
	if (RET_OK != _i2c_master_imitate_stop() )
	{
		return RET_FAILED;
	}

	return RET_OK;
}

/*
 * 单地址单字节写，向从机芯片发送一个字节地址，写入一个字节数据
 */
uint8_t i2c_master_imitate_write_byte (uint8_t baseAddr,uint16_t offsetAddr, uint8_t value)
{
	_i2c_master_update_width_flag(baseAddr);

	// 发送START信号
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 对设备地址进行SLA+W寻址
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//发送偏移地址
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// 写入数据
	if (RET_OK != i2c_master_imitate_send_byte(value))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 发送STOP信号
	if (RET_OK != _i2c_master_imitate_stop())
	{
		return RET_FAILED;
	}

	return RET_OK;
}

uint8_t i2c_master_imitate_write(uint8_t baseAddr, uint16_t offsetAddr, uint8_t *buff, uint8_t len)
{
	uint8_t i = 0;
	// 发送START信号
	if (RET_OK != _i2c_master_imitate_start())
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	// 对设备地址进行SLA+W寻址
	if (RET_OK != i2c_master_imitate_send_byte((baseAddr & 0xFE)))
	{
		_i2c_master_imitate_stop();
		return RET_FAILED;
	}

	//发送偏移地址
	_i2c_master_send_offset_addr(baseAddr, offsetAddr);

	// 写入数据
	for (i = 0; i < len; i ++)
	{
		if (RET_OK != i2c_master_imitate_send_byte(buff[i]))
		{
			_i2c_master_imitate_stop();
			return RET_FAILED;
		}
	}

	// 发送STOP信号
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

