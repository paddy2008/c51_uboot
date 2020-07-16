#include "mcu_sal.h"

volatile uint8_t  busy = 0;	   //because busy is used by UART0 interrupt handle function and main loop,declare variable by "volatile" keywords

static void _uart_clock_select(void)  //Timer1 is used for uart0 clock source
{
	lib_uart_timer_init(TH1); 				    // initial Timer1
}

static void _uart_dev_init (void)
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x0;

	XBR0 |= XBR0_URT0E__ENABLED; 		//enable UART0 I/O

	SCON0 	 |= 0x10;                  //Mode0:8-bit UART with Variable Baud Rate(bit7);
									   //Logic level of stop bit is ignored(bit5);
									   //Receive Enable(bit4);
									   //Clear Transmit Interrupt Flag(bit1);Clear Receive Interrupt Flag(bit0)

	#if (SYSCLK/UART_BAUDRATE/2/256 < 1)
	{
		TH1 = 256-(SYSCLK/UART_BAUDRATE/2);
		CKCON0 &= ~0x0B;                  			// T1M = 1; SCA1:0 = xx
		CKCON0 |=  0x08;								//Timer1 use the system clock
	}
	#elif (SYSCLK/UART_BAUDRATE/2/256 < 4)
	{
		TH1 = 256-(SYSCLK/UART_BAUDRATE/2/4);
		CKCON0 &= ~0x0B;                  			// T1M = 0; SCA1:0 = 01
		CKCON0 |=  0x01;								//Timer1 user System clock divided by 4
	}
	#elif (SYSCLK/UART_BAUDRATE/2/256 < 12)
	{
		TH1 = 256-(SYSCLK/UART_BAUDRATE/2/12);
		CKCON0 &= ~0x0B;                  			// T1M = 0; SCA1:0 = 00,if SYSCLK = 24.5MHZ,UART_BAUDRATE = 9600 kbsp,so choose this
		CKCON0 |= 0x00;								//Timer1 user System clock divided by 12
	}
	#elif
	{
		TH1 = 256-(SYSCLK/UART_BAUDRATE/2/48);
		CKCON0 &= ~0x0B;                  			// T1M = 0; SCA1:0 = 10
		CKCON0 |=  0x02;								//Timer1 user System clock divided by 48
	}
	#endif

	SCON0_TI  = 0;                      // Indicate TX0 ready

	IP       |= 0x10;				    // Make UART high priority3,priority3 is the highest priority!

	SFRPAGE   = 0x10;
	IPH      |= 0x10;
	IE       |= 0x10;                   // Enable UART0 interrupts

	SFRPAGE = SFRPAGE_SAVE;
}

void uart_init(void)
{
	_uart_dev_init();
	_uart_clock_select();
}


void uart_putchar(char ch)
{
	if (MY_GET_BIT(XBR0, 0) == 0)  //if uart is disable
	{
		return;
	}

	while(busy);
	busy = 1;
	SBUF0 = ch;
}

/*
 * 因为在初始化的过程中并且全局中断未打开之前，有可能需要打印信息
 * 例如检测SDA和SCL的电平状态等等，所以这里在输出信息之前，先将
 * 全局中断打开
 */
void uart_put_string(uint8_t *str)
{
	bit EA_SAVE = IE_EA;

	IE_EA = 1;
	if (str == NULL)
		return ;

	while(*str)
	{
		uart_putchar(*str);
		str++;
	}
	IE_EA = EA_SAVE;
}

/*
 * 因为在初始化的过程中并且全局中断未打开之前，有可能需要打印信息
 * 例如检测SDA和SCL的电平状态等等，所以这里在输出信息之前，先将
 * 全局中断打开
 */
void uart_printf(uint8_t *fmt,...)
{
    uint8_t string[255];
    va_list ap;

    va_start(ap,fmt); 			//获得第一个实际参数的起始地址
    vsprintf(string,fmt,ap);	//其实这个函数才是核心函数
    uart_put_string(string);
    va_end(ap);     			//将ap赋值为NULL
}


SI_INTERRUPT(UART0_ISR, UART0_IRQn)
{
	if (SCON0_RI == 1)  		 // Receive
	{
		uint8_t recv = SBUF0;
		terminal_recv_chars(&recv, 1);
		SCON0_RI = 0;  			 // Clear interrupt flag
	}

	if(SCON0_TI == 1)
	{
		SCON0_TI  = 0;
		busy = 0;
	}
}
