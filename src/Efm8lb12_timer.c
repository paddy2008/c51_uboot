#include "mcu_sal.h"

/*
 * Timer0 is used for i2c master clock source
 */
void timer0_hw_init(uint16_t clock)  //the uint of the clock is khz
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;
	uint32_t ClockSpeed = clock;

	SFRPAGE    = 0x0;
    clock      = SYSCLK/3/4/(ClockSpeed * 1000);  //ClockSpeed * 1000乘法运算的结果是保存到ClockSpeed这个变量的，所以这里需要先将clock赋值为ClockSpeed

    TMOD     |= 0x02;						 // Sets to 8 bit mode,it is mode2 ,the TL1 is  responsible for counter,TH1 is responsible for Initial value in mode2
    CKCON0   |= 0x01;						 // Use system clock divided by 4,if it is not setted ,default system clock value is divided by 12
    TH0       = -clock; 					 //SMB_FREQUENCY smbus 的时钟为200khz,根据200Khz算出来的应该是253,因为SCL的时钟频率是定时器溢出率的1/3,所以定时器溢出的频率必须是600khz
	TL0       = TH0;						 // Reload the initial value into TL1
	IE_ET0    = 0x0;                 		 // disable Timer 0 interrupt(TF0),this bit is the 第1 bit of IE register,IE page addr is all
	TCON_TR0  = 0x1;						 // Enable Timer0 ,this bit is the 第4 bit of TCON register
    SFRPAGE = SFRPAGE_SAVE;
}

/*
 *Timer1 is used for uart0 clock source
 */
void timer1_hw_init(uint16_t  counter)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE   = 0x0;

	TMOD 	 &= ~0xf0;
	TMOD     |= 0x20;   				// Sets to 8 bit mode,it is mode2 ,the TL1 is  responsible for counter,TH1 is responsible for Initial value in mode2
	TL1       = counter;                // Initial Value
	TH1       = TL1;					// Reload the initial value into TL1
	IE_ET1    = 0x0;                    // Disable Timer 1 interrupt(TF1)
	TCON     |= 0x40;   			    // Enable Timer1
	SFRPAGE = SFRPAGE_SAVE;
}

/*
 * Timer2 for use delay and polling
 */
static void _timer2_hw_init(void)
{
    uint8_t SFRPAGE_SAVE = SFRPAGE;
	SFRPAGE   = 0x0;

	TMR2CN0 &= ~0x04;					//disable timer2
	TMR2CN0 &= ~0x08;					//set timer2 into 16 bit modes

	CKCON0 &= ~0x30;					//Use system clock divided by 12
	TMR2CN0 &= ~0x03;

	TMR2RL = -(SYSCLK/12/1000);         // Reload value to be used in Timer2
	TMR2 = TMR2RL;                      // Init the Timer2 register,Every 1 milliseconds（1ms >>> 3*(1/18000000)） overflow a time

	TMR2CN0 |= 0x04;                    // Enable Timer2 in auto-reload mode
	IE_ET2 = 1;                         // Timer2 interrupt enable
	SFRPAGE = SFRPAGE_SAVE;
}

static void _timer5_hw_init(void)
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	/*1s    counter:72000000/12*/
	/*1ms   counter:72000/12*/
	/*1us   counter:72/12 = 6*/

	SFRPAGE  = 0x10;

	TMR5CN0_TF5H = 0;   	// Clear Timer5 overflow flag

	TMR5CN0 &= ~0x04;    	//disable timer5
	TMR5CN0 &= ~0x08; 		//set timer5 into 16 bit modes

	CKCON1  &= ~0x0C;		//set timer5 clock is system clock
	TMR5CN0 &= ~0x03;       //clock divided by 12

	TMR5     = 100;     	//set timer5 init value
	TMR5RL   = TMR5;        //set timer5 reload value

	TMR5CN0 |= 0x04;          // Enable Timer5
	//EIE2  |= 0x08;          // Timer5 interrupt enable

	SFRPAGE = SFRPAGE_SAVE;
}

