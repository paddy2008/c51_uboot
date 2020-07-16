#include "Efm8lb12_qsfp28.h"
#include "Efm8lb12_cmd.h"
#include "mcu_sal.h"


static uint8_t _mod_type 	 = INVALID;
static uint8_t _mod_sub_type = INVALID;
static uint8_t _mod_hw_rev   = INVALID;

static POLL_T poll = {0};

static MSA_PAGE_LOWER_MON_T _lower_mon = {0};

const uint8_t lut_page_select[MSA_LUT_TYPE_MAX] = {SFP_PAGE_06H, SFP_PAGE_06H, SFP_PAGE_07H};
const uint8_t lut_temp_select[MSA_LUT_TYPE_MAX] = {LUT_LT_TEMP_MSB, LUT_MT_TEMP_MSB, LUT_HT_TEMP_MSB};

uint8_t smb_write_page_byte(uint8_t BaseAddr, uint8_t page, uint16_t OffsetAddr,uint8_t *Data, uint16_t Length)
{
	uint8_t ret = RET_FAILED;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, page);
	JUDGE_RETURN(ret);
	ret = i2c_master_write(BaseAddr, OffsetAddr, Data, Length);
	return ret;
}

uint8_t smb_read_page_byte(uint8_t BaseAddr, uint8_t page, uint16_t OffsetAddr,uint8_t *Data, uint16_t Length)
{
	uint8_t ret = RET_FAILED;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, page);
	JUDGE_RETURN(ret);
	ret = i2c_master_read(BaseAddr, OffsetAddr, Data, Length);
	return ret;
}


const uint8_t lut_laser_cfg_select[MSA_LUT_TYPE_MAX][CHANNEL_MAX] =
{
	{PAGE6_LT_CH0_LASER_CFG_BASE_ADDR, PAGE6_LT_CH1_LASER_CFG_BASE_ADDR, PAGE6_LT_CH2_LASER_CFG_BASE_ADDR, PAGE6_LT_CH3_LASER_CFG_BASE_ADDR},
	{PAGE6_MT_CH0_LASER_CFG_BASE_ADDR, PAGE6_MT_CH1_LASER_CFG_BASE_ADDR, PAGE6_MT_CH2_LASER_CFG_BASE_ADDR, PAGE6_MT_CH3_LASER_CFG_BASE_ADDR},
	{PAGE7_HT_CH0_LASER_CFG_BASE_ADDR, PAGE7_HT_CH1_LASER_CFG_BASE_ADDR, PAGE7_HT_CH2_LASER_CFG_BASE_ADDR, PAGE7_HT_CH3_LASER_CFG_BASE_ADDR}
};

const uint8_t page3_alarm_wran_select[] =
{
	PAGE3_TEMP_HIGH_ALARM_MSB  , PAGE3_TEMP_LOW_ALARM_MSB  , PAGE3_TEMP_HIGH_WARN_MSB  , PAGE3_TEMP_LOW_WARN_MSB ,
	PAGE3_VCC_HIGH_ALARM_MSB   , PAGE3_VCC_LOW_ALARM_MSB   , PAGE3_VCC_HIGH_WARN_MSB   , PAGE3_VCC_LOW_WARN_MSB   ,
	PAGE3_RXPWR_HIGH_ALARM_MSB , PAGE3_RXPWR_LOW_ALARM_MSB , PAGE3_RXPWR_HIGH_WARN_MSB , PAGE3_RXPWR_LOW_WARN_MSB ,
	PAGE3_TXBIAS_HIGH_ALARM_MSB, PAGE3_TXBIAS_LOW_ALARM_MSB, PAGE3_TXBIAS_HIGH_WARN_MSB, PAGE3_TXBIAS_LOW_WARN_MSB,
	PAGE3_TXPWR_HIGH_ALARM_MSB , PAGE3_TXPWR_LOW_ALARM_MSB , PAGE3_TXPWR_HIGH_WARN_MSB , PAGE3_TXPWR_LOW_WARN_MSB
};

uint8_t _qsfp28_get_channel(const char  *s)
{
	uint8_t channel = -1;

	if (!strcmp(s, "ch0"))
	{
		channel = CHANNEL0;
	}
	else if (!strcmp(s, "ch1"))
	{
		channel = CHANNEL1;
	}
	else if (!strcmp(s, "ch2"))
	{
		channel = CHANNEL2;
	}
	else if (!strcmp(s, "ch3"))
	{
		channel = CHANNEL3;
	}
	else if (!strcmp(s, "all"))
	{
		channel = CHANNEL_MAX;
	}
	else
	{
		channel = RET_FAILED;
		uart_printf("\r\nchannedl%bu is not support\r\n",channel);
	}
	return channel;
}

static uint16_t _com_int_pow(uint8_t m, uint8_t n)
{
	uint8_t i = 0;
	uint16_t result = 1;

	for ( i = 0; i < n; i++)
	{
		result *= m;
	}
	return result;
}

