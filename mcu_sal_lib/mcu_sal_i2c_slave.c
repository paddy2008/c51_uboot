#include "mcu_sal.h"

#ifdef MCU_I2C_SLAVE_MODULE_ENABLED

static I2C_SLAVE_STRUCT_T _i2c_slave_struct;

static void _i2c_slave_read_data (void);

static void _i2c_slave_struct_init(void)
{
	memset((void *)&_i2c_slave_struct, 0 ,sizeof(I2C_SLAVE_STRUCT_T));
}

void  i2c_slave_init(void)
{
	lib_i2c_slave_init();
	_i2c_slave_struct_init();
}

void i2c_slave_process_devaddr_write(void)
{
	_i2c_slave_struct.devAddr   	  		 = RECV_DATA_REG;
	_i2c_slave_struct.num 				 	 = 0;
	_i2c_slave_struct.time_series_status     = I2C_NEXT_I2CREGADDRESS;
}

void i2c_slave_process_devaddr_read(void)
{
	_i2c_slave_struct.devAddr   			= RECV_DATA_REG;
	_i2c_slave_struct.num 					= 0;
	_i2c_slave_struct.time_series_status 	= I2C_NEXT_I2CREGADDRESS;

	if(_i2c_slave_struct.devAddr == I2C_SLAVE_A0_ADDR ||
	   _i2c_slave_struct.devAddr == I2C_SLAVE_A2_ADDR)
	{
		i2c_slave_process_data_read_ack();
	}
}

void i2c_slave_process_bus_error(void)
{
	_i2c_slave_struct.num 		   = 0;
	_i2c_slave_struct.bus_status   = I2C_SLAVE_BUS_IDLE;
}

void i2c_slave_process_data_write(void)
{
	uint8_t recv_data = 0;

	recv_data = RECV_DATA_REG;

	switch (_i2c_slave_struct.time_series_status)
	{
		case I2C_NEXT_I2CREGADDRESS:
		{
			_i2c_slave_struct.time_series_status = I2C_NEXT_DATA;

			if (I2C_SLAVE_A0_ADDR == _i2c_slave_struct.devAddr)
			{
				_i2c_slave_struct.A0_offsetAddr = recv_data;
			}
			else
			{
				_i2c_slave_struct.A2_offsetAddr = recv_data;
			}
			_i2c_slave_read_data();  //read data by this offsetaddr after receive the first offsetaddr regardless of read or write
			break;
		}
		case I2C_NEXT_DATA:
		{
			if (_i2c_slave_struct.num < I2C_WRITE_SIZE_ONCE)
			{
				_i2c_slave_struct.DataBuff[_i2c_slave_struct.num] = recv_data;
			}
			_i2c_slave_struct.num++;
			break;
		}
		default:
			break;
	}
}

void i2c_slave_process_data_read_ack(void)
{
	if (I2C_SLAVE_A0_ADDR == _i2c_slave_struct.devAddr)
	{
		SEND_DATA_REG = _i2c_slave_struct.A0_sendData;
		_i2c_slave_struct.A0_offsetAddr ++;
	}
	else
	{
		SEND_DATA_REG = _i2c_slave_struct.A2_sendData;
		_i2c_slave_struct.A2_offsetAddr ++;
	}

	_i2c_slave_read_data();
}

void i2c_slave_process_data_read_nack(void)
{
	_i2c_slave_struct.bus_status = I2C_SLAVE_BUS_IDLE;
}

void i2c_slave_process_stop(void)
{
	if(_i2c_slave_struct.num == 0)
	{
		_i2c_slave_struct.bus_status = I2C_SLAVE_BUS_IDLE;
	}
	else if(_i2c_slave_struct.num > I2C_WRITE_SIZE_ONCE)
	{
		// 如果接收的字节超过8个，直接抛弃掉，不处理
		_i2c_slave_struct.num 		 = 0;
		_i2c_slave_struct.bus_status = I2C_SLAVE_BUS_IDLE;
		memset((void *)&_i2c_slave_struct.DataBuff[0], 0, I2C_WRITE_SIZE_ONCE);
	}
	else
	{
		_i2c_slave_struct.bus_status = I2C_SLAVE_BUS_WAITING_TO_PROCESS;
		task_fast_handle(TASK_ID_MSA_RUN_TASK);
	}
}

static void _i2c_slave_read_data (void)
{
	uint8_t page = 0;

	if (I2C_SLAVE_A0_ADDR == _i2c_slave_struct.devAddr)
	{
		_i2c_slave_struct.A0_sendData = app_page_read_byte(APP_LOWER_PAGE, OFFSET_INDEX(_i2c_slave_struct.A0_offsetAddr));
	}
	else
	{
		if (_i2c_slave_struct.A2_offsetAddr <= 0x7F)
		{
			_i2c_slave_struct.A2_sendData = app_page_read_byte(APP_LOWER_PAGE, OFFSET_INDEX(_i2c_slave_struct.A2_offsetAddr)) + 1;
		}
		else
		{
			_i2c_slave_struct.A2_sendData = 0xFF;

			page = app_page_read_byte(APP_LOWER_PAGE, OFFSET_INDEX(MSA_LOWER_PAGE_SELECT_BYTE));

			if (page == 0x00)
			{
				uint8_t value = 0;
				flash_read_by_isr(MSA_FLASH_ADDR_PAGE_00H + OFFSET_INDEX(_i2c_slave_struct.A2_offsetAddr), &value, 1);
				_i2c_slave_struct.A2_sendData = value;
			}
			else if (page == 0x02)
			{
				_i2c_slave_struct.A2_sendData = app_page_read_byte(APP_PAGE_02H, OFFSET_INDEX(_i2c_slave_struct.A2_offsetAddr));

			}
			else if (page == 0x4)
			{
				_i2c_slave_struct.A2_sendData = app_page_read_byte(APP_PAGE_04H, OFFSET_INDEX(_i2c_slave_struct.A2_offsetAddr));
			}
		}
	}
}

void i2c_slave_write_data (void)
{
	uint16_t   i = 0;

	// 如果I2C缓存中不存在数据，退出处理函数
	if (_i2c_slave_struct.bus_status == I2C_SLAVE_BUS_IDLE)
	{
		return;
	}

    // 处理A0地址写入的数据
	if (I2C_SLAVE_A0_ADDR == _i2c_slave_struct.devAddr)
	{
		for (i = 0; i < _i2c_slave_struct.num; i++)
		{
			app_page_write_byte(APP_LOWER_PAGE, _i2c_slave_struct.A0_offsetAddr, _i2c_slave_struct.DataBuff[i]);
			_i2c_slave_struct.A0_offsetAddr ++;
		}
	}
	else if (I2C_SLAVE_A2_ADDR == _i2c_slave_struct.devAddr)
	{
		for (i = 0; i < _i2c_slave_struct.num; i++)
		{
			app_page_write_byte(APP_LOWER_PAGE, _i2c_slave_struct.A2_offsetAddr, _i2c_slave_struct.DataBuff[i]);
			_i2c_slave_struct.A2_offsetAddr ++;
		}
	}

	// 清空I2C缓存区,并将缓存区状态修正为IDLE状态
	_i2c_slave_struct.num = 0x00;
	_i2c_slave_struct.bus_status = I2C_SLAVE_BUS_IDLE;
	memset((void *)&_i2c_slave_struct.DataBuff[0], 0, I2C_WRITE_SIZE_ONCE);
}

#endif