/*
 * 如果要精确us级别的延时，要么延时函数中其他指令的时间也是需要考虑进来的
 * 该函数的汇编代码如下:
 *  _timer5_delay_us:
*0000a82b:   MOV     R0, #22H           --->2    	==>4
*0000a82d:   MOV     @R0, 0A7H			--->2
*100       	idata uint16_t counter = 0;
*0000a82f:   CLR     A					--->1    	==>7
*0000a830:   INC     R0					--->1
*0000a831:   MOV     @R0, A				--->2
*0000a832:   INC     R0					--->1
*0000a833:   MOV     @R0, A				--->2
*106       	if(us == 1)
*0000a834:   MOV     A, R7				--->2        ==> 13
*0000a835:   XRL     A, #01H			--->2
*0000a837:   ORL     A, R6				--->1
*0000a838:   JNZ     0A861H				--->8
*108       		delay_38_nop();
*0000a83a:   NOP
*0000a83b:   NOP
*0000a83c:   NOP
*0000a83d:   NOP
*0000a83e:   NOP
*0000a83f:   NOP
*0000a840:   NOP
*0000a841:   NOP
*0000a842:   NOP
*0000a843:   NOP
*0000a844:   NOP
*0000a845:   NOP
*0000a846:   NOP
*0000a847:   NOP
*0000a848:   NOP
*0000a849:   NOP
*0000a84a:   NOP
*0000a84b:   NOP
*0000a84c:   NOP
*0000a84d:   NOP
*0000a84e:   NOP
*0000a84f:   NOP
*0000a850:   NOP
*0000a851:   NOP
*0000a852:   NOP
*0000a853:   NOP
*0000a854:   NOP
*0000a855:   NOP
*0000a856:   NOP
*0000a857:   NOP
*0000a858:   NOP
*0000a859:   NOP
*0000a85a:   NOP
*0000a85b:   NOP
*0000a85c:   NOP
*0000a85d:   NOP
*0000a85e:   NOP
*0000a85f:   NOP
*0000a860:   RET
*112       	counter = (65535) - us * 6 ;
*0000a861:   MOV     R4, #00H			--->2   ==>33
*0000a863:   MOV     R5, #06H			--->2
*0000a865:   LCALL   ?C?IMUL(1A85H)		--->9
*0000a868:   CLR     C					--->1
*0000a869:   MOV     A, #0FFH			--->2
*0000a86b:   SUBB    A, R7				--->1
*0000a86c:   MOV     R7, A				--->2
*0000a86d:   MOV     A, #0FFH			--->2
*0000a86f:   SUBB    A, R6				--->1
*0000a870:   MOV     R6, A				--->2
*0000a871:   MOV     R0, #23H			--->2
*0000a873:   MOV     @R0, A				--->2
*0000a874:   INC     R0					--->1
*0000a875:   MOV     A, R7				--->2
*0000a876:   MOV     @R0, A				--->2
*114       	SFRPAGE  = 0x10;
*0000a877:   MOV     0A7H, #10H			--->2     ==>11
*116       	IE_ET2 = 0;
*0000a87a:   CLR     IE.5				--->1
*118       	TMR5     = counter;
*0000a87c:   MOV     0D5H, R6			--->2
*0000a87e:   MOV     0D4H, A			--->2
*119       	TMR5RL   = TMR5;
*0000a880:   MOV     0D3H, 0D5H			--->2
*0000a883:   MOV     0D2H, 0D4H			--->2
*121       	while(TMR5CN0_TF5H != 0x1);
*0000a886:   JNB     SMBxCN.7, 0A886H	--->9    ==>11
*123       	TMR5CN0_TF5H = 0;
*0000a889:   CLR     SMBxCN.7			--->1
*124       	IE_ET2 = 1;
*0000a88b:   SETB    IE.5				--->1
*126       	SFRPAGE = SFRPAGE_SAVE;
*0000a88d:   MOV     R0, #22H			--->2    ==> 16
*0000a88f:   MOV     A, @R0				--->2
*0000a890:   MOV     0A7H, A			--->2
*127       }
*0000a892:   RET						--->10
 *  整个函数执行完成需要95个NOP周期，因为定时器加1需要12NOP周期，所以设置定时器的初值时需要
 *  在原来的基础上再加上95个NOP的周期，12 * 8 约等于95个NOP周期
 *  另外，当us = 1时，counter = (65535 - us * 6) + 8 得到的结果是大于65535的，
 *  所以当us = 1时，需要特别处理，因为指令
 *  idata uint8_t SFRPAGE_SAVE = SFRPAGE;
 *	idata uint16_t counter = 0; 共占用11个NOP时间
 *	if(us == 1)占用13个NOP时间，RETURN占用10个NOP时间，这些指令共占用34个NOP周期，
 *	1us共计72个NOP周期，所以还需要执行38个NOP指令
 */