int16_t com_str_to_int16(uint8_t *str)
{
	idata uint8_t  *ptr = str, i = 0, len = 0, tmp[5];
	idata int16_t  temp = 0;
	bool IsHex = false;

	memset(tmp,'\0',5);

	if ((strchr(str,'x') != NULL) || (strchr(str,'X') != NULL))
	{
		IsHex = true;
		ptr += 2;
		for (i = 0; i < 4 ; i++)
		{
			tmp[i] = *ptr;
			ptr++;
		}
		ptr = tmp;
	}
	else
	{
		ptr = str;
	}

	if (IsHex)
	{
		len = strlen(ptr);
		for (i = 0; i < len; i++)
		{
			if (*ptr >= '0' && *ptr <= '9')
			{
				temp += (*ptr-'0') * _com_int_pow(16,len-1-i);
			}
			else if (*ptr >= 'a' && *ptr <= 'f')
			{
				temp += (*ptr-'a'+ 10) * _com_int_pow(16,len-1-i);
			}
			else if (*ptr >= 'A' && *ptr <= 'F')
			{
				temp += (*ptr-'A' + 10) * _com_int_pow(16,len-1-i);
			}
			ptr++;
		}
	}
	else
	{
		temp = (int16_t)atoi(ptr);
	}

	return temp;
}

void com_delay(uint16_t sec)
{
	timer_delay_ms(sec);
}

void com_delay_us(uint8_t us)   //excute one time is 72 clock cycles
{
	uint8_t i = 0;

	for(i = 0; i < us; i++)
	{
		_nop_();

		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();

		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();

		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();

		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();

		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();


		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}

bool com_get_module_boot_status()
{
	uint8_t value = 1, i = 0;

	for(i = 0; i < 20; i++)
	{
		value = 1;
		if(RET_OK == i2c_master_read_byte(I2C_DEVICE_ADDR, MSA_BOOT_STATUS, &value))
		{
			if(MY_GET_BIT(value, 0) != 0x0) //equal to 0 indicate the module boot success
			{
				break;
			}
		}
	}

	if(i == 20)
	{
		return true;
	}

	return false;
}

bool com_test_board_pre_handle()
{
	if(com_get_module_boot_status())
	{
		uart_printf("%s","\r\nModule boot success");
		uart_printf("%s","\r\n#McuTestBoard:");
		return true;
	}
	else
	{
		uart_printf("%s","\r\nModule boot error");
		return false;
	}

}

uint16_t com_average_value16(uint16_t *buffer, uint8_t length)
{
	uint8_t  i  = 0;
	uint16_t max = 0, min = 0;
	uint32_t sum = 0;

	max = buffer[0];
	min = buffer[0];

	for(i = 0; i < length; i++)
	{
		if(buffer[i] > max)
		{
			max = buffer[i];
		}

		if(buffer[i] < min)
		{
			min = buffer[i];
		}
		sum += buffer[i];
	}

	sum = sum - min - max;

	return (uint16_t)(sum / (length -2));
}




bool qsfp28_get_cmd_status()
{
	uint8_t  cmd_ret_status = 0;
	uint16_t counter = 0;

	do
	{
		i2c_master_read_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, &cmd_ret_status);
		counter ++;
		timer_delay_ms(2 * counter);
		if(counter >= 100)  //delay 10s
		{
			uart_printf("\r\nQsfp28_get_cmd_status failed");
			return false;
		}

	}while(cmd_ret_status != RET_OK && cmd_ret_status != RET_FAILED);

	i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, SFP_DBG_CMD_IDLE);  //set dbg_cmd_status idle

	if(cmd_ret_status == RET_FAILED)
	{
		return false;
	}
	else
	{
		return true;
	}
}

static char * qsfp28_get_mod_name(char *name)
{
	switch(_mod_type) {
		case MODULE_TYPE_SR4:
			sprintf(name, "SR4-%bx%02bx", _mod_type, _mod_sub_type);
			break;
		case MODULE_TYPE_CWDM4:
			sprintf(name, "CWDM4-%bx%02bx", _mod_type, _mod_sub_type);
			break;
		case MODULE_TYPE_LR4:
			sprintf(name, "LR4-%bx%02bx", _mod_type, _mod_sub_type);
			break;
		default:
			sprintf(name, "UNKNOWN");
			break;
	}
	return name;
}

uint8_t qsfp28_common_rssi_show()
{
	uint16_t rssi_ua[4];

	uint8_t rssi_addr[] = {0x86, 0x92, 0x9E, 0xAA} ,channel = 0 , ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	for(channel = 0; channel < CHANNEL_MAX; channel++)
	{
		ret = i2c_master_read(I2C_DEVICE_ADDR, rssi_addr[channel], (uint8_t *)&rssi_ua[channel], 2);
	}
	uart_printf( "\r\nRssi_ua %-5hu %-5hu %-5hu %-5hu", rssi_ua[0],rssi_ua[1],rssi_ua[2],rssi_ua[3]);

	return ret;
}

uint32_t qsfp28_get_mod_soft_type()
{
	uint32_t ret = RET_OK;
	static uint16_t num = 0;

	_mod_type 	  = INVALID;
	_mod_sub_type = INVALID;
	_mod_hw_rev   = INVALID;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_05H);
	JUDGE_RETURN(ret);

	i2c_master_read_byte(I2C_DEVICE_ADDR, MSA_OFF_MODULE_TYPE, &_mod_type);
	i2c_master_read_byte(I2C_DEVICE_ADDR, MSA_OFF_MODULE_SUB_TYPE, &_mod_sub_type);
	i2c_master_read_byte(I2C_DEVICE_ADDR, MSA_OFF_HW_VER, &_mod_hw_rev);

	ret =  (uint32_t)_mod_type << 16 | (uint32_t)_mod_sub_type << 8 | _mod_hw_rev;

	/*
	switch(((uint32_t)_mod_type << 8)|((uint32_t)_mod_sub_type))
	{
		case 0x101:
			AOC_101_cmd_init();
			break;
		case 0x206:
			CWDM4_206_cmd_init();
			break;
		case 0x301:
			LR4_301_cmd_init();
			break;
		case 0x207:
			CWDM4_207_cmd_init();
			break;
		case 0x211:
			CWDM4_211_cmd_init();
			break;
		default:
			break;
	}
	*/
	terminal_register_cmd(NULL,NULL);
	return ret;
}

