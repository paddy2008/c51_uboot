#include "mcu_sal.h"
#include "Efm8lb12_cmd.h"
#include "Efm8lb12_port.h"
#include "Efm8lb12_qsfp28.h"

TIMER_STRUCT_T prbs_code_timer;
TIMER_STRUCT_T rssi_poll_timer;
TIMER_STRUCT_T intl_trigger_timer;

/************************************************function declaration************************************************************/
#if 0
uint8_t smb_read_multi_byte_test(uint8_t BaseAddr,uint16_t OffsetAddr,uint8_t *Pdata,uint16_t Length)
{
	uint32_t i = 0;

	for (i = 0;SMB_BUSY;i++); // Wait for SMBus to be free.
	SMB_BUSY = 1;                       // Claim SMBus (set to busy)

	// Set SMBus ISR parameters
	TARGET = BaseAddr;               	// Set target slave address
	SMB_RW = WRITE;                     // A random read starts as a write
									    // then changes to a read after
									    // the repeated start is sent. The
									    // ISR handles this switchover if
									    // the <SMB_RANDOMREAD> bit is set.
	SMB_SENDWORDADDR = 1;               // Send Word Address after Slave Address
	SMB_RANDOMREAD = 1;                 // Send a START after the word address
	SMB_ACKPOLL = 0;                    // Enable Acknowledge Polling

	if(BaseAddr == 0xa8 || BaseAddr == 0xb8 || BaseAddr == 0x24) //12bit offsetaddr
    {
    	WORD_ADDR_HIGH = (OffsetAddr & 0xFF00) >> 8;
    	WORD_ADDR_LOW = OffsetAddr & 0xFF;
    	gSingleByteOffsetAddr = false;
    }
    else
    {
    	WORD_ADDR_LOW = OffsetAddr & 0xFF;
    	gSingleByteOffsetAddr = true;
    }

	// Set the the incoming data pointer
	pSMB_DATA_IN = Pdata;
	SMB_DATA_LEN = Length;                // Specify to ISR that the next transferwill contain <len> data bytes

	// Initiate SMBus Transfer
	SMB0CN0_STA = 1;

	/*
	for(i = 0;SMB_BUSY;i++)                       // Wait until data is read
	{
		if (i >= 0xfffff)
		{
 			uart_printf("\r\nRead multi type timeout,reset smbus");
			SMB0CF &= ~0x80;           // Reset communication
			SMB0CF |= 0x80;
			SMB0CN0_STA = 0;
			SMB0CN0_STO = 0;
			SMB0CN0_ACK = 0;
			SMB_BUSY = 0;              // Free SMBus
			SMB_ACKPOLL = 0;
			return RET_FAILED;
		}
	}
	*/

	return RET_OK;
}

#endif

