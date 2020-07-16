#ifndef __Efm8lb12_timer_h__
#define __Efm8lb12_timer_h__

#include "stdint.h"

#define delay_10_nop		{_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
#define delay_38_nop()		{delay_10_nop; delay_10_nop;delay_10_nop;_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}

extern void timer_hw_wdt_disable(void);
extern void timer_hw_wdt_init(uint8_t interval);
extern void timer_hw_init(void);
extern void timer0_hw_init(uint16_t clock);
extern void timer1_hw_init(uint16_t  counter);
extern void timer5_hw_delay_us(uint16_t us);

#endif

