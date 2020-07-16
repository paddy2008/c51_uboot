#ifndef __MCU_SAL_DEF_H__
#define __MCU_SAL_DEF_H__

#include "mcu_sal_hw_conf.h"

typedef enum
{
  RET_OK       			= 0x00U,
  RET_BUSY    			= 0x01U,
  RET_TIMEOUT  			= 0x02U,
  RET_FAILED    		= 0xFFU,
  RET_NO_PERMISSION     = 0xFDU
} RETURN_VALUE_TYPE;

#ifndef NULL
 #define NULL ((void *) 0L)
#endif


#define  MY_SET_BIT(val, bit)		((val)  | (1 << (bit)))
#define  MY_GET_BIT(val, bit)		(((val) >> (bit)) & (0x00000001))
#define  MY_CLEAR_BIT(val, bit)		((val)  & (~(1 << (bit))))

#define  U8_CLEAR_MSB_4BITS(val)	((val)  & (0x0F))
#define  U8_CLEAR_LSB_4BITS(val)	((val)  & (0xF0))
#define  U8_SET_LSB_4BITS(val)		((val)  | (0x0F))
#define  U8_SET_MSB_4BITS(val)		((val)  | (0xF0))
#define  U8_GET_LSB_4BITS(val)		((val)  & (0x0F))
#define  U8_GET_MSB_4BITS(val)		(((val) & (0xF0)) >> 4)
#define  U16_GET_MSB(val) 			(((val) & (0xFF00)) >> 8)
#define  U16_GET_LSB(val)			((val)  & (0xFF))

#define  ASSERT(exp) 				if ((exp) == NULL) return
#define  RE_ASSERT(exp) 			if ((exp) == NULL) return 0
#define  COM_LOCK_SEM(sem)     		((sem)--)
#define  COM_UNLOCK_SEM(sem)   		((sem)++)
#define  COM_GET_LOCK(sem)     		(sem)

#define  JUDGE_RETURN(ret)  		if((ret) != RET_OK) return ret

#define  OFFSET_INDEX(offset)		(offset & 0x7f)


#endif

