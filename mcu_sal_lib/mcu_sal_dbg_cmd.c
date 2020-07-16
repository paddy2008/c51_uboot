#include "mcu_sal.h"

#ifdef MCU_DBG_CMD_MODULE_ENABLED

/*
 * 该函数应用于i2c主设备端,用于i2c主设备向i2c从设备请求命令后获取请求命令的状态信息，到底是执行成功，还是失败
 */
static bool dbg_cmd_get_status()
{
	uint8_t  result = 0;
	uint16_t counter = 0;

	do
	{
		i2c_master_read_byte(I2C_DEVICE_ADDR, DEBUG_CMD_START_ADDR, &result);
		counter ++;
		timer_delay_ms(2 * counter);
		if(counter >= 100)  //delay 10s
		{
			lib_log("\r\n dbg_cmd_get_status failed");
			return false;
		}

	}while(result != RET_OK && result != RET_FAILED);

	i2c_master_write_byte(I2C_DEVICE_ADDR, DEBUG_CMD_START_ADDR, CMD_TYPE_IDEL);  //set dbg_cmd_status idle

	if(result == RET_FAILED)
	{
		return false;
	}
	else
	{
		return true;
	}
}

/*
 * 该函数应用于i2c主设备端,用于i2c主设备向i2c从设备请求命令
 */
uint8_t dbg_cmd_conf(DEBUG_CMD_TYPE cmd)
{
	uint8_t ret = RET_OK;

	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, DEBUG_CMD_START_ADDR, cmd);

	if(dbg_cmd_get_status() == false)
	{
		lib_log("\r\n dbg_cmd_conf failed");
		ret = RET_FAILED;
	}

	return ret;
}


/*
 * 该函数应用于i2c主设备端,用于i2c主设备访问i2c从设备内部的i2c从设备
 */
uint8_t i2c_pass_through_read(uint8_t dev_addr, uint8_t page, uint16_t offset_addr, uint8_t *value, uint8_t len)
{
	CONFIG_STRUCT_T dbg_conf;
	uint8_t ret = RET_FAILED;

	dbg_conf.cmd 			= CMD_TYPE_READ;
	dbg_conf.dev_addr   	= dev_addr;
	dbg_conf.offset     	= offset_addr;   //因为这个变量是16bit的，所以移植后注意不同芯片大小端的问题
	dbg_conf.len 			= len;
	dbg_conf.page 			= page;

	i2c_master_write_byte(I2C_DEVICE_ADDR, PAGE_SELECT_BYTE, page);
	ret = i2c_master_write(I2C_DEVICE_ADDR, DEBUG_CMD_START_ADDR + 1, (uint8_t *)&dbg_conf.dev_addr, sizeof(dbg_conf) - 1);

	ret = dbg_cmd_conf(CMD_TYPE_READ);

	if(ret == RET_FAILED)
	{
		return RET_FAILED;
	}

	ret = i2c_master_read(I2C_DEVICE_ADDR, DEBUG_CMD_START_ADDR + sizeof(dbg_conf), value, len);
	return ret;
}

/*
 * 该函数应用于i2c从设备端,用于i2c主设备访问i2c从设备内部的i2c从设备
 */
uint8_t i2c_pass_through_write(uint8_t dev_addr, uint8_t page,uint16_t offset_addr, uint8_t *value, uint8_t len)
{
	CONFIG_STRUCT_T dbg_conf;
	uint8_t ret = RET_FAILED;

	dbg_conf.cmd 	   = CMD_TYPE_WRITE;
	dbg_conf.dev_addr  = dev_addr;
	dbg_conf.offset    = offset_addr; //因为这个变量是16bit的，所以移植后注意不同芯片大小端的问题
	dbg_conf.len 	   = len;
	dbg_conf.page 	   = page;

	i2c_master_write_byte(I2C_DEVICE_ADDR, PAGE_SELECT_BYTE, page);
	ret = i2c_master_write(I2C_DEVICE_ADDR, DEBUG_CMD_START_ADDR + 1, (uint8_t *)&dbg_conf.dev_addr, sizeof(dbg_conf) - 1);

	ret = i2c_master_write(I2C_DEVICE_ADDR, DEBUG_CMD_START_ADDR + sizeof(dbg_conf), (uint8_t *)value, dbg_conf.len);

	ret = dbg_cmd_conf(CMD_TYPE_WRITE);

	if(ret == RET_FAILED)
	{
		return RET_FAILED;
	}

	return ret;
}

/*
 * 该函数应用于i2c从设备端,用于i2c主设备的请求命令的处理，调用格式如下:
 * uint8_t buff[128];
 * dbg_cmd_handle((DEBUG_CMD_STRUCT_T *)&buff[OFFSET_INDEX(MSA_PAGE4_DEBUG_CMD)]);
 */
void dbg_cmd_handle(DBG_CMD_STRUCT_T *dbg_cmd)
{
	uint8_t  ret = RET_OK, i = 0;

	if(dbg_cmd == NULL)
	{
		return;
	}


	if (dbg_cmd->config.cmd != CMD_TYPE_IDEL &&
		dbg_cmd->config.cmd != RET_OK        &&
		dbg_cmd->config.cmd != RET_FAILED)
	{
		switch(dbg_cmd->config.cmd)
		{
			case CMD_TYPE_READ:  // read
			{
				if (dbg_cmd->config.dev_addr == I2C_DEVICE_ADDR)
				{
					switch(dbg_cmd->config.page)
					{
						default:
							break;
					}
				}
				else
				{
					ret = i2c_master_read(dbg_cmd->config.dev_addr, dbg_cmd->config.offset, dbg_cmd->buf, dbg_cmd->config.len);
				}
				break;
			}
			case CMD_TYPE_WRITE: 	// write
			{
				if (dbg_cmd->config.dev_addr == I2C_DEVICE_ADDR)
				{

				}
				else
				{
					ret = i2c_master_write(dbg_cmd->config.dev_addr, dbg_cmd->config.offset, dbg_cmd->buf, dbg_cmd->config.len);
				}
				break;
			}
			default:
				break;
		}

		if(ret != RET_FAILED)
		{
			dbg_cmd->config.cmd = RET_OK;
		}
		else
		{
			dbg_cmd->config.cmd = RET_FAILED;
		}
	}
}

#endif
