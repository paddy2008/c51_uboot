#include "mcu_sal.h"


static I2C_MASTER_STRUCT_T _i2c_master_struct;


static void _i2c_master_struct_init(void)
{
	memset((void *)&_i2c_master_struct, 0 ,sizeof(_i2c_master_struct));

	_i2c_master_struct.freq = I2C_MASTER_DEF_FREQUENCY;
}

static void _i2c_master_update_width_flag(uint8_t baseAddr)
{
	if(baseAddr == I2C_SLAVE_DEV1_ADDR || baseAddr == I2C_SLAVE_DEV2_ADDR || baseAddr == I2C_SLAVE_DEV3_ADDR) //12bit offsetaddr
	{
		 _i2c_master_struct.widthFlag = true;
	}
	else
	{
		 _i2c_master_struct.widthFlag = false;
	}
}

static void _i2c_master_check_bus_status()
{
	uint8_t i = 0;

	for(i = 0; i < 50; i++)
	{
		if(SDA && SCL)
		{
			return;
		}
		lib_delay_us(10);
	}

	uart_printf("Bus status failed:SDA = %bu, SCL = %bu", (SDA)? 1: 0, (SCL)? 1: 0);
}

static void _i2c_master_dev_init (void)
{
   uint8_t SFRPAGE_SAVE = SFRPAGE;

   SFRPAGE = 0x0;

   XBR0 |= XBR0_SMB0E__ENABLED; 		//enable SMB0 I/O

   SMB0CF = 0x5C;                      // Use Timer0 overflows as SMBus clock source;
                                       // enable slave mode;
                                       // Enable setup & hold time extensions;
                                       // enable SMBus Free timeout detect;
                                       // enable SCL low timeout detect;
   SMB0CF |= 0x80;                     // Enable SMBus;

   EIE1 |= 0x01;                 	   // Enable the SMBus interrupt

   SFRPAGE  = 0x10;
   EIP1  &= ~0x01;				   	   // Make smbus  high priority2,priority3 is the highest priority!
   EIP1H |= 0x01;

   SFRPAGE = SFRPAGE_SAVE;
}

void i2c_master_hw_init()
{
	_i2c_master_struct_init();
	_i2c_master_dev_init();
	_i2c_master_check_bus_status();
}

void i2c_master_hw_reset(void)
{
	SMB0CF &= ~0x80;           // Reset communication
	SMB0CF |= 0x80;
	SMB0CN0_STA = 0;
	SMB0CN0_STO = 0;
	SMB0CN0_ACK = 0;
	_i2c_master_struct.busyFlag    = 0;              // Free SMBus
	_i2c_master_struct.ackPollFlag = 0;
}

uint8_t i2c_master_hw_write(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len)
{
	uint32_t i = 0;

	if(len == 0 || (buff == NULL))
	{
		return RET_FAILED;
	}

	for(i = 0; _i2c_master_struct.busyFlag & (i < 0xffff); i++); // Wait for SMBus to be free.

	_i2c_master_struct.busyFlag = 1;                       // Claim SMBus (set to busyFlag)

   // Set i2c master ISR parameters
   _i2c_master_struct.baseAddr     = baseAddr;
   _i2c_master_struct.offAddr      = offsetAddr;
   _i2c_master_struct.rwFlag       = I2C_WRITE;
   _i2c_master_struct.offAddrFlag  = 1;
   _i2c_master_struct.rwChangeFlag = 0;
   _i2c_master_struct.ackPollFlag  = 1;
   _i2c_master_struct.buff 		   = buff;
   _i2c_master_struct.len   	   = len;

   _i2c_master_update_width_flag(baseAddr);

   SMB0CN0_STA = 1;  //send start signal

   for (i = 0; _i2c_master_struct.busyFlag; i++)    // Wait until data I2C_WRITE OK
   {
		if (i >= 0xffff)
		{
			uart_printf("\r\nWrite byte timeout,reset smbus");
			i2c_master_hw_reset();
			return RET_FAILED;
		}
	}
   return RET_OK;
}


uint8_t i2c_master_hw_read(uint8_t baseAddr,uint16_t offsetAddr,uint8_t *buff,uint16_t len)
{
	uint32_t i = 0;

	if(len == 0 || (buff == NULL))
	{
		return RET_FAILED;
	}

	for (i = 0; _i2c_master_struct.busyFlag & (i < 0xffff); i++); // Wait for SMBus to be free.

	_i2c_master_struct.busyFlag = 1;                       // Claim SMBus (set to busyFlag)

	// Set SMBus ISR parameters
	_i2c_master_struct.baseAddr 	 = baseAddr;
	_i2c_master_struct.offAddr 		 = offsetAddr;
	_i2c_master_struct.rwFlag 		 = I2C_WRITE;
	_i2c_master_struct.offAddrFlag   = 1;
	_i2c_master_struct.rwChangeFlag  = 1;
	_i2c_master_struct.ackPollFlag   = 1;
	_i2c_master_struct.buff 		 = buff;
	_i2c_master_struct.len 			 = len;

	 _i2c_master_update_width_flag(baseAddr);

	SMB0CN0_STA = 1;  //send start signal
	for(i = 0; _i2c_master_struct.busyFlag; i++)                       // Wait until data is I2C_READ
	{
		if (i >= 0xffff)
		{
 			uart_printf("\r\nRead multi type timeout,reset smbus");
 			i2c_master_hw_reset();
			return RET_FAILED;
		}
	}
	return RET_OK;
}

uint16_t i2c_master_hw_get_freq(void)
{
	return (uint16_t)_i2c_master_struct.freq;
}