static RETURN_VALUE_TYPE prvDispEepConfig(PARAM_INFO_T  *param)//eep [init|show|regset|erase] devaddr offsetaddr value
{
	uint8_t ret = RET_OK;

	if(!strcmp(param->argv[1],"init"))
	{
		ret = qsfp28_eep_handle(EEP_INIT, 0, 0, 0);
	}
	else if(!strcmp(param->argv[1],"show"))
	{
		ret = qsfp28_eep_handle(EEP_SHOW, 0, 0, 0);
	}
	else if(!strcmp(param->argv[1],"regset"))
	{
		SAVE_REG_T save_reg;
		save_reg.devAddr    = com_str_to_int16(param->argv[2]);
		save_reg.offsetAddr = com_str_to_int16(param->argv[3]);
		save_reg.value      = com_str_to_int16(param->argv[4]);
		ret = qsfp28_eep_handle(EEP_SAVE, save_reg.devAddr, save_reg.offsetAddr, save_reg.value);
	}
	else if(!strcmp(param->argv[1],"erase"))
	{
		ret = qsfp28_eep_handle(EEP_ERASE, 0, 0, 0);
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispOprwConfig(PARAM_INFO_T  *param)
{
	I2C_DATA_T i2c_data;
	uint8_t databuff[200] = {0};  //read 200 bytes at one time
	uint8_t result = RET_FAILED;
	uint8_t page = 0;

	i2c_data.devAddr 	= com_str_to_int16(param->argv[1]);
	i2c_data.offsetAddr = com_str_to_int16(param->argv[2]);
	i2c_data.readData   = databuff;
	i2c_data.writeData  = databuff;
	i2c_data.i2c_dbg_mode = false;  //false:normal i2c mode,true:dbg i2c mode

	if ((!strcmp(param->argv[0],"opw"))||(!strcmp(param->argv[0],"opwt")))
	{
		i2c_data.rwType = WRITE_CMD;
	}
	else if ((!strcmp(param->argv[0],"opr"))||(!strcmp(param->argv[0],"oprt")))
	{
		i2c_data.rwType = READ_CMD;
	}

	if(i2c_data.rwType == READ_CMD)
	{
		if(param->argc < 3)
		{
			return RET_FAILED;
		}
		else if(param->argc == 3)
		{
			i2c_data.rwByteNum = 1;
		}
		else
		{
			i2c_data.rwByteNum = (com_str_to_int16(param->argv[3]) > 200) ? 200 : com_str_to_int16(param->argv[3]);
		}
	}
	else if(i2c_data.rwType == WRITE_CMD)
	{
		uint8_t i = 0, index = 0;

		if(param->argc < 4)
		{
			return RET_FAILED;
		}
		else if(param->argc == 0x4)
		{
			i2c_data.rwByteNum = param->argc - 3;
			index = 3;
		}
		else
		{
			i2c_data.rwByteNum = ((param->argc - 4) > 128) ? 128 : (param->argc - 4);
			index = 4;
		}

		for(i= 0; i < i2c_data.rwByteNum;i++ )
		{
			databuff[i] = com_str_to_int16(param->argv[index + i]);
		}
	}
	else
	{
		return RET_FAILED;
	}

	if((strcmp(param->argv[0],"opwt")) && strcmp(param->argv[0],"oprt"))
	{
		if(i2c_data.devAddr != I2C_DEVICE_ADDR && i2c_data.devAddr != I2C_DEVICE_ADDR1)
		{
			i2c_data.i2c_dbg_mode = true;
		}
		else
		{
			i2c_master_read_byte(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, &page);
		}
	}

	if(i2c_data.rwType == READ_CMD)
	{
		if((i2c_data.i2c_dbg_mode == true) && (i2c_data.devAddr != I2C_DEVICE_ADDR))
		{
			result = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
			JUDGE_RETURN(result);
			result = i2c_pass_through_read(i2c_data.devAddr, page, i2c_data.offsetAddr, i2c_data.readData, i2c_data.rwByteNum);
		}
		else
		{
			result = i2c_master_read(i2c_data.devAddr, i2c_data.offsetAddr,  i2c_data.readData, i2c_data.rwByteNum);
		}
	}
	else
	{
		if(i2c_data.i2c_dbg_mode == true)
		{
			result = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
			JUDGE_RETURN(result);
			result = i2c_pass_through_write(i2c_data.devAddr, page, i2c_data.offsetAddr, i2c_data.readData, i2c_data.rwByteNum);
		}
		else
		{
			result = i2c_master_write(i2c_data.devAddr, i2c_data.offsetAddr, i2c_data.readData, i2c_data.rwByteNum);
		}
	}

	if(result == RET_OK)
	{
		uart_printf("\r\n The value on page (0x%bx), index(0x%x) : ",i2c_data.devAddr,i2c_data.offsetAddr);
		while(i2c_data.rwByteNum --)
		{
			uart_printf(" 0x%bx",*(i2c_data.readData++));
		}
	}
	return result;
}


static RETURN_VALUE_TYPE prvDispMdiorwConfig(PARAM_INFO_T  *param)
{
	I2C_DATA_T i2c_data;
	uint16_t databuff[10] = {0};  //
	uint8_t result = RET_OK;
	uint8_t page = 0;
	uint32_t ret = 0;

	i2c_data.devAddr 	= com_str_to_int16(param->argv[1]);  //mdior 0x3 0x5
	i2c_data.offsetAddr = com_str_to_int16(param->argv[2]);

	//i2c_data.readData   = databuff;
	//i2c_data.writeData  = databuff;
	i2c_data.i2c_dbg_mode = false;  //false:normal i2c mode,true:dbg i2c mode

	if (!strcmp(param->argv[0],"mdiow"))
	{
		i2c_data.rwType = WRITE_CMD;
	}
	else if (!strcmp(param->argv[0],"mdior"))
	{
		i2c_data.rwType = READ_CMD;
	}

	if(i2c_data.rwType == READ_CMD)
	{
		if(param->argc < 3)
		{
			return RET_FAILED;
		}
		else if(param->argc == 3)
		{
			i2c_data.rwByteNum = 1;
		}
		else
		{
			i2c_data.rwByteNum = (com_str_to_int16(param->argv[3]) > 200) ? 200 : com_str_to_int16(param->argv[3]);
		}
	}
	else if(i2c_data.rwType == WRITE_CMD)
	{
		uint8_t i = 0, index = 0;

		if(param->argc < 4)
		{
			return RET_FAILED;
		}
		else if(param->argc == 0x4)
		{
			i2c_data.rwByteNum = param->argc - 3;
			index = 3;
		}
		else
		{
			i2c_data.rwByteNum = ((param->argc - 4) > 128) ? 128 : (param->argc - 4);
			index = 4;
		}

		for(i= 0; i < i2c_data.rwByteNum;i++ )
		{
			databuff[i] = com_str_to_int16(param->argv[index + i]);
		}
	}
	else
	{
		return RET_FAILED;
	}


	if(i2c_data.rwType == READ_CMD)
	{
		//uint32_t reg =  i2c_data.devAddr << 16 | i2c_data.offsetAddr;
		//ret = (uint16_t)mdio_bb_read(0x1, reg);

		ret = mdio_read(0x1, i2c_data.devAddr, i2c_data.offsetAddr);

		uart_printf("\r\n The value on page (0x%bx), index(0x%hx) : ",i2c_data.devAddr,i2c_data.offsetAddr);

		while(i2c_data.rwByteNum --)
		{
			uart_printf(" 0x%lx", ret);
		}
	}
	else
	{
		//uint32_t reg =  i2c_data.devAddr << 16 | i2c_data.offsetAddr;
		//mdio_bb_write(0x1, reg, databuff[0]);

		mdio_write(0x1, i2c_data.devAddr, i2c_data.offsetAddr, databuff[0]);

		uart_printf("\r\n The value on page (0x%bx), index(0x%x) : ",i2c_data.devAddr,i2c_data.offsetAddr);

		while(i2c_data.rwByteNum --)
		{
			uart_printf(" 0x%hx", databuff[0]);
		}
	}


	return result;
}

static RETURN_VALUE_TYPE prvDispVersionConfig(PARAM_INFO_T  *param)
{
	uart_printf( "\r\n MCU_TEST_BOARD-v%bu.%bu    %s.%s", VERSION_MAJOR, VERSION_MINOR, __DATE__,__TIME__);

	param = NULL;
	return RET_OK;
}

static RETURN_VALUE_TYPE prvDispConfigModConfig(PARAM_INFO_T  *param)
{
	RETURN_VALUE_TYPE ret = RET_OK;

	if(!strcmp(param->argv[1],"enable"))
	{
		ret = qsfp28_config_mod_console(true);
	}
	else
	{
		ret = qsfp28_config_mod_console(false);
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispMsaDownloadConfig()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x0;
	P1MDOUT &= ~P1MDOUT_B5__PUSH_PULL ;

	SFRPAGE = 0x20;
	P2MDOUT &= ~P2MDOUT_B0__PUSH_PULL ;

	RESET  = 1;
	MODSEL = 1;

	SFRPAGE = SFRPAGE_SAVE;
	return RET_OK;
}

static RETURN_VALUE_TYPE prvDispResetConfig()
{
	uint8_t ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, DBG_RESET_CMD);
	return ret;
}

static RETURN_VALUE_TYPE prvDispRebootConfig()
{
	uint8_t ret = RET_OK;

	ret = prvDispResetConfig();
	JUDGE_RETURN(ret);
	//M37046_txcdr_reset();
	//M37049_txcdr_reset();
	RSTSRC |= 0x10;			//Writing a 1 to fourth bit forces a system reset
	return ret;
}

static RETURN_VALUE_TYPE prvDispLpmodeConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;

	if (!strcmp(param->argv[0], "set"))
	{
		value = (uint8_t)com_str_to_int16(param->argv[2]);
		LPMODE = (value ? 1 : 0);
	}
	else if (!strcmp(param->argv[0], "get"))
	{
		uart_printf( "\r\nlpmode is %bu",LPMODE ? 1 : 0);
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispgsetResetConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;

	if (!strcmp(param->argv[0], "set"))
	{
		value = (uint8_t)com_str_to_int16(param->argv[2]);
		RESET = (value ? 1 : 0);
		com_delay_us(1);
		RESET = 1;
	}
	else if (!strcmp(param->argv[0], "get"))
	{
		uart_printf( "\r\nreset is %bu",RESET ? 1 : 0);
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispgsetResetLevelConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;

	if (!strcmp(param->argv[0], "set"))
	{
		value = (uint8_t)com_str_to_int16(param->argv[2]);
		RESET = (value ? 1 : 0);
	}
	else if (!strcmp(param->argv[0], "get"))
	{
		uart_printf( "\r\nreset is %bu",RESET ? 1 : 0);
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispLogConfig(PARAM_INFO_T  *param)
{
	DEBUG_LOG_ACTION type;

	if(!strcmp(param->argv[0],"clear"))
	{
		type = CLEAR_LOG_CMD;
	}
	else if(!strcmp(param->argv[0],"enable"))
	{
		type = ENABLE_LOG_CMD;
	}
	else if(!strcmp(param->argv[0],"disable"))
	{
		type = DISABLE_LOG_CMD;
	}
	else if(!strcmp(param->argv[0],"read"))
	{
		type = READ_LOG_CMD;
	}
	else
	{
		return RET_FAILED;
	}

	return qsfp28_debug_log_handle(type);
}

static RETURN_VALUE_TYPE prvDispModselConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;

	if (!strcmp(param->argv[0], "set"))
	{
		value = (uint8_t)com_str_to_int16(param->argv[2]);
		MODSEL = (value ? 1 : 0);
	}
	else if (!strcmp(param->argv[0], "get"))
	{
		uart_printf( "\r\nmodsel is %bu",MODSEL ? 1 : 0);
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispTestModselConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, tmp[256] = {0},ret = RET_OK;

	if (!strcmp(param->argv[1], "modsel"))  //test modsel 0|1
	{
		if (!strcmp(param->argv[0], "test"))  //test modsel 0|1
		{
			if (!strcmp(param->argv[2], "aborted"))  //test modsel aborted
			{
				//smb_read_multi_byte_test(I2C_DEVICE_ADDR, 0x0, &tmp, 256); //12.5ms
				timer_delay_ms(6);
				MODSEL = 1;

				#if I2C_MASTER_MODE
					i2c_master_reset();
				#endif
				com_delay_us(100);

				if(RET_OK == i2c_master_read_byte(0x16, 0x0, &value))
				{
					uart_printf( "\r\nmodsel is %bu and dvt test board 37046 reg0 = %bx",MODSEL ? 1 : 0, value);
				}

				MODSEL = 0;
				if(RET_OK == i2c_master_read_byte(I2C_DEVICE_ADDR, 0x0, &value))
				{
					uart_printf( "\r\nmodsel is %bu and qsfp28 lower page0 = %bx",MODSEL ? 1 : 0, value);
				}

			}
			else
			{
				value = (uint8_t)com_str_to_int16(param->argv[2]);
				MODSEL = (value ? 1 : 0);
				com_delay_us(1);
				if(RET_OK == i2c_master_read_byte(I2C_DEVICE_ADDR, 0x0, &value))
				{
					uart_printf( "\r\nmodsel is %bu and value = %bx",MODSEL ? 1 : 0, value);
				}
			}
		}
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static uint8_t prvDispInitConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;

	param = NULL;

	uart_printf( "\r\nintl is %bu",INTL ? 1 : 0);
	return ret;
}

static RETURN_VALUE_TYPE prvDispSwverConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;

	if (!strcmp(param->argv[0], "get"))
	{
		ret = qsfp28_get_mod_release_info();
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispModStatusConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;

	if (!strcmp(param->argv[0], "get"))
	{
		if(com_get_module_boot_status())
		{
			ret = qsfp28_get_mod_release_info();
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else
	{
		ret = RET_FAILED;
	}
	if(ret == RET_OK)
	{
		uart_printf("%s","\r\nModule boot success");
	}
	else
	{
		uart_printf("%s","\r\nModule boot error");
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispModuleConfig(PARAM_INFO_T  *param)
{
	uint8_t value = 0, channel = 0, ret = RET_OK;

	 ret = qsfp28_get_msa_lower_page_monitor();

	 if(!strcmp(param->argv[2],"vcc"))
	 {
		 ret = qsfp28_get_ddmi(DDMI_VCC, 0, DDMI_CALI_AFTER);
	 }
	 else if(!strcmp(param->argv[2],"temp"))
	 {
		 ret = qsfp28_get_ddmi(DDMI_TEMP, 0, DDMI_CALI_AFTER);
	 }
	 else
	 {
		 channel = (uint8_t)com_str_to_int16(param->argv[3]);

		 if(!strcmp(param->argv[2],"bias"))
		 {
			 ret = qsfp28_get_ddmi(DDMI_BIAS, channel, DDMI_CALI_AFTER);
		 }
		 else if(!strcmp(param->argv[2],"txpower"))
		 {
			 ret = qsfp28_get_ddmi(DDMI_TXMON, channel, DDMI_CALI_AFTER);
		 }
		 else if(!strcmp(param->argv[2],"rxpower"))
		 {
			 ret =  qsfp28_get_ddmi(DDMI_RSSI, channel, DDMI_CALI_AFTER);
		 }
		 else
		 {
			 ret = RET_FAILED;
		 }
	 }

	 return ret;
}

static RETURN_VALUE_TYPE prvDispVendorPnConfig(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_VENDOR_PN, param->argv[2], MSA_VENDOR_PN_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_VENDOR_PN, param->argv[2], MSA_VENDOR_PN_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nVender_pn:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispVendorRevConfig(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_VENDOR_REV, param->argv[2], MSA_VENDOR_REV_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_VENDOR_REV, param->argv[2], MSA_VENDOR_REV_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nVender_rev:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispVendorSnConfig(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_VENDOR_SN, param->argv[2], MSA_VENDOR_SN_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_VENDOR_SN, param->argv[2], MSA_VENDOR_SN_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nVender_sn:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispDateCodeConfig(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_DATA_CODE, param->argv[2], MSA_DATA_CODE_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, MSA_PAGE_00H, MSA_DATA_CODE, param->argv[2], MSA_DATA_CODE_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nDate_code:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispSuperCodeConfig(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, SUPERCODE, param->argv[2], SUPERCODE_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, SUPERCODE, param->argv[2], SUPERCODE_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nSupercode:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispPcbaSnConfig(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, PCBA_SN, param->argv[2], PCBA_SN_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, PCBA_SN, param->argv[2], PCBA_SN_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nPcba_sn:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispPcbaCodeConfig(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, PCBA_CODE, param->argv[2], PCBA_CODE_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, PCBA_CODE, param->argv[2], PCBA_CODE_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nPcba_code:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispBondingConfig()
{
	return qsfp28_msa_page4_dbg_cmd(DBG_BONDING_MODE);
}

static RETURN_VALUE_TYPE prvDispFactoryResetConfig()
{
	return qsfp28_msa_page4_dbg_cmd(DBG_FACTORY_RESET_CMD);
}

static uint8_t _cli_laser_tx_sn_cmd(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, LASER_TX_SN, param->argv[2], LASER_TX_SN_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H,  LASER_TX_SN, param->argv[2], LASER_TX_SN_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nLaser_tx_sn:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_rx_losa_cmd(PARAM_INFO_T  *param)
{
	uint16_t  value16 = 0;
	uint8_t ret = RET_OK, channel = 0;

	const uint8_t losa_addr[] = {RX_LOSA_THLD_CH0_MSB, RX_LOSA_THLD_CH1_MSB,RX_LOSA_THLD_CH2_MSB,RX_LOSA_THLD_CH3_MSB};

	if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1])))
	{
		return RET_FAILED;
	}

	if (!strcmp(param->argv[0],"set"))   //set [ch0|ch1|ch2|ch3] losa_thld value(uA)
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			value16 = com_str_to_int16(param->argv[3]);
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, losa_addr[channel], (uint8_t *)&value16, 2);
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, losa_addr[channel], (uint8_t *)&value16, 2);
			JUDGE_RETURN(ret);
			uart_printf("\r\nRx_losa_thld%bu:%hu",channel, value16);
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_rx_losd_cmd(PARAM_INFO_T  *param)
{
	uint16_t  value16 = 0;
	uint8_t   ret = RET_OK, channel = 0;

	const uint8_t losd_addr[] = {RX_LOSD_THLD_CH0_MSB, RX_LOSD_THLD_CH1_MSB,RX_LOSD_THLD_CH2_MSB,RX_LOSD_THLD_CH3_MSB};

	if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1])))
	{
		return RET_FAILED;
	}

	if (!strcmp(param->argv[0],"set"))   //set [ch0|ch1|ch2|ch3] losd_thld value(uA)
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			value16 = com_str_to_int16(param->argv[3]);
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, losd_addr[channel], (uint8_t *)&value16, 2);
		}
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, losd_addr[channel], (uint8_t *)&value16, 2);
			JUDGE_RETURN(ret);
			uart_printf("\r\nRx_losd_thld%bu:%hu",channel, value16);
		}
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_rx_los_mode_cmd(PARAM_INFO_T  *param)
{
	uint8_t  value8 = 0, ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))   //set rx_los mode [0|1]
	{
		value8 = com_str_to_int16(param->argv[3]);
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, RX_LOS_MODE, (uint8_t *)&value8, 1);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, RX_LOS_MODE, (uint8_t *)&value8, 1);
		JUDGE_RETURN(ret);
		uart_printf("\r\nRx_los_mode:%bu",value8);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_rssi_darka_cmd(PARAM_INFO_T  *param)
{
	uint16_t  value16 = 0;
	uint8_t ret = RET_OK, channel = 0, i = 0;

	const uint8_t rssi_darka_addr[] = {RSSI_DRAKA_THLD_CH0_MSB, RSSI_DRAKA_THLD_CH1_MSB,RSSI_DRAKA_THLD_CH2_MSB,RSSI_DRAKA_THLD_CH3_MSB};

	if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1])))
	{
		return RET_FAILED;
	}

	if (!strcmp(param->argv[0],"set"))   //set [ch0|ch1|ch2|ch3|all] darka_thld value(uA)
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			value16 = com_str_to_int16(param->argv[3]);
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darka_addr[channel], (uint8_t *)&value16, 2);
		}
		else if(channel == CHANNEL_MAX)
		{
			for(i = 0; i < CHANNEL_MAX; i++)
			{
				value16 = com_str_to_int16(param->argv[3]);
				ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darka_addr[i], (uint8_t *)&value16, 2);
				JUDGE_RETURN(ret);
			}
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darka_addr[channel], (uint8_t *)&value16, 2);
			JUDGE_RETURN(ret);
			uart_printf("\r\nRssi_darka_thld%bu:%hu",channel, value16);
		}
		else if(channel == CHANNEL_MAX)
		{
			for(i = 0; i < CHANNEL_MAX; i++)
			{
				ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darka_addr[i], (uint8_t *)&value16, 2);
				JUDGE_RETURN(ret);
				uart_printf("\r\nRssi_darka_thld%bu:%hu",i, value16);
			}
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_rssi_darkd_cmd(PARAM_INFO_T  *param)
{
	uint16_t  value16 = 0;
	uint8_t   ret = RET_OK, channel = 0, i = 0;

	const uint8_t rssi_darkd_addr[] = {RSSI_DRAKD_THLD_CH0_MSB, RSSI_DRAKD_THLD_CH1_MSB,RSSI_DRAKD_THLD_CH2_MSB,RSSI_DRAKD_THLD_CH3_MSB};

	if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1])))
	{
		return RET_FAILED;
	}

	if (!strcmp(param->argv[0],"set"))   //set [ch0|ch1|ch2|ch3] darkd_thld value(uA)
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			value16 = com_str_to_int16(param->argv[3]);
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darkd_addr[channel], (uint8_t *)&value16, 2);
		}
		else if(channel == CHANNEL_MAX)
		{
			for(i = 0; i < CHANNEL_MAX; i++)
			{
				value16 = com_str_to_int16(param->argv[3]);
				ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darkd_addr[i], (uint8_t *)&value16, 2);
				JUDGE_RETURN(ret);
			}
		}
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
		{
			ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darkd_addr[channel], (uint8_t *)&value16, 2);
			JUDGE_RETURN(ret);
			uart_printf("\r\nRssi_darkd_thld%bu:%hu",channel, value16);
		}
		else if(channel == CHANNEL_MAX)
		{
			for(i = 0; i < CHANNEL_MAX; i++)
			{
				ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_08H, rssi_darkd_addr[i], (uint8_t *)&value16, 2);
				JUDGE_RETURN(ret);
				uart_printf("\r\nRssi_darkd_thld%bu:%hu",i, value16);
			}
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_tx_config_laser(uint8_t channel, uint8_t enable)
{
	uint8_t value = 0, ret = RET_OK, tx_dis = 0;

	ret = i2c_master_read(I2C_DEVICE_ADDR, MSA_TX_ENABLE, (uint8_t *)&tx_dis, 1);

	tx_dis = tx_dis & 0x0F;

	if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
	{
		tx_dis = ((enable) ? (MY_CLEAR_BIT(tx_dis, channel)) : (MY_SET_BIT(tx_dis, channel)));  //enable = 1, Disable laser , enable = 0, Enable laser
	}
	else if(channel == CHANNEL_MAX)
	{
		tx_dis =  ((enable) ? (U8_CLEAR_LSB_4BITS(tx_dis)) : (U8_SET_LSB_4BITS(tx_dis)));
	}

	ret = i2c_master_write(I2C_DEVICE_ADDR, MSA_TX_ENABLE, (uint8_t *)&tx_dis, 1);

	return ret;
}