uint8_t qsfp28_get_mod_type()
{
	return _mod_type;
}

uint8_t qsfp28_get_mod_release_info()
{
	uint8_t ret = RET_OK;
	char mod_name[32] = {0};
	MODULE_REALEASE_INFO  mod_realea_info;

	qsfp28_get_mod_soft_type();
	qsfp28_get_mod_name(mod_name);

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_MOD_REA_SEC, (uint8_t *)&mod_realea_info, sizeof(mod_realea_info));

	if(strcmp(mod_name, "UNKNOWN"))
	{
		uart_printf("\r\n%s-AM%bu_v%bu.%bu	20%bu.%bu.%bu-%bu:%bu",
		mod_name, _mod_hw_rev,
		mod_realea_info.ver_num_msb, mod_realea_info.ver_num_lsb,
		mod_realea_info.year, mod_realea_info.mon, mod_realea_info.day,
		mod_realea_info.hour, mod_realea_info.min);
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}

uint8_t qsfp28_get_msa_lower_page_monitor()
{
	return i2c_master_read(I2C_DEVICE_ADDR, MSA_PAGE_LOWER_MON_BASE_ADDR, (uint8_t *)&_lower_mon, sizeof(MSA_PAGE_LOWER_MON_T));
}

static uint8_t qsfp28_get_ddmi_vcc(DDMI_CALI_TYPE type)
{
	uint8_t ret = RET_OK;

	if(type == DDMI_CALI_BEFORE)
	{
		uint16_t vcc;

		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
		JUDGE_RETURN(ret);
		ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_DDMI_VCC_MSB, (uint8_t *)&vcc, 2);		//uint:0.1mv
		JUDGE_RETURN(ret);
		uart_printf("\r\nDdmi_vcc:%f(v)",(float)vcc * 0.1 / 1000.0);
	}
	else if(type == DDMI_CALI_AFTER)
	{
		//_lower_mon.vcc uint:0.1mV
		uart_printf("\r\nDdmi_cali_vcc:%f(v)",(float)_lower_mon.vcc * 0.1 / 1000.0);  //uint:V
	}

	return ret;
}

static uint8_t qsfp28_get_ddmi_temp(DDMI_CALI_TYPE type)
{
	uint8_t ret = RET_OK;

	if(type == DDMI_CALI_BEFORE)
	{
		int16_t temp = 0;

		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
		JUDGE_RETURN(ret);
		ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_DDMI_TEMP_MSB, (uint8_t *)&temp, 2);
		JUDGE_RETURN(ret);
		uart_printf("\r\nDdmi_temp:%f(C)", (float)temp/(float)DDMI_CALI_TEMP_SCALE);
	}
	else if(type == DDMI_CALI_AFTER)
	{
		//_lower_mon.temp uint:1/256 C
		uart_printf("\r\nDdmi_cali_temp:%f(C)",(float)_lower_mon.temp/(float)DDMI_CALI_TEMP_SCALE);  //uint:ÉãÊÏ¶È
	}

	return ret;
}

static uint8_t qsfp28_get_ddmi_bias(uint8_t channel, DDMI_CALI_TYPE type)
{
	uint8_t ret = RET_OK;

	if(channel >= CHANNEL_MAX)
	{
		uart_printf("\r\nchannedl%bu is not support\r\n",channel);
		ret = RET_FAILED;
	}

	if(type == DDMI_CALI_BEFORE)
	{
		uint16_t bias;
		uint8_t bias_addr[] = {0x84, 0x90, 0x9C, 0xA8};

		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
		JUDGE_RETURN(ret);
		ret = i2c_master_read(I2C_DEVICE_ADDR, bias_addr[channel], (uint8_t *)&bias, 2);  //uint:0.1mA
		JUDGE_RETURN(ret);
		uart_printf("\r\nDdmi_bias%bu:%f(mA)",channel, (float)bias * 0.1);  //uint:mA
	}
	else if(type == DDMI_CALI_AFTER)
	{
		//_lower_mon.txbias[channel]  uint:2uA
		uart_printf("\r\nDdmi_cali_bias%bu:%f(mA)",channel, (float)_lower_mon.txbias[channel] * 2.0 / 1000.0);  //uint:mA
	}
	return ret;
}

static uint8_t qsfp28_get_ddmi_txmon(uint8_t channel, DDMI_CALI_TYPE type)
{
	uint8_t ret = RET_OK;

	if(channel >= CHANNEL_MAX)
	{
		uart_printf("\r\nchannedl%bu is not support\r\n",channel);
		ret = RET_FAILED;
	}

	if(type == DDMI_CALI_BEFORE)
	{
		uint16_t txmon;
		uint8_t  txmon_addr[] = {0x88, 0x94, 0xA0, 0xAC};

		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
		JUDGE_RETURN(ret);
		ret = i2c_master_read(I2C_DEVICE_ADDR, txmon_addr[channel], (uint8_t *)&txmon, 2);  //uint:0.1mA
		if(qsfp28_get_mod_type() != MODULE_TYPE_SR4 )
		{
			uart_printf("\r\nDdmi_txmon%bu:%f(mV)",channel, (float)txmon * 0.1 );  //uint:mA
		}
		else 
		{
			uart_printf("\r\nDdmi_txmon%bu:%f(mA)",channel, (float)txmon * 0.1 );  //uint:mA
		}
	}
	else if(type == DDMI_CALI_AFTER)
	{
		//_lower_mon.txpower uint:0.1uW
		uart_printf("\r\nDdmi_cali_txpower%bu:%f(dbm)",channel, 10.0 *log10(((float)_lower_mon.txpower[channel] * 0.1 /1000.0)));
	}
	return ret;
}

