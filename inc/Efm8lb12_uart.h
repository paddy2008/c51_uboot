#ifndef __Efm8lb12_uart_h__
#define __Efm8lb12_uart_h__

#include <SI_EFM8LB1_Register_Enums.h>

#define  UART_BAUDRATE          (115200)      	  	// Baud rate of uart in bps

extern void 	 	  uart_init (void);
extern void 		  uart_putchar(char ch);
extern void 	 	  uart_put_string(uint8_t *str);
extern void 	 	  uart_printf(uint8_t *fmt,...);

#endif