static uint8_t _cli_tx_get_laser_status(uint8_t channel)
{
	uint8_t tmp = 0, value = 0, ret = RET_OK;

	if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
	{
		ret = i2c_master_read(I2C_DEVICE_ADDR, MSA_TX_ENABLE, (uint8_t *)&value, 1);
		JUDGE_RETURN(ret);
		tmp = MY_GET_BIT(value, channel);
		uart_printf("\r\nPrbs%bu:%bu",channel, tmp);
	}
	else if(channel == CHANNEL_MAX)
	{
		for(channel = 0; channel  < CHANNEL_MAX; channel ++)
		{
			ret = i2c_master_read(I2C_DEVICE_ADDR, MSA_TX_ENABLE, (uint8_t *)&value, 1);
			JUDGE_RETURN(ret);
			tmp = MY_GET_BIT(value, channel);
			uart_printf("\r\nPrbs%bu:%bu",channel, tmp);
		}
	}
	return ret;
}

static uint8_t _cli_tx_laser_cmd(PARAM_INFO_T  *param)
{
	uint8_t  channel = 0, ret = RET_OK;

	channel = _qsfp28_get_channel(param->argv[1]);

	if (!strcmp(param->argv[0],"set"))
	{
		if (!strcmp(param->argv[3],"enable"))  //[set|get] [ch0|ch1|ch2|ch3|all] tx [enable|disable]
		{
			ret = _cli_tx_config_laser(channel, ENABLE_LAESER);
		}
		else if(!strcmp(param->argv[3],"disable"))
		{
			ret = _cli_tx_config_laser(channel, DISABLE_LAESER);
		}
	}
	else if(!strcmp(param->argv[0],"get"))
	{
		ret = _cli_tx_get_laser_status(channel);
	}
	return ret;
}