static uint8_t qsfp28_get_ddmi_rssi(uint8_t channel,DDMI_CALI_TYPE type )
{
	uint8_t ret = RET_OK;

	if(channel >= CHANNEL_MAX)
	{
		uart_printf("\r\nchannedl%d is not support\r\n",channel);
		ret = RET_FAILED;
	}

	if(type == DDMI_CALI_BEFORE)
	{
		uint16_t rssi;
		uint8_t rssi_addr[] = {0x86, 0x92, 0x9E, 0xAA};

		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
		JUDGE_RETURN(ret);
		ret = i2c_master_read(I2C_DEVICE_ADDR, rssi_addr[channel], (uint8_t *)&rssi, 2);  //uint:uA
		JUDGE_RETURN(ret);
		uart_printf("\r\nDdmi_rssi%bu:%hu(uA)",channel, rssi);
	}
	else if(type == DDMI_CALI_AFTER)
	{
		uart_printf("\r\nDdmi_cali_rxpower%bu:%f(dbm)",channel, 10 *log10(((float)_lower_mon.rssi[channel] * 0.1 /1000.0)));
	}
	return ret;
}

uint8_t qsfp28_get_ddmi(DDMI_DATA_TYPE ddmi_type, uint8_t channel, DDMI_CALI_TYPE type)
{
	uint8_t ret = RET_OK;

	switch(ddmi_type)
	{
		case DDMI_VCC:
		{
			ret = qsfp28_get_ddmi_vcc(type);
			break;
		}
		case DDMI_TEMP:
		{
			ret = qsfp28_get_ddmi_temp(type);
			break;
		}
		case DDMI_BIAS:
		{
			ret = qsfp28_get_ddmi_bias(channel, type);
			break;
		}
		case DDMI_TXMON:
		{
			ret = qsfp28_get_ddmi_txmon(channel, type);
			break;
		}
		case DDMI_RSSI:
		{
			ret = qsfp28_get_ddmi_rssi(channel, type);
			break;
		}
		default:
			break;
	}
	return ret;
}

uint8_t qsfp28_set_ddmi_cali_para(DDMI_DATA_TYPE ddmi_type, uint8_t channel, DDMI_CALI_PARA_T* cali_para)
{
	uint8_t ret = RET_OK;
	uint8_t page_select[] = {SFP_PAGE_06H, SFP_PAGE_07H, SFP_PAGE_07H, SFP_PAGE_07H};

	switch(ddmi_type)
	{
		case DDMI_VCC:
		{
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, (uint8_t)SFP_PAGE_06H, PAGE6_DDMI_CALI_VCC_OFFSET_MSB, (uint8_t *)&cali_para->vcc, 2);
			break;
		}
		case DDMI_TEMP:
		{
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, (uint8_t)SFP_PAGE_06H, PAGE6_DDMI_CALI_TEMP_OFFSET_MSB, (uint8_t *)&cali_para->temp, 2);
			break;
		}
		case DDMI_BIAS:
		{
			uint8_t bias_para_addr[] = {PAGE6_DDMI_CALI_CH0_BIAS_BASE_ADDR, PAGE7_DDMI_CALI_CH1_BIAS_BASE_ADDR, PAGE7_DDMI_CALI_CH2_BIAS_BASE_ADDR, PAGE7_DDMI_CALI_CH3_BIAS_BASE_ADDR};
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, (uint8_t)page_select[channel], bias_para_addr[channel], (uint8_t *)&cali_para->current[channel].bias, sizeof(cali_para->current[0].bias));
			break;
		}
		case DDMI_TXMON:
		{
			uint8_t txpower_para_addr[] = {PAGE6_DDMI_CALI_CH0_TXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH1_TXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH2_TXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH3_TXPOWER_BASE_ADDR};
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, page_select[channel], txpower_para_addr[channel], (uint8_t *)&cali_para->current[channel].txpower, sizeof(cali_para->current[0].txpower));
			break;
		}
		case DDMI_RSSI:
		{
			uint8_t rxpower_para_addr[] = {PAGE6_DDMI_CALI_CH0_RXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH1_RXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH2_RXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH3_RXPOWER_BASE_ADDR};
			ret = smb_write_page_byte(I2C_DEVICE_ADDR, page_select[channel], rxpower_para_addr[channel], (uint8_t *)&cali_para->current[channel].rxpower, sizeof(cali_para->current[0].rxpower));
			break;
		}
		default:
			break;
	}
	return ret;
}

