#ifndef __MCU_SAL_I2C_SLAVE_H__
#define __MCU_SAL_I2C_SLAVE_H__

#include "mcu_sal_def.h"

#ifdef MCU_I2C_SLAVE_MODULE_ENABLED


#define  I2C_STATUS_VECTOR_MASK  (0x0F)   // NACK, START, STOP, WR, RD
// I2C States
#define  I2C_ADDR_RD    (0x09)			// Valid Slave Address + Master Read Request
#define  I2C_ADDR_WR    (0x0A)			// Valid Slave Address + Master Write Request
#define  I2C_RD_DATA    (0x01)			// Transfer data from Slave (Also can check
                                        // B4 to see what ACK we just received from master)
#define  I2C_WR_DATA    (0x02)			// Write data to Slave (Also can check B4 to
                                        // see what ACK(ACK/NACK) we just sent)
#define  I2C_STO        (0x04)
#define  I2C_STOSTA     (0x0C)
#define  I2C_STOSTARD   (0x0D)
#define  I2C_STOSTAWR   (0x0E)


typedef enum
{
   I2C_SLAVE_BUS_IDLE,
   I2C_SLAVE_BUS_WAITING_TO_PROCESS
} I2C_SLAVE_BUS_STATUS;

typedef enum
{
	I2C_NEXT_UNKNOWN,
	I2C_NEXT_I2CREGADDRESS,
	I2C_NEXT_DATA,
	I2C_NEXT_DATA_NUM,
	I2C_NEXT_CRC,
	I2C_NEXT_ACK,
	I2C_NEXT_NACK,
	I2C_NEXT_FINISH
} I2C_SLAVE_TIME_SERIES_STATUS;

typedef struct
{
	uint8_t         devAddr;
	uint8_t         A2_offsetAddr;
	uint8_t         A0_offsetAddr;
	uint8_t 		A2_sendData;
	uint8_t 		A0_sendData;
	uint8_t         num;
	uint8_t         DataBuff[I2C_WRITE_SIZE_ONCE];
	I2C_SLAVE_BUS_STATUS   			   bus_status;
	I2C_SLAVE_TIME_SERIES_STATUS       time_series_status;
}I2C_SLAVE_STRUCT_T;

extern void i2c_slave_process_devaddr_read(void);
extern void i2c_slave_process_bus_error(void);
extern void i2c_slave_process_devaddr_write(void);
extern void i2c_slave_process_data_read_nack(void);
extern void i2c_slave_process_data_read_ack(void);
extern void i2c_slave_process_data_write(void);
extern void i2c_slave_process_stop(void);
extern void i2c_slave_init (void);
extern void i2c_slave_write_data (void);

#endif


#endif