static uint8_t _cli_laser_rx_sn_cmd(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, LASER_RX_SN, param->argv[2], LASER_RX_SN_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, LASER_RX_SN, param->argv[2], LASER_RX_SN_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nLaser_rx_sn:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_pic_code_cmd(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, PICCODE, param->argv[2], PICCODE_LEN);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, PICCODE, param->argv[2], PICCODE_LEN);
		JUDGE_RETURN(ret);
		uart_printf("\r\nPic_code:%s",param->argv[2]);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_module_type_cmd(PARAM_INFO_T  *param)
{
	uint8_t  ret = RET_OK;
	uint16_t value16 = 0;

	if (!strcmp(param->argv[0],"set"))
	{
		value16 = com_str_to_int16(param->argv[2]);
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, MSA_OFF_MODULE_TYPE, (uint8_t *)&value16, 2);
	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, MSA_OFF_MODULE_TYPE, (uint8_t *)&value16, 2);
		JUDGE_RETURN(ret);
		uart_printf("\r\nModule_type:0x%hx",value16);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_hw_ver_cmd(PARAM_INFO_T  *param)
{
	uint8_t  value = 0, ret = RET_OK;

	if (!strcmp(param->argv[0],"set"))
	{
		value = (uint8_t)com_str_to_int16(param->argv[2]);
		ret = smb_write_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, MSA_OFF_HW_VER, (uint8_t *)&value, 1);

	}
	else if (!strcmp(param->argv[0],"get"))
	{
		ret = smb_read_page_byte(I2C_DEVICE_ADDR, SFP_PAGE_05H, MSA_OFF_HW_VER, (uint8_t *)&value, 1);
		JUDGE_RETURN(ret);
		uart_printf("\r\nHw_ver:%bx",value);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}


static uint8_t _cli_set_lut_tab(LUT_SHOW_TYPE show_type, MSA_LUT_TYPE lut_type, PARAM_INFO_T  *param)
{
	MSA_LASER_CH_CFG_T cfg;
	uint8_t  channel = 0, ret = RET_OK;

	 ret = ((RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1]))) ? RET_FAILED : RET_OK);

	 if(channel == CHANNEL_MAX)
	 {
		 ret = RET_FAILED;
	 }
	 else if (!strcmp(param->argv[0],"set"))
	 {
		if(!strcmp(param->argv[3],"now"))
		{
			ret = qsfp28_set_lut_tab_para_from_reg_inner(channel, lut_type + 1);
			return ret;
		}
		cfg.temp  = com_str_to_int16(param->argv[3]);
		cfg.bias  = com_str_to_int16(param->argv[4]);
		cfg.mod	  = com_str_to_int16(param->argv[5]);
		cfg.cross = com_str_to_int16(param->argv[6]);
		cfg.temp  = cfg.temp * DDMI_CALI_TEMP_SCALE;   			//because the module receive result is actual temp * 256
		ret = qsfp28_set_lut_tab(channel, lut_type, cfg);
	 }
	 else if (!strcmp(param->argv[0],"get"))
	 {
		ret = qsfp28_get_lut_tab(channel, show_type, lut_type);
	 }
	 else
	 {
		 ret = RET_FAILED;
	 }

	 return ret;
}

static uint8_t _cli_get_all_lut_tab()
{
	uint8_t channel = 0, ret = RET_OK;

	uart_printf("\r\nchN	temp     bias    mod     crossing");
	uart_printf("\r\n---------------------------------------");
	for(channel = 0; channel < CHANNEL_MAX; channel++)
	{
		ret = qsfp28_get_lut_tab(channel,ALL_LUT_INFO, MSA_LUT_TYPE_LOW_TEMP);
		ret = qsfp28_get_lut_tab(channel,ALL_LUT_INFO, MSA_LUT_TYPE_MID_TEMP);
		ret = qsfp28_get_lut_tab(channel,ALL_LUT_INFO, MSA_LUT_TYPE_HI_TEMP);
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispLuttabConfig(PARAM_INFO_T  *param) //[get|set] [ch0|ch1|ch2|ch3] #[cur|fixed_temp] lut tab
{
	uint8_t channel = 0, ret = RET_OK;
	int16_t value16 = 0;

	if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1])))
	{
		return RET_FAILED;
	}

	if (!strcmp(param->argv[0],"get"))
	{
		if (!strcmp(param->argv[2],"cur")) //get ch1 [cur|fixed_temp] lut tab
		{
			ret = qsfp28_get_cur_apc_value(channel);
		}
		else
		{
			value16 = com_str_to_int16(param->argv[2]);
			ret = qsfp28_get_fixed_temp_point_apc_value(value16, channel);
		}
	}

	/*
	else if (!strcmp(param->argv[0],"set"))
	{
		if (!strcmp(param->argv[2],"cur")) //set ch1 [cur|fixed_temp] lut tab
		{
			ret = qsfp28_set_cur_apc_value(channel);
		}
		else
		{
			value16 = com_str_to_int16(param->argv[2]);
			ret = qsfp28_set_fixed_temp_point_apc_value((uint8_t)value16, channel);
		}
	}
	*/

	return ret;
}