uint8_t qsfp28_get_ddmi_cali_para(DDMI_DATA_TYPE ddmi_type, uint8_t channel, DDMI_CALI_PARA_T* cali_para)
{
	uint8_t ret = RET_OK;

	if(channel == 0x0 || ddmi_type == DDMI_VCC || ddmi_type == DDMI_TEMP)
	{
		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_06H);
		JUDGE_RETURN(ret);
	}
	else
	{
		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_07H);
		JUDGE_RETURN(ret);
	}

	switch(ddmi_type)
	{
		case DDMI_VCC:
		{
			ret = i2c_master_read(I2C_DEVICE_ADDR, PAGE6_DDMI_CALI_VCC_OFFSET_MSB, (uint8_t *)&cali_para->vcc, 2);
			break;
		}
		case DDMI_TEMP:
		{
			ret = i2c_master_read(I2C_DEVICE_ADDR, PAGE6_DDMI_CALI_TEMP_OFFSET_MSB, (uint8_t *)&cali_para->temp, 2);
			break;
		}
		case DDMI_BIAS:
		{
			uint8_t bias_para_addr[] = {PAGE6_DDMI_CALI_CH0_BIAS_BASE_ADDR, PAGE7_DDMI_CALI_CH1_BIAS_BASE_ADDR, PAGE7_DDMI_CALI_CH2_BIAS_BASE_ADDR, PAGE7_DDMI_CALI_CH3_BIAS_BASE_ADDR};
			ret = i2c_master_read(I2C_DEVICE_ADDR, bias_para_addr[channel], (uint8_t *)&cali_para->current[channel].bias, sizeof(cali_para->current[0].bias));
			break;
		}
		case DDMI_TXMON:
		{
			uint8_t txpower_para_addr[] = {PAGE6_DDMI_CALI_CH0_TXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH1_TXPOWER_BASE_ADDR,PAGE7_DDMI_CALI_CH2_TXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH3_TXPOWER_BASE_ADDR};
			ret = i2c_master_read(I2C_DEVICE_ADDR, txpower_para_addr[channel], (uint8_t *)&cali_para->current[channel].txpower, sizeof(cali_para->current[0].txpower));
			break;
		}
		case DDMI_RSSI:
		{
			uint8_t rxpower_para_addr[] = {PAGE6_DDMI_CALI_CH0_RXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH1_RXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH2_RXPOWER_BASE_ADDR, PAGE7_DDMI_CALI_CH3_RXPOWER_BASE_ADDR};
			ret = i2c_master_read(I2C_DEVICE_ADDR, rxpower_para_addr[channel], (uint8_t *)&cali_para->current[channel].rxpower, sizeof(cali_para->current[0].rxpower));
			break;
		}
		default:
			break;
	}

	return ret;
}

uint8_t qsfp28_set_lut_tab(uint8_t channel, MSA_LUT_TYPE type,  MSA_LASER_CH_CFG_T cfg)
{
	uint8_t ret = RET_OK;

	ret = smb_write_page_byte(I2C_DEVICE_ADDR, lut_page_select[type], lut_temp_select[type], (uint8_t *)&cfg.temp, 2);
	JUDGE_RETURN(ret);
	ret = smb_write_page_byte(I2C_DEVICE_ADDR, lut_page_select[type], lut_laser_cfg_select[type][channel], (uint8_t *)&cfg.bias, 6);
	return ret;
}

uint8_t qsfp28_get_lut_tab(uint8_t channel,LUT_SHOW_TYPE show_type, MSA_LUT_TYPE type)
{
	uint8_t *lt = "lt";
	uint8_t *mt = "mt";
	uint8_t *ht = "ht";
	uint8_t ret = RET_OK;
	MSA_LASER_CH_CFG_T laser_cfg;
	uint8_t *temp_type[MSA_LUT_TYPE_MAX] = {"lt","mt","ht"};

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, lut_page_select[type]);
	JUDGE_RETURN(ret);

	ret = i2c_master_read(I2C_DEVICE_ADDR, lut_temp_select[type], (uint8_t *)&laser_cfg.temp, 2);
	ret = i2c_master_read(I2C_DEVICE_ADDR, lut_laser_cfg_select[type][channel], (uint8_t *)&laser_cfg.bias, 6);

	if(show_type == PART_LUT_INFO)
	{
		uart_printf("\r\nch%bu-%s:temp     bias    mod     crossing", channel,temp_type[type]);
		uart_printf("\r\n\t%hd\t%hu\t%hu\t%hd",laser_cfg.temp/DDMI_CALI_TEMP_SCALE, laser_cfg.bias, laser_cfg.mod, laser_cfg.cross);
	}
	else
	{
		uart_printf("\r\nch%bu-%s\t%hd\t%hu\t%hu\t%hd",channel,temp_type[type],laser_cfg.temp/DDMI_CALI_TEMP_SCALE, laser_cfg.bias, laser_cfg.mod, laser_cfg.cross);
	}

	return ret;
}


uint8_t qsfp28_set_lut_tab_para_from_reg_inner(uint8_t channel, SAVE_CUR_LUT_TAB cur_lut)
{
	uint8_t ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA0, cur_lut);
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA1, channel);
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, DBG_SET_CUR_LUT_CMD);

	if(qsfp28_get_cmd_status() == false)
	{
		uart_printf("%s","\r\nSet lut tab now is failed");
		ret = RET_FAILED;
	}
	return ret;
}