void i2c_master_hw_set_freq(uint16_t freq)
{
	_i2c_master_struct.freq = freq;
	lib_i2c_master_timer_init(freq);
}


SI_INTERRUPT(SMBUS0_ISR, SMBUS0_IRQn)
{
	bit fail = 0;                		// Used by the ISR to flag failed transfers
	static uint16_t i = 0;       		// Used by the ISR to count the number of data bytes sent or received

   if (SMB0CN0_ARBLOST == 0)            // Check for errors
   {
	   switch (SMB0CN0 & 0xF0)             // Status vector
	   {
		  // Master Transmitter/Receiver: START condition transmitted.
		  case SMB_MTSTA:
			 SMB0DAT = (MY_CLEAR_BIT(_i2c_master_struct.baseAddr, 0) | _i2c_master_struct.rwFlag);
			 SMB0CN0_STA = 0;               // Manually clear START bit
			 i = 0;                         // Reset data byte counter
			 break;

		  // Master Transmitter: Data byte (or Slave Address) transmitted
		  case SMB_MTDB:
 			 if (SMB0CN0_ACK)               // Slave Address or Data Byte Acknowledged
			 {
				 if (_i2c_master_struct.startFlag)
				{
				   SMB0CN0_STA = 1;
				   _i2c_master_struct.startFlag = 0;
				   break;
				}

				if(_i2c_master_struct.offAddrFlag)       // Are we sending the dev address?
				{
				   if( _i2c_master_struct.widthFlag == 0x0)  //indicate offsetAddr width is 8bits
				   {
					   _i2c_master_struct.offAddrFlag = 0;   				  // Clear flag
					   SMB0DAT = U16_GET_LSB(_i2c_master_struct.offAddr);     // Send offsetAdd low byte

					   if (_i2c_master_struct.rwChangeFlag)
					   {
						  _i2c_master_struct.startFlag = 1;      //because baseAddr and offsetAddr send end, next circulate send a new start signal
						  _i2c_master_struct.rwFlag    = I2C_READ;
					   }
					   break;
				   }
				   else  //indicate offsetAddr width > 8bits
				   {
					   _i2c_master_struct.cnt ++;

					   if(_i2c_master_struct.cnt == 1)
					   {
						   SMB0DAT = U16_GET_MSB(_i2c_master_struct.offAddr);    // Send offsetAddr high byte
					   }
					   else if(_i2c_master_struct.cnt == 2)
					   {
						   _i2c_master_struct.cnt 		  = 0;
						   _i2c_master_struct.offAddrFlag = 0;       // Clear flag

						   SMB0DAT = U16_GET_LSB(_i2c_master_struct.offAddr);    // Send offsetAddr low byte

						   if (_i2c_master_struct.rwChangeFlag)
						   {
							   _i2c_master_struct.startFlag = 1;          // Send a START after the next SMB0CN_ACK cycle,因为已经把baseaddr和offsetaddr发送结束了，所以下个循环重新发送一个Start信号
							  _i2c_master_struct.rwFlag     = I2C_READ;
						   }
					   }
					   break;
				   }
				}

				if (_i2c_master_struct.rwFlag == I2C_WRITE)         // Is this transfer a I2C_WRITE?
				{
				   if (i < _i2c_master_struct.len)   // Is there data to send?
				   {
					  // send data byte
					  SMB0DAT = *(_i2c_master_struct.buff);
					  _i2c_master_struct.buff++;
					  i++;
				   }
				   else        			  // This is the last byte
				   {
					  SMB0CN0_STO = 1;      // Set SMB0CN_STO to terminte transfer
					  _i2c_master_struct.busyFlag = 0;         // Clear software busyFlag flag
				   }
				}
				else {}                    // If this transfer is a I2C_READ, do nothing

			 }
			 else // If slave NACK,
			 {
				if(_i2c_master_struct.ackPollFlag)
				{
				   SMB0CN0_STA = 1;        // Restart transfer
				   //由于在i2c通信的过程中，对方设备正在处理其他的事情，可能不能及时回i2c的ack，从而导致通信失败，为了保持通信的正确性，这里应该反复请求,如果只
				   //只访问50次，实验证明，会存在大量的地址写入失败,但是这里又不能一直等待，这样会造成程序卡死，然后不能依次读取基地址,解决方案这里一直去请求，在读写函数里
				   //去做判断，如果超过3s没有回复，即将SMB0CN_STA = 0
				}
				else
				{
					fail = 1;               // Indicate failed transfer
				}                          // and handle at end of ISR
			 }
			 break;

		  // Master Receiver: byte received
		  case SMB_MRDB:
			 if (++i < _i2c_master_struct.len)        // Is there any data remaining?
			 {
				*(_i2c_master_struct.buff) = SMB0DAT;   // Store received byte
				_i2c_master_struct.buff++;              // Increment data in pointer
				SMB0CN0_ACK = 1;           				// Set SMB0CN_ACK bit (may be cleared later in the code)
			 }
			 else // This is the last byte
			 {
				*_i2c_master_struct.buff    = SMB0DAT;   // Store received byte
				_i2c_master_struct.busyFlag = 0;         // Free SMBus interface
				SMB0CN0_ACK = 0;           // Send NACK to indicate last byte of this transfer
				SMB0CN0_STO = 1;           // Send STOP to terminate transfer
			 }
			 break;
		  default:
			  fail = 1;                     // Indicate failed transfer  and handle at end of ISR;
			 break;
	   }
   }
   else
   {
	   fail = 1;                     // Indicate failed transfer  and handle at end of ISR;
   }

   if (fail)                           // If the transfer failed,
   {
	  i2c_master_hw_reset();
	  fail = 0;
   }
	 SMB0CN0_SI = 0;                             // Clear interrupt flag
}