static uint8_t _cli_ddmi_cali(PARAM_INFO_T  *param)
{
	uint8_t channel = 0, ret = RET_OK;
	DDMI_CALI_PARA_T cali_para;

	if (!strcmp(param->argv[2],"temp_offset")) //set|get ddmi_cali temp_offset
	{
		if (!strcmp(param->argv[0],"set"))
		{
			if(CHECK_DDMI_CALI_TEMP_OFFSET(atof(param->argv[3])))
			{
				cali_para.temp = (int16_t)(atof(param->argv[3]) * (float)DDMI_CALI_TEMP_SCALE);
				ret = qsfp28_set_ddmi_cali_para(DDMI_TEMP, 0, &cali_para);  //uint:1/256 C
			}
			else
			{
				uart_printf( "\r\n Temp_offset is out of range");
				ret = RET_FAILED;
			}
		}
		else if (!strcmp(param->argv[0],"get"))
		{
			ret = qsfp28_get_ddmi_cali_para(DDMI_TEMP, 0, &cali_para);//uint:1/256 C
			JUDGE_RETURN(ret);
			uart_printf( "\r\n temp_offset:%f(C)", (float)cali_para.temp / (float)DDMI_CALI_TEMP_SCALE);
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else if (!strcmp(param->argv[2],"vcc_offset")) //set|get ddmi_cali_para vcc_offset
	{
		if (!strcmp(param->argv[0],"set"))
		{
			if(CHECK_DDMI_CALI_VCC_OFFSET(atof(param->argv[3])))
			{
				cali_para.vcc = (int16_t)(atof(param->argv[3]) * 1000.0 * 10);  //uint:0.1mv
				ret = qsfp28_set_ddmi_cali_para(DDMI_VCC, 0, &cali_para);
			}
			else
			{
				uart_printf( "\r\n Vcc_offset is out of range");
				ret = RET_FAILED;
			}
		}
		else if (!strcmp(param->argv[0],"get"))
		{
			ret = qsfp28_get_ddmi_cali_para(DDMI_VCC, 0, &cali_para);//uint:0.1mv
			JUDGE_RETURN(ret);
			uart_printf( "\r\n vcc_offset:%f(V)", (float)cali_para.vcc * 0.1 / 1000.0);
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else if (!strcmp(param->argv[3],"bias_offset"))  //set|get ddmi_cali_para [ch0|ch1|ch2|ch3|all] bias_offset bias_offset_x bias_offset_y
	{
		uint8_t i = 0;

		if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[2])))
		{
			return RET_FAILED;
		}

		if (!strcmp(param->argv[0],"set"))
		{
			if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
			{
				cali_para.current[channel].bias.bias_offset1 = (int16_t)(atof(param->argv[4]) * 10.0);  //uint:0.1mA
				cali_para.current[channel].bias.bias_offset2 = (int16_t)(atof(param->argv[5]) * 10.0);  //uint:0.1mA
				ret = qsfp28_set_ddmi_cali_para(DDMI_BIAS, channel, &cali_para);
			}
			else if(channel == CHANNEL_MAX)
			{
				for(i = CHANNEL0; i < CHANNEL_MAX; i++)
				{
					cali_para.current[i].bias.bias_offset1 = (int16_t)(atof(param->argv[4]) * 10.0);  //uint:0.1mA
					cali_para.current[i].bias.bias_offset2 = (int16_t)(atof(param->argv[5]) * 10.0);  //uint:0.1mA
					ret = qsfp28_set_ddmi_cali_para(DDMI_BIAS, i, &cali_para);
					JUDGE_RETURN(ret);
				}
			}
		}
		else if (!strcmp(param->argv[0],"get"))
		{
			if(channel >= CHANNEL0 && channel < CHANNEL_MAX)
			{
				ret = qsfp28_get_ddmi_cali_para(DDMI_BIAS, channel, &cali_para); //uint:0.1mA
				JUDGE_RETURN(ret);
				uart_printf( "\r\n ch%bu-bias_offset_x:%f(mA)\r\n ch%bu-bias_offset_y:%f(mA)",  \
						channel,(float)cali_para.current[channel].bias.bias_offset1 * 0.1,		  \
						channel,(float)cali_para.current[channel].bias.bias_offset2 * 0.1);
			}
			else if(channel == CHANNEL_MAX)
			{
				for(i = CHANNEL0; i < CHANNEL_MAX; i++)
				{
					ret = qsfp28_get_ddmi_cali_para(DDMI_BIAS, i, &cali_para); //uint:0.1mA
					JUDGE_RETURN(ret);
					uart_printf( "\r\n ch%bu-bias_offset_x:%f(mA)\r\n ch%bu-bias_offset_y:%f(mA)",  \
							i,(float)cali_para.current[i].bias.bias_offset1 * 0.1,		  \
							i,(float)cali_para.current[i].bias.bias_offset2 * 0.1);
				}
			}
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else if (!strcmp(param->argv[3],"txpwr"))  //set|get ddmi_cali_para [ch0|ch1|ch2|ch3] txpwr TXPOWER BIAS
	{
		if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[2])))
		{
			return RET_FAILED;
		}

		if (!strcmp(param->argv[0],"set"))
		{
			cali_para.current[channel].txpower.txpower1 = (uint16_t)(pow(10.0, atof(param->argv[4]) / 10.0) * 1000.0 * 10.0); //uint:0.1uW
			cali_para.current[channel].txpower.bias1    = (uint16_t)(atof(param->argv[5]) * 10.0); //uint:0.1mA

			cali_para.current[channel].txpower.txpower2 = (uint16_t)(pow(10.0, atof(param->argv[6]) / 10.0) * 1000.0 * 10.0); //uint:0.1uW
			cali_para.current[channel].txpower.bias2    = (uint16_t)(atof(param->argv[7]) * 10.0); //uint:0.1mA
			ret = qsfp28_set_ddmi_cali_para(DDMI_TXMON, channel, &cali_para);
		}
		else if (!strcmp(param->argv[0],"get"))
		{
			float txpower1 = 0.0, txpower2 = 0.0;

			ret = qsfp28_get_ddmi_cali_para(DDMI_TXMON, channel, &cali_para);
			JUDGE_RETURN(ret);
			txpower1 = (float)cali_para.current[channel].txpower.txpower1  * 0.1 /1000.0; //uint:mW
			txpower2 = (float)cali_para.current[channel].txpower.txpower2  * 0.1 /1000.0; //uint:mW

			uart_printf( "\r\nch%bu-txpower1:%f(dbm)\r\nch%bu-bias1   :%f(mA)\r\nch%bu-txpower2:%f(dbm)\r\nch%bu-bias2  :%f(mA)", \
					channel, (float)(10 * log10(txpower1)), 	   						\
					channel, (float)cali_para.current[channel].txpower.bias1 * 0.1 , 	\
					channel, (float)(10 * log10(txpower2)),        						\
					channel, (float)cali_para.current[channel].txpower.bias2 * 0.1);
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else if (!strcmp(param->argv[3],"rxpwr"))  //set|get ddmi_cali_para [ch0|ch1|ch2|ch3] rxpwr RXPOWER1 RSSI1 RXPOWER2 RSSI2
	{
		if(RET_FAILED == (channel = _qsfp28_get_channel(param->argv[2])))
		{
			return RET_FAILED;
		}

		if (!strcmp(param->argv[0],"set"))
		{
			if(CHECK_DDMI_CALI_POWER_OFFSET(atof(param->argv[4])) && //CHECK_DDMI_CALI_RSSI_OFFSET(com_str_to_int16(param->argv[5])) &&&& CHECK_DDMI_CALI_RSSI_OFFSET(com_str_to_int16(param->argv[7]))
			   CHECK_DDMI_CALI_POWER_OFFSET(atof(param->argv[6])) )
			{
				cali_para.current[channel].rxpower.rxpower1 = (uint16_t)(pow(10.0, atof(param->argv[4]) / 10.0) * 1000.0 * 10.0); //uint:0.1uW
				cali_para.current[channel].rxpower.rssi1    = (uint16_t)com_str_to_int16(param->argv[5]);
				cali_para.current[channel].rxpower.rxpower2 = (uint16_t)(pow(10.0, atof(param->argv[6])/ 10.0) * 1000.0 * 10.0); //uint:0.1uW
				cali_para.current[channel].rxpower.rssi2    = (uint16_t)com_str_to_int16(param->argv[7]);

				ret = qsfp28_set_ddmi_cali_para(DDMI_RSSI, channel, &cali_para);
			}
			else
			{
				uart_printf( "\r\nRxpower or rssi is out of range");
				ret = RET_FAILED;
			}
		}
		else if (!strcmp(param->argv[0],"get"))
		{
			float rxpower1 = 0.0, rxpower2 = 0.0;

			ret = qsfp28_get_ddmi_cali_para(DDMI_RSSI, channel, &cali_para);
			JUDGE_RETURN(ret);

			rxpower1 = (float)cali_para.current[channel].rxpower.rxpower1  * 0.1 /1000.0; //uint:mW
			rxpower2 = (float)cali_para.current[channel].rxpower.rxpower2  * 0.1 /1000.0; //uint:mW
			uart_printf( "\r\nch%bu-rxpower1:%f(dbm)\r\nch%bu-rssi1   :%hu(uA)\r\nch%bu-rxpower2:%f(dbm)\r\nch%bu-rssi2   :%hu(uA)", \
					channel, (float)(10 * log10(rxpower1)), 	   						\
					channel, cali_para.current[channel].rxpower.rssi1 , 	\
					channel, (float)(10 * log10(rxpower2)),        					\
					channel, cali_para.current[channel].rxpower.rssi2);
		}
		else
		{
			ret = RET_FAILED;
		}
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static uint8_t _cli_ddmi_cmd(PARAM_INFO_T  *param)
{
	uint8_t channel = 0, ret = RET_OK;

	 if(!strcmp(param->argv[2],"vcc"))
	 {
			 ret = qsfp28_get_ddmi(DDMI_VCC, 0, DDMI_CALI_BEFORE);  //get mcu vcc
	 }
	 else if(!strcmp(param->argv[2],"temp"))
	 {
			 ret = qsfp28_get_ddmi(DDMI_TEMP, 0, DDMI_CALI_BEFORE);  //get mcu temp
	 }
	 else
	 {
		 channel = (uint8_t)com_str_to_int16(param->argv[3]);

		 if(!strcmp(param->argv[2],"bias"))
		 {
			 ret = qsfp28_get_ddmi(DDMI_BIAS, channel, DDMI_CALI_BEFORE);
		 }
		 else if(!strcmp(param->argv[2],"txmon"))
		 {
			 ret = qsfp28_get_ddmi(DDMI_TXMON, channel, DDMI_CALI_BEFORE);
		 }
		 else if(!strcmp(param->argv[2],"rssi"))
		 {
			 ret =  qsfp28_get_ddmi(DDMI_RSSI, channel, DDMI_CALI_BEFORE);
		 }
		 else
		 {
			 ret = RET_FAILED;
		 }
	 }

	 return ret;
}


static uint8_t _cli_thrhld_cmd(PARAM_INFO_T  *param)
{
	uint8_t value = 0, ret = RET_OK;
	uint16_t value16 = 0;
	float    tmp_f = 1.0;
	MSA_PAGE3_ALARM_WARING_THRESHOLD_T type = UNKNOW_ALARM;

	tmp_f = atof(param->argv[3]);

	if(!strcmp(param->argv[2],"all"))
	{
		type = HTRHLD_ALL;
	}
	else if(!strcmp(param->argv[2],"temp_high_alarm"))  //temp
	{
		type = TEMP_HIGH_ALARM;
		value16 = (uint16_t)(tmp_f * (float)DDMI_CALI_TEMP_SCALE);
	}
	else if(!strcmp(param->argv[2],"temp_low_alarm"))
	{
		type = TEMP_LOW_ALARM;
		value16 = (uint16_t)(tmp_f * (float)DDMI_CALI_TEMP_SCALE);
	}
	else if(!strcmp(param->argv[2],"temp_high_warn"))
	{
		type = TEMP_HIGH_WARN;
		value16 = (uint16_t)(tmp_f * (float)DDMI_CALI_TEMP_SCALE);
	}
	else if(!strcmp(param->argv[2],"temp_low_warn"))
	{
		type = TEMP_LOW_WARN;
		value16 = (uint16_t)(tmp_f * (float)DDMI_CALI_TEMP_SCALE);
	}
	else if(!strcmp(param->argv[2],"vcc_high_alarm"))  // vcc
	{
		type = VCC_HIGH_ALARM;
		value16 = (uint16_t)(tmp_f * 1000.0 * 10.0);
	}
	else if(!strcmp(param->argv[2],"vcc_low_alarm"))
	{
		type = VCC_LOW_ALARM;
		value16 = (uint16_t)(tmp_f * 1000.0 * 10.0);
	}
	else if(!strcmp(param->argv[2],"vcc_high_warn"))
	{
		type = VCC_HIGH_WARN;
		value16 = (uint16_t)(tmp_f * 1000.0 * 10.0);
	}
	else if(!strcmp(param->argv[2],"vcc_low_warn"))
	{
		type = VCC_LOW_WARN;
		value16 = (uint16_t)(tmp_f * 1000.0 * 10.0);
	}
	else if(!strcmp(param->argv[2],"rxpwr_high_alarm"))  // rxpower
	{
		type = RXPWR_HIGH_ALARM;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else if(!strcmp(param->argv[2],"rxpwr_low_alarm"))
	{
		type = RXPWR_LOW_ALARM;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else if(!strcmp(param->argv[2],"rxpwr_high_warn"))
	{
		type = RXPWR_HIGH_WARN;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else if(!strcmp(param->argv[2],"rxpwr_low_warn"))
	{
		type =RXPWR_LOW_WARN;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else if(!strcmp(param->argv[2],"bias_high_alarm"))  // bias
	{
		type = TXBIAS_HIGH_ALARM;
		value16 = (uint16_t)(tmp_f * 1000.0 / 2.0);
	}
	else if(!strcmp(param->argv[2],"bias_low_alarm"))
	{
		type = TXBIAS_LOW_ALARM;
		value16 = (uint16_t)(tmp_f * 1000.0 / 2.0);
	}
	else if(!strcmp(param->argv[2],"bias_high_warn"))
	{
		type = TXBIAS_HIGH_WARN;
		value16 = (uint16_t)(tmp_f * 1000.0 / 2.0);
	}
	else if(!strcmp(param->argv[2],"bias_low_warn"))
	{
		type = TXBIAS_LOW_WARN;
		value16 = (uint16_t)(tmp_f * 1000.0 / 2.0);
	}
	else if(!strcmp(param->argv[2],"txpwr_high_alarm"))  // txpower
	{
		type = TXPWR_HIGH_ALARM;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else if(!strcmp(param->argv[2],"txpwr_low_alarm"))
	{
		type = TXPWR_LOW_ALARM;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else if(!strcmp(param->argv[2],"txpwr_high_warn"))
	{
		type = TXPWR_HIGH_WARN;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else if(!strcmp(param->argv[2],"txpwr_low_warn"))
	{
		type = TXPWR_LOW_WARN;
		value16 = (uint16_t)(pow(10.0, tmp_f/10.0) * 10000.0);
	}
	else
	{
		ret = RET_FAILED;
	}

	if(!strcmp(param->argv[0],"set"))
	{
	   if(value >= TEMP_HIGH_ALARM && value < HTRHLD_ALL)
		{
			ret = qsfp28_set_thrhld_value(type,value16);
		}
	}
	else if(!strcmp(param->argv[0],"get"))
	{
		ret = qsfp28_get_thrhld_value(type);
	}

	return ret;
}

static uint8_t _cli_module_class_cmd(PARAM_INFO_T  *param)
{
	uint8_t ret = RET_OK;

	if(!strcmp((char*)param->argv[0],"get"))
	{
		ret = qsfp28_get_module_class();
	}
	else if(!strcmp((char*)param->argv[0],"set"))
	{
		uint8_t value = (uint8_t)com_str_to_int16(param->argv[2]);

		ret = qsfp28_set_module_class(value);
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static uint8_t  _cli_set_iic_cmd(PARAM_INFO_T  *param)
{
	uint8_t ret = RET_OK;

	if(!strcmp(param->argv[0],"get"))
	{
		uart_printf("\r\n i2c Clock Speed : %hu kHz", i2c_master_get_freq());
	}
	else if(!strcmp(param->argv[0],"set"))
	{
		uint16_t Clock = com_str_to_int16(param->argv[2]);

		if(Clock > 30)
		{
			i2c_master_set_freq(Clock);
		}
		else
		{
			uart_printf("%s","\r\n Minimum frequency 30kHz ");
		}
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static uint8_t  _cli_get_mod_boot_reason_cmd(PARAM_INFO_T  *param)
{
	uint8_t ret = RET_OK;
	uint8_t value = 0;

	if(!strcmp(param->argv[0],"get"))
	{
    	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
    	JUDGE_RETURN(ret);
		ret = i2c_master_read_byte(I2C_DEVICE_ADDR, SFP_MOD_BOOT_REASON, &value);
		if(value == 0x03)
		{
			uart_printf("%s","\r\n Module state is normal");
		}
		else
		{
			uart_printf("%s","\r\n Module Watch Dog reboot");
		}
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

static void _rssi_poll_timer_handle(void)
{
	uart_put_string("\r\n111");
}

static void _intl_get_ddmi_data_timer_handle()
{
	uart_put_string("\r\n222");
}

static void _prbs_code_timer_handle(void)  //1000ms Ö´ÐÐÒ»´Î
{
	uart_put_string("\r\n333");
}

static RETURN_VALUE_TYPE prvDispConfigRssiConfig(PARAM_INFO_T  *param)
{
	uint8_t ret = RET_OK;

	if(!strcmp(param->argv[1],"show"))
	{
		int16_t num = (int16_t)com_str_to_int16(param->argv[2]);

		if(num > 0 && num <= 30000)
		{
			rssi_poll_timer.timeout = num;
		}
		else
		{
			rssi_poll_timer.timeout = 500;
		}

		timer_start(&rssi_poll_timer);
	}
	else if(!strcmp(param->argv[1],"stop"))
	{
		timer_stop(&rssi_poll_timer);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispAutoAbleConfig(PARAM_INFO_T  *param)//auto enable|disable
{
	if(!strcmp(param->argv[1],"enable"))
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_AUTO_ENABLE_CMD);
	}
	else
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_AUTO_DISABLE_CMD);
	}
}

static RETURN_VALUE_TYPE prvDispDdmiAbleConfig(PARAM_INFO_T  *param) //ddmi enable|disable
{
	if(!strcmp(param->argv[1],"enable"))
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_DDMI_ENABLE_CMD);
	}
	else
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_DDMI_DISABLE_CMD);
	}
}

static RETURN_VALUE_TYPE prvDispTxcdrRxcdrI2cEnableConfig(PARAM_INFO_T  *param) //[txcdr|rxcdr] i2c enable
{
	if(!strcmp(param->argv[0],"txcdr"))
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_I2C_TXCDR_ENABLE);
	}
	else if(!strcmp(param->argv[0],"rxcdr"))
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_I2C_RXCDR_ENABLE);
	}
	else
	{
		return RET_FAILED;
	}
}

static RETURN_VALUE_TYPE prvDispUpgdConfig() //upgd
{
	return qsfp28_msa_page4_dbg_cmd(DBG_JUMP_TO_BOOT_CMD);
}

static RETURN_VALUE_TYPE prvDispAgingConfig(PARAM_INFO_T  *param) //aging [start|stop|clean]
{
	if(!strcmp(param->argv[1],"start"))
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_AGING_START_CMD);
	}
	else if(!strcmp(param->argv[1],"stop"))
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_AGING_STOP_CMD);
	}
	else// if(!strcmp(param->argv[1],"clear") )
	{
		return qsfp28_msa_page4_dbg_cmd(DBG_AGING_EEP_INIT_CMD);
	}
}

static RETURN_VALUE_TYPE prvIntlOutputConfig(PARAM_INFO_T  *param) //intl output [enable|disable]
{
	uint8_t ret = RET_OK;

	if(!strcmp(param->argv[2],"enable"))
	{
		timer_start(&intl_trigger_timer);
		port_match_all_interrupt_enable(1);
	}
	else if(!strcmp(param->argv[2],"disable"))
	{
		timer_stop(&intl_trigger_timer);
		port_match_all_interrupt_enable(0);
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

static RETURN_VALUE_TYPE prvDispLutConfig(PARAM_INFO_T  *param) //[get|set] [ch0|ch1|ch2|ch3|all] [lt|rt|ht|lut] #temp #bias #mod #cross
{
	if (!strcmp(param->argv[2],"lt"))  //set|get [ch1|ch2|ch3|ch4|all] lt temp bias mod cross
	{
		return _cli_set_lut_tab(PART_LUT_INFO, MSA_LUT_TYPE_LOW_TEMP, param);
	}
	else if (!strcmp(param->argv[2],"rt"))  //set|get [ch1|ch2|ch3|ch4|all] rt temp bias mod cross
	{
		return _cli_set_lut_tab(PART_LUT_INFO, MSA_LUT_TYPE_MID_TEMP, param);
	}
	else if (!strcmp(param->argv[2],"ht"))  //set|get [ch1|ch2|ch3|ch4|all] ht temp bias mod cross
	{
		return _cli_set_lut_tab(PART_LUT_INFO, MSA_LUT_TYPE_HI_TEMP, param);
	}
	else if (!strcmp(param->argv[2],"lut"))  //get all lut
	{
		return  _cli_get_all_lut_tab();
	}
	else
	{
		return  RET_FAILED;
	}
}

uint8_t M37049_tx_cdr_prbs_cmd(PARAM_INFO_T  *param) //"set [ch0|ch1|ch2|ch3|all] tx_prbs [enable|disable]"
{
	uint8_t ret = RET_OK, channel = 0;
	uint8_t ch[] = {CHANNEL2,CHANNEL1,CHANNEL3,CHANNEL0,CHANNEL_MAX};

	ret = ((RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1]))) ? RET_FAILED : RET_OK);

	channel = ch[channel];

	if (!strcmp(param->argv[3],"enable"))
	{
		//ret = M37049_txcdr_prbs_enable(channel);
	}
	else if(!strcmp(param->argv[3],"disable"))
	{
		//ret = M37049_txcdr_prbs_disable(channel);
	}
	return ret;
}

uint8_t M37046_rx_cdr_prbs_pn_normal_cmd(PARAM_INFO_T  *param) //"set [ch0|ch1|ch2|ch3|all] rx_prbs check [enable|disable] #value(ms)"
{
	uint8_t ret = RET_OK, channel = 0;
	uint8_t ch[] = {CHANNEL1,CHANNEL2,CHANNEL0,CHANNEL3,CHANNEL_MAX};

	ret = ((RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1]))) ? RET_FAILED : RET_OK);

	channel = ch[channel];

	if (!strcmp(param->argv[4],"enable"))
	{
		int16_t num = (int16_t)com_str_to_int16(param->argv[5]);

		if(num > 0 && num <= 30000)
		{
			prbs_code_timer.timeout = num;
		}
		else
		{
			prbs_code_timer.timeout = 1000;
		}

		timer_start(&prbs_code_timer);
		//return M37046_rxcdr_prbs_enable(channel, PN_NORMAL);
	}
	else if(!strcmp(param->argv[4],"disable"))
	{
		timer_stop(&prbs_code_timer);
		//return M37046_rxcdr_prbs_disable(channel);
	}
	return RET_FAILED;
}