uint8_t qsfp28_get_fixed_temp_point_apc_value(int8_t temp, uint8_t channel)
{
	MSA_LASER_CH_CFG_T cfg;
	uint8_t ret = RET_OK;

	cfg.temp = temp * DDMI_CALI_TEMP_SCALE; //because the module receive result is actual temp * 256

	if(channel == CHANNEL_MAX)
	{
		return RET_FAILED;
	}

	uart_printf("\r\n   ch%bu:temp     bias    mod     crossing", channel);

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA0, channel & 0xFF);
	ret = i2c_master_write(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA2, (uint8_t *)&cfg.temp, 2);
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, DBG_APC_CALI_CMD);

	if(qsfp28_get_cmd_status() == false)
	{
		uart_printf("%s","\r\nQsfp28_get_fixed_temp_point_apc_value failed");
		return RET_FAILED;
	}

	ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA4, (uint8_t *)&cfg.bias, 2);
	ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA6, (uint8_t *)&cfg.mod, 2);
	ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA8, (uint8_t *)&cfg.cross, 2);
	uart_printf("\r\n\t%hd\t%hu\t%hu\t%hd",cfg.temp/DDMI_CALI_TEMP_SCALE, cfg.bias, cfg.mod, cfg.cross);

	return ret;
}


uint8_t  qsfp28_get_cur_apc_value(uint8_t channel)
{
	uint8_t ret = RET_OK;
	MSA_LASER_CH_CFG_T cfg;

	if(channel == CHANNEL_MAX)
	{
		return RET_FAILED;
	}

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_DDMI_TEMP_MSB, (uint8_t *)&cfg.temp, 2);
	ret = ((RET_FAILED == qsfp28_get_fixed_temp_point_apc_value(cfg.temp/DDMI_CALI_TEMP_SCALE, channel))? RET_FAILED : RET_OK);

	return ret;
}

static uint8_t qsfp28_read_log()
{
	uint8_t log[128];
	uint8_t i = 0, len = 0, ret = RET_OK;

	memset(log, '\0', sizeof(log));

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, MSA_PAGE_02H);
	JUDGE_RETURN(ret);

	ret = i2c_master_read(I2C_DEVICE_ADDR, SFP_DEBUG_LOG_START_ADDR, &log[0], 128);
	uart_printf("\r\nLog:%s",log);

	return ret;
}


uint8_t qsfp28_debug_log_handle(DEBUG_LOG_ACTION action)
{
	uint8_t ret = RET_OK;

	if(action == READ_LOG_CMD)
	{
		ret = qsfp28_read_log();
	}
	else if(action == DISABLE_LOG_CMD || action == ENABLE_LOG_CMD || action == CLEAR_LOG_CMD)
	{
		ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
		JUDGE_RETURN(ret);
		ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA0, action);
		ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, DBG_DEBUG_LOG_CMD);

		if(qsfp28_get_cmd_status() == false)
		{
			uart_printf("%s","\r\nQsfp28_debug_log_handle failed");
			ret = RET_FAILED;
		}
	}
	else
	{
		ret = RET_FAILED;
	}
	return ret;
}


static uint8_t qsfp28_eep_save(uint8_t dev_addr, uint16_t offset_addr, uint8_t value)
{
	uint8_t ret = RET_OK;
	SAVE_REG_T save_reg;

	save_reg.devAddr 	= dev_addr;
	save_reg.offsetAddr = offset_addr;
	save_reg.value 		= value;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	ret = i2c_master_write(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA0, (uint8_t *)&save_reg, sizeof(SAVE_REG_T));
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, DBG_SAVE_REG_TO_EEP_CMD);

	if(qsfp28_get_cmd_status() == false)
	{
		uart_printf("%s","\r\neep save failed");
		ret = RET_FAILED;
	}

	return ret;
}


static uint8_t qsfp28_eep_show()
{
	uint8_t eep_data_size = 0, i = 0, ret = RET_OK;
	SAVE_REG_T save_reg;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_05H);
	JUDGE_RETURN(ret);

	ret = i2c_master_read_byte(I2C_DEVICE_ADDR, EEP_DATA_SIZE, &eep_data_size);

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_0AH);
	JUDGE_RETURN(ret);

	uart_printf("\r\n\tDevAddr\t\tOffsetAddr\tValue");
	for (i = 0; i < (eep_data_size/4); i++)
	{
		ret = i2c_master_read(I2C_DEVICE_ADDR, PAGE10_START_ADDR + 4 * i, (uint8_t *)&save_reg, sizeof(SAVE_REG_T));
		uart_printf("\r\n\t0x%bx\t\t0x%hx\t\t0x%bx",save_reg.devAddr, save_reg.offsetAddr, save_reg.value);
		com_delay(10);
	}

	return ret;
}

uint8_t qsfp28_eep_handle(EEP_ACTION action, uint8_t dev_addr, uint16_t offset_addr, uint8_t value)
{
	uint8_t ret = RET_OK;

	switch(action)
	{
		case EEP_INIT:
		{
			ret = qsfp28_msa_page4_dbg_cmd(DBG_EEP_INIT_CMD);
			break;
		}
		case EEP_SAVE:
		{
			ret = qsfp28_eep_save(dev_addr, offset_addr, value);
			break;
		}
		case EEP_SHOW:
		{
			ret = qsfp28_eep_show();
			break;
		}
		case EEP_ERASE:
		{
			ret = qsfp28_msa_page4_dbg_cmd(DBG_EEP_ERASE_CMD);  //erase all eep info
			break;
		}
		default:
			break;
	}

	return ret;
}

uint8_t qsfp28_msa_page4_dbg_cmd(MSA_PAGE4_DBG_CMD_T cmd)
{
	uint8_t ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, cmd);

	if(qsfp28_get_cmd_status() == false)
	{
		uart_printf("%s","\r\nCmd failed");
		ret = RET_FAILED;
	}

	return ret;
}