void timer5_hw_delay_us(uint16_t us)
{
	idata uint8_t SFRPAGE_SAVE = SFRPAGE;
	idata uint16_t counter = 0;

	/*1s    counter:72000000/12*/
	/*1ms   counter:72000/12*/
	/*1us   counter:72/12 = 6*/

	if(us == 0)
	{
		return;
	}
	else if(us == 1)
	{
		delay_38_nop();
		return;
	}

	counter = (65535 - us * 6) + 12;

	SFRPAGE  = 0x10;

	IE_ET2 = 0;              // Disable Timer2 interrupt in order to prevent preemptive CPU resources,ensure delay is exact

	TMR5     = counter;     //set timer5 init value
	TMR5RL   = TMR5;        //set timer5 reload value

	TMR5CN0_TF5H = 0;   	 	  // Clear Timer5 overflow flag
	while(TMR5CN0_TF5H != 0x1);   // wait timer5 overflow

	IE_ET2 = 1;              // Enable Timer2 interrupt

	SFRPAGE = SFRPAGE_SAVE;
}

void timer_hw_wdt_init(uint8_t interval)
{
	WDTCN = 0xFF;
	WDTCN &= ~0x80;  //must be logic 0 when setting this interval
	WDTCN = interval;
}

void timer_hw_wdt_disable(void)
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	//disable the watch dog timer
	SFRPAGE = 0x00;
	IE_EA = 0;      			  //disable global interrupts prevent the write operation is interrupt
	WDTCN = 0xDE; //First key
	WDTCN = 0xAD; //Second key
	SFRPAGE  = SFRPAGE_SAVE;
}

void timer_hw_init()
{
	timer0_hw_init(i2c_master_get_freq());
	_timer2_hw_init();
	_timer5_hw_init();
}


SI_INTERRUPT_USING(TIMER3_ISR, TIMER3_IRQn, 3)
{
   SMB0CF  &= ~0x80;                    // Disable SMBus
   SMB0CF  |= 0x80;                     // Re-enable SMBus
   TMR3CN0 &= ~0xC0;                    // Clear Timer3 overflow flag
   SMB0CN0_STA = 0;
}

SI_INTERRUPT (TIMER4_ISR, TIMER4_IRQn)
{
	SFRPAGE = PG3_PAGE;

	I2C0CN0 &= ~I2C0CN0_I2C0EN__BMASK;			// Disable I2C module
	I2C0CN0 |= I2C0CN0_I2C0EN__ENABLED;			// Re-enable I2C module

	SFRPAGE = PG2_PAGE;
	TMR4CN0 &= ~0xC0;							// Clear Timer4 interrupt-pending flag
}

SI_INTERRUPT_USING(TIMER2_ISR, TIMER2_IRQn, 2)
{
	TMR2CN0_TF2H = 0;  // Clear the interrupt flag

	timer_update();
}