uint8_t M37046_rx_cdr_prbs_pn_inverse_cmd(PARAM_INFO_T  *param) //"set [ch0|ch1|ch2|ch3|all] rx_prbs check inverse [enable|disable] #value(ms)"
{
	uint8_t ret = RET_OK, channel = 0;
	uint8_t ch[] = {CHANNEL1,CHANNEL2,CHANNEL0,CHANNEL3,CHANNEL_MAX};

	ret = ((RET_FAILED == (channel = _qsfp28_get_channel(param->argv[1]))) ? RET_FAILED : RET_OK);

	channel = ch[channel];

	if (!strcmp(param->argv[5],"enable"))
	{
		int16_t num = (int16_t)com_str_to_int16(param->argv[6]);

		if(num > 0 && num <= 30000)
		{
			prbs_code_timer.timeout = num;
		}
		else
		{
			prbs_code_timer.timeout = 1000;
		}

		timer_start(&prbs_code_timer);
		//ret = M37046_rxcdr_prbs_enable(channel, PN_INVERSE);
	}
	else if(!strcmp(param->argv[5],"disable"))
	{
		timer_stop(&prbs_code_timer);
		//ret = M37046_rxcdr_prbs_disable(channel);
	}
	return ret;
}

static RETURN_VALUE_TYPE prvDispshowReg(PARAM_INFO_T  *param) //ddmi enable|disable
{
	param = NULL;
	reg_add_show("P0", P0, 0x00);
	reg_add_show("P1", P1, 0x00);
	reg_add_show("P2", P2, 0x00);
	reg_add_show("P3", P3, 0x00);

	reg_add_show("P0MDOUT", P0MDOUT, 0x20);
	reg_add_show("P1MDOUT", P1MDOUT, 0x20);
	reg_add_show("P2MDOUT", P2MDOUT, 0x20);
	reg_add_show("P3MDOUT", P3MDOUT, 0x20);

	reg_add_show("P0MDIN", P0MDIN,  0x20);
	reg_add_show("P1MDIN", P1MDIN,  0x20);
	reg_add_show("P2MDIN", P2MDIN,  0x20);
	reg_add_show("P3MDIN", P3MDIN,  0x20);

	return RET_OK;
}