uint8_t qsfp28_config_mod_console(bool enable)
{
	uint8_t SFRPAGE_SAVE = SFRPAGE, ret = RET_OK;
	uint8_t tmp = enable ? 1 : 0;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA0, tmp);
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, DBG_SET_MOD_CONSOLE);

	if(enable)  //enable module uart func
	{
		if(qsfp28_get_cmd_status() == false)
		{
			uart_printf("%s","\r\nenable module uart failed");
			return RET_FAILED;
		}

		SFRPAGE = 0x0;
		P1MDOUT &= ~P1MDOUT_B5__PUSH_PULL ;

		SFRPAGE = 0x20;
		P2MDOUT &= ~P2MDOUT_B0__PUSH_PULL ;

		RESET  = 1;
		MODSEL = 1;

		SFRPAGE = SFRPAGE_SAVE;
	}
	else  //disable module uart func
	{
		SFRPAGE = 0x0;
		P1MDOUT |= P1MDOUT_B5__PUSH_PULL ;

		SFRPAGE = 0x20;
		P2MDOUT |= P2MDOUT_B0__PUSH_PULL ;

		RESET  = 1;
		MODSEL = 0;

		if(qsfp28_get_cmd_status() == false)
		{
			uart_printf("%s","\r\ndisable module uart failed");
			return RET_FAILED;
		}
	}

	SFRPAGE = SFRPAGE_SAVE;

	return ret;
}

uint8_t qsfp28_get_tec(SFP_PAGE page,uint8_t tec,uint16_t *Pdata)
{
	uint8_t datab[2];
	uint8_t ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, page);
	JUDGE_RETURN(ret);
	ret = i2c_master_read(I2C_DEVICE_ADDR,tec,datab,2);
	*Pdata = (uint16_t)((datab[0] << 8) & 0xff00)| datab[1];

	return ret;
}

RETURN_VALUE_TYPE LR4_301_set_tec_able(uint8_t enale)
{
	RETURN_VALUE_TYPE  ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);

	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_CMD, DBG_TEC_ENABLE_CMD);
	ret = i2c_master_write_byte(I2C_DEVICE_ADDR, SFP_DBG_READ_DATA0, enale);

	if(qsfp28_get_cmd_status() == false)
	{
		uart_printf("%s","\r\nenable module uart failed");
	}
	return ret;
}

RETURN_VALUE_TYPE  qsfp28_set_tec(SFP_PAGE page,uint8_t tec,uint16_t *Pdata)
{
	uint8_t ret = RET_OK;
	uint8_t datab[2] = {0};

	datab[0]= *Pdata >> 8;
	datab[1]= *Pdata;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, page);
	JUDGE_RETURN(ret);
	ret = smb_write_page_byte(I2C_DEVICE_ADDR, page, tec, datab, 2);
	return ret;
}

static uint8_t qsfp28_get_each_thrhld_value(MSA_PAGE3_ALARM_WARING_THRESHOLD_T type)
{
	uint16_t value16 = 0;
	uint8_t ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, MSA_PAGE_03H);
	JUDGE_RETURN(ret);

	ret = i2c_master_read(I2C_DEVICE_ADDR, page3_alarm_wran_select[type], (uint8_t *)&value16, 2);

	switch(type)
	{
		case TEMP_HIGH_ALARM:
		{
			uart_printf("\r\nthrhld_temp_high_alarm\t:%f(C)", ((float)(int16_t)value16)/(float)DDMI_CALI_TEMP_SCALE);
			break;
		}
		case TEMP_LOW_ALARM:
		{
			uart_printf("\r\nthrhld_temp_low_alarm\t:%f(C)", ((float)(int16_t)value16)/(float)DDMI_CALI_TEMP_SCALE);
			break;
		}
		case TEMP_HIGH_WARN:
		{
			uart_printf("\r\nthrhld_temp_high_warn\t:%f(C)", ((float)(int16_t)value16)/(float)DDMI_CALI_TEMP_SCALE);
			break;
		}
		case TEMP_LOW_WARN:
		{
			uart_printf("\r\nthrhld_temp_low_warn\t:%f(C)", ((float)(int16_t)value16)/(float)DDMI_CALI_TEMP_SCALE);
			break;
		}
		case VCC_HIGH_ALARM:    //vcc
		{
			uart_printf("\r\nthrhld_vcc_high_alarm\t:%f(V)", (float)(value16) * 0.1 / 1000.0);
			break;
		}
		case VCC_LOW_ALARM:
		{
			uart_printf("\r\nthrhld_vcc_low_alarm\t:%f(V)", (float)(value16) * 0.1 / 1000.0);
			break;
		}
		case VCC_HIGH_WARN:
		{
			uart_printf("\r\nthrhld_vcc_high_warn\t:%f(V)", (float)(value16) * 0.1 / 1000.0);
			break;
		}
		case VCC_LOW_WARN:
		{
			uart_printf("\r\nthrhld_vcc_low_warn\t:%f(V)", (float)(value16) * 0.1 / 1000.0);
			break;
		}
		case RXPWR_HIGH_ALARM: //rxpwr
		{
			uart_printf("\r\nthrhld_rxpwr_high_alarm\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		case RXPWR_LOW_ALARM:
		{
			uart_printf("\r\nthrhld_rxpwr_low_alarm\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		case RXPWR_HIGH_WARN:
		{
			uart_printf("\r\nthrhld_rxpwr_high_warn\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		case RXPWR_LOW_WARN:
		{
			uart_printf("\r\nthrhld_rxpwr_low_warn\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		case TXBIAS_HIGH_ALARM: //txbias
		{
			uart_printf("\r\nthrhld_txbias_high_alarm:%f(mA)", (float)value16 * 2.0 / 1000.0);
			break;
		}
		case TXBIAS_LOW_ALARM:
		{
			uart_printf("\r\nthrhld_txbias_low_alarm\t:%f(mA)", (float)value16 * 2.0 / 1000.0);
			break;
		}
		case TXBIAS_HIGH_WARN:
		{
			uart_printf("\r\nthrhld_txbias_high_warn\t:%f(mA)", (float)value16 * 2.0 / 1000.0);
			break;
		}
		case TXBIAS_LOW_WARN:
		{
			uart_printf("\r\nthrhld_txbias_low_warn\t:%f(mA)", (float)value16 * 2.0 / 1000.0);
			break;
		}
		case TXPWR_HIGH_ALARM:  //txpwr
		{
			uart_printf("\r\nthrhld_txpwr_high_alarm\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		case TXPWR_LOW_ALARM:
		{
			uart_printf("\r\nthrhld_txpwr_low_alarm\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		case TXPWR_HIGH_WARN:
		{
			uart_printf("\r\nthrhld_txpwr_high_warn\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		case TXPWR_LOW_WARN:
		{
			uart_printf("\r\nthrhld_txpwr_low_warn\t:%f(dbm)", 10.0 *log10(((float)value16 * 0.1 /1000.0)));
			break;
		}
		default:
		{
			ret = RET_FAILED;
			break;
		}
	}

	return ret;
}

uint8_t qsfp28_set_thrhld_value(MSA_PAGE3_ALARM_WARING_THRESHOLD_T type, uint16_t value)
{
	uint8_t ret = RET_OK;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, MSA_PAGE_03H);
	JUDGE_RETURN(ret);
	ret = i2c_master_write(I2C_DEVICE_ADDR, page3_alarm_wran_select[type],(uint8_t *)&value, 2);

	return ret;
}

uint8_t qsfp28_get_thrhld_value(MSA_PAGE3_ALARM_WARING_THRESHOLD_T type)
{
	uint16_t value = 0;
	uint8_t i = 0, ret = RET_OK;

	if(type >= TEMP_HIGH_ALARM && type < HTRHLD_ALL)
	{
		ret = qsfp28_get_each_thrhld_value(type);
	}
	else if(type == HTRHLD_ALL)
	{
		for(i = TEMP_HIGH_ALARM; i < HTRHLD_ALL; i++)
		{
			ret = qsfp28_get_each_thrhld_value(i);
		}
	}
	else
	{
		ret = RET_FAILED;
	}

	return ret;
}

uint8_t qsfp28_get_module_class()
{
	uint8_t ret = RET_FAILED;
	uint8_t value = 0;

	ret = smb_read_page_byte(I2C_DEVICE_ADDR,SFP_PAGE_05H, MSA_MODULE_CLASS, &value, 1);
	JUDGE_RETURN(ret);
	uart_printf("\r\n module_class:%bu",value);
	return ret;
}

uint8_t qsfp28_set_module_class(uint8_t value)
{
	uint8_t ret = RET_FAILED;

	ret = smb_write_page_byte(I2C_DEVICE_ADDR,SFP_PAGE_05H, MSA_MODULE_CLASS, &value, 1);
	return ret;
}

uint8_t qsfp28_disable_module_poll()
{
	uint8_t ret = RET_FAILED;
	uint16_t value = 0;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);
	ret = i2c_master_read(I2C_DEVICE_ADDR,SFP_DDMI_ENABLE,(uint8_t *)&poll, 2);
	ret = i2c_master_write(I2C_DEVICE_ADDR,SFP_DDMI_ENABLE,(uint8_t *)&value, 0x2);

	return ret;
}

uint8_t qsfp28_enable_module_poll()
{
	uint8_t ret = RET_FAILED;

	ret = qsfp28_select_page(I2C_DEVICE_ADDR, SFP_PAGE_SELECT, SFP_PAGE_04H);
	JUDGE_RETURN(ret);
	ret = i2c_master_write(I2C_DEVICE_ADDR,SFP_DDMI_ENABLE, (uint8_t *)&poll, 0x2);

	return ret;
}

static uint8_t qsfp28_check_module_security_level(uint8_t page)
{
	uint8_t ret = RET_OK, value = 0xCC;  //don't write value = 0xFE and 0xFF, because 0xFF indicate fail and 0xFE indicate success

	if(page >= SFP_PAGE_04H && page < SFP_NONE_PAGE)
	{
		i2c_master_write_byte(I2C_DEVICE_ADDR,SFP_PAGE_SELECT, SFP_PAGE_04H);
		i2c_master_write(I2C_DEVICE_ADDR, SFP_MOD_SECUR_LEVEL_STATUS, (uint8_t *)&value, 0x1);

		value = 0x0;
		i2c_master_read(I2C_DEVICE_ADDR, SFP_MOD_SECUR_LEVEL_STATUS, (uint8_t *)&value, 0x1);

		if(value == 0xCC)  //Write OK indicate enter security lever
		{
			ret = RET_OK;
		}
		else
		{
			ret = RET_NO_PERMISSION;
		}
	}

	return ret;
}

uint8_t qsfp28_select_page(uint8_t BaseAddr,uint16_t OffsetAddr,uint8_t page)
{
	uint8_t ret = RET_OK;

	ret = qsfp28_check_module_security_level(page);
	JUDGE_RETURN(ret);
	ret = i2c_master_write_byte(BaseAddr,OffsetAddr, page);

	return ret;
}