static RETURN_VALUE_TYPE prvDispReadWriteReg(PARAM_INFO_T  *param) //ddmi enable|disable
{
	int16_t num = 0;

	if (!strcmp(param->argv[0],"read"))  //read reg P0
	{
		if (!strcmp(param->argv[1],"reg"))
		{
			printf("\r\n%s\t = 0x%bx", param->argv[2], reg_read(param->argv[2]));
		}
	}
	else if (!strcmp(param->argv[0],"write"))  //write reg  P0 0x33
	{
		if (!strcmp(param->argv[1],"reg"))
		{
			num = (int16_t)com_str_to_int16(param->argv[3]);
			reg_write(param->argv[2], (MCU_BIT_WIDTH_T)num);
		}
	}


	return RET_OK;
}
void cmd_init()
{
	prbs_code_timer.function = _rssi_poll_timer_handle;
	prbs_code_timer.timeout  = 1000;

	timer_add(&prbs_code_timer);

	rssi_poll_timer.function = _prbs_code_timer_handle;
	rssi_poll_timer.timeout  = 2000;
	timer_add(&rssi_poll_timer);

	intl_trigger_timer.function = _intl_get_ddmi_data_timer_handle;
	intl_trigger_timer.timeout  = 3000;
	timer_add(&intl_trigger_timer);

	terminal_register_cmd("build info"	\
				  ,prvDispVersionConfig);
	terminal_register_cmd("reset"    \
				  ,prvDispResetConfig);
	terminal_register_cmd("reboot"    \
				  ,prvDispRebootConfig);
	terminal_register_cmd("factory_reset" \
				  ,prvDispFactoryResetConfig);
	terminal_register_cmd("get swver" \
				  ,prvDispSwverConfig);
	terminal_register_cmd("[opr|opw] &baseAddr &OffAddr #ParmNum(0~0xff)" \
					  ,prvDispOprwConfig);
	terminal_register_cmd("[mdior|mdiow] &baseAddr &OffAddr #ParmNum(0~0xff)" \
						  ,prvDispMdiorwConfig);
	terminal_register_cmd("get intl" \
				  ,prvDispInitConfig);
	terminal_register_cmd("get mod_status" \
				  ,prvDispModStatusConfig);
	terminal_register_cmd("enter bonding" \
				  ,prvDispBondingConfig);
	terminal_register_cmd("eep   [init|show|erase] " \
				  ,prvDispEepConfig);
	terminal_register_cmd("auto [enable|disable]" \
				  ,prvDispAutoAbleConfig);
	terminal_register_cmd("ddmi [enable|disable]" \
				  ,prvDispDdmiAbleConfig);
	terminal_register_cmd("aging [start|stop|clean]" \
				  ,prvDispAgingConfig);
	terminal_register_cmd("intl output [enable|disable]" \
				  ,prvIntlOutputConfig);
	terminal_register_cmd("test modsel #[0|1]" \
				  ,prvDispTestModselConfig);
	terminal_register_cmd("test modsel aborted" \
				  ,prvDispTestModselConfig);
	terminal_register_cmd("[get|set] lpmode #[0|1]" \
				  ,prvDispLpmodeConfig);
	terminal_register_cmd("[get|set] reset  #[0|1]" \
				  ,prvDispgsetResetConfig);
	terminal_register_cmd("[get|set] reset_l #[0|1]" \
				  ,prvDispgsetResetLevelConfig);
	terminal_register_cmd("[get|set] modsel #[0|1]" \
				  ,prvDispModselConfig);
	terminal_register_cmd("enter msa download" \
				  ,prvDispMsaDownloadConfig);
	terminal_register_cmd("rssi [show|stop] #value(ms)" \
				  ,prvDispConfigRssiConfig);
	terminal_register_cmd("mod_console [enable|disable]" \
				  ,prvDispConfigModConfig);
	terminal_register_cmd("[txcdr|rxcdr] i2c enable" \
				  ,prvDispTxcdrRxcdrI2cEnableConfig);
	terminal_register_cmd("[read|enable|disable|clear] log"    \
				  ,prvDispLogConfig);
	terminal_register_cmd("eep regset &devaddr &offsetaddr &value" \
				  ,prvDispEepConfig);
	terminal_register_cmd("[oprt|opwt] &baseAddr &OffAddr #ParmNum(0~0xff)" \
				  ,prvDispOprwConfig);
	terminal_register_cmd("[get|set] laser_tx_sn #" \
				  ,_cli_laser_tx_sn_cmd);
	terminal_register_cmd("[get|set] laser_rx_sn #" \
				  ,_cli_laser_rx_sn_cmd);
	terminal_register_cmd("[get|set] pic_code #" \
				  ,_cli_pic_code_cmd);
	terminal_register_cmd("[get|set] module_type #[0x101|0x206|0x211|0x301]" \
				  ,_cli_module_type_cmd);
	terminal_register_cmd("[get|set] hw_ver #value(AM)" \
				  ,_cli_hw_ver_cmd);
	terminal_register_cmd("[get|set] vendor_pn  #char[16]" \
				  ,prvDispVendorPnConfig);
	terminal_register_cmd("[get|set] vendor_rev #char[2]"  \
				  ,prvDispVendorRevConfig);
	terminal_register_cmd("[get|set] vendor_sn  #char[16]" \
				  ,prvDispVendorSnConfig);
	terminal_register_cmd("[get|set] date_code  #char[8]"  \
				  ,prvDispDateCodeConfig);
	terminal_register_cmd("[get|set] supercode  #char[9]"  \
				  ,prvDispSuperCodeConfig);
	terminal_register_cmd("[get|set] pcba_sn    #char[16]" \
				  ,prvDispPcbaSnConfig);
	terminal_register_cmd("[get|set] pcba_code  #char[16]" \
				  ,prvDispPcbaCodeConfig);
	terminal_register_cmd("get module [vcc|temp|bias|txpower|rxpower] #channel" \
				  ,prvDispModuleConfig);
	terminal_register_cmd("[get|set] [ch0|ch1|ch2|ch3|all] [lt|rt|ht|lut] #temp #bias #mod #cross" \
				  ,prvDispLutConfig);
	terminal_register_cmd("[get|set] [ch0|ch1|ch2|ch3] #[cur|fixed_temp] lut tab" \
				  ,prvDispLuttabConfig);
	terminal_register_cmd("[set|get] ddmi_cali [temp_offset|vcc_offset] #value" \
				  ,_cli_ddmi_cali);
	terminal_register_cmd("[set|get] ddmi_cali [ch0|ch1|ch2|ch3|all] bias_offset #BIAS_X #BIAS_Y" \
				  ,_cli_ddmi_cali);
	terminal_register_cmd("[set|get] ddmi_cali [ch0|ch1|ch2|ch3] txpwr #TXPWR1 #BIAS1 #TXPWR2 #BIAS2" \
				  ,_cli_ddmi_cali);
	terminal_register_cmd("[set|get] ddmi_cali [ch0|ch1|ch2|ch3] rxpwr #RXPWR1 #RSSI1 #RXPWR2 #RSSI2" \
				  ,_cli_ddmi_cali);
	terminal_register_cmd("get ddmi [vcc|temp]" \
				  ,_cli_ddmi_cmd);
	terminal_register_cmd("get ddmi [bias|txmon|rssi] [0|1|2|3]" \
				  ,_cli_ddmi_cmd);
	terminal_register_cmd("[set|get] thrhld [temp_high_alarm|temp_high_warn|temp_low_alarm|temp_low_warn] #value" \
				  ,_cli_thrhld_cmd);
	terminal_register_cmd("[set|get] thrhld [vcc_high_alarm|vcc_high_warn|vcc_low_alarm|vcc_low_warn]     #value" \
				  ,_cli_thrhld_cmd);
	terminal_register_cmd("[set|get] thrhld [bias_high_alarm|bias_high_warn|bias_low_alarm|bias_low_warn] #value" \
				  ,_cli_thrhld_cmd);
	terminal_register_cmd("[set|get] thrhld [rxpwr_high_alarm|rxpwr_high_warn|rxpwr_low_alarm|rxpwr_low_warn] #value" \
			      ,_cli_thrhld_cmd);
	terminal_register_cmd("[set|get] thrhld [txpwr_high_alarm|txpwr_high_warn|txpwr_low_alarm|txpwr_low_warn] #value" \
		    	  ,_cli_thrhld_cmd);
	terminal_register_cmd("[get|set] module_class #value" \
				  ,_cli_module_class_cmd);
	terminal_register_cmd("[set|get] iic_speed #value(kHz)" \
				  ,_cli_set_iic_cmd);
	terminal_register_cmd("get mod_boot_reason" \
				  ,_cli_get_mod_boot_reason_cmd);
	terminal_register_cmd("set [ch0|ch1|ch2|ch3|all] tx_prbs [enable|disable]" \
				  ,M37049_tx_cdr_prbs_cmd);
	terminal_register_cmd("set [ch0|ch1|ch2|ch3|all] rx_prbs check [enable|disable] #value(ms)" \
				  ,M37046_rx_cdr_prbs_pn_normal_cmd);
	terminal_register_cmd("set [ch0|ch1|ch2|ch3|all] rx_prbs invert check [enable|disable] #value(ms)" \
				  ,M37046_rx_cdr_prbs_pn_inverse_cmd);
	terminal_register_cmd("set [ch0|ch1|ch2|ch3|all] tx [enable|disable]" \
				  ,_cli_tx_laser_cmd);
	terminal_register_cmd("get [ch0|ch1|ch2|ch3|all] tx" \
				  ,_cli_tx_laser_cmd);
	terminal_register_cmd("[get|set] [ch0|ch1|ch2|ch3|all] darka_thld #value(uA)" \
				  ,_cli_rssi_darka_cmd);
	terminal_register_cmd("[get|set] [ch0|ch1|ch2|ch3|all] darkd_thld #value(uA)" \
				  ,_cli_rssi_darkd_cmd);
	terminal_register_cmd("[get|set] [ch0|ch1|ch2|ch3] losa_thld #value(uA)" \
				  ,_cli_rx_losa_cmd);
	terminal_register_cmd("[get|set] [ch0|ch1|ch2|ch3] losd_thld #value(uA)" \
				  ,_cli_rx_losd_cmd);
	terminal_register_cmd("show mcu reg" \
				  ,prvDispshowReg);
	terminal_register_cmd("[read|write] reg &reg_name #value" \
				  ,prvDispReadWriteReg);
}








