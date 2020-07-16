
//-----------------------------------------------------------------------------
// Includes
//-----------------------------------------------------------------------------
#include "mcu_sal.h"
#include "Efm8lb12_cmd.h"
#include "Efm8lb12_port.h"

//-----------------------------------------------------------------------------
// main() Routine
// ----------------------------------------------------------------------------
//
// Note: the software watchdog timer is not disabled by default in this
// example, so a long-running program will reset periodically unless
// the timer is disabled or your program periodically writes to it.
//
// Review the "Watchdog Timer" section under the part family's datasheet
// for details. To find the datasheet, select your part in the
// Simplicity Launcher and click on "Data Sheet".
//
//-----------------------------------------------------------------------------
static void mcu_sysclk_init (void)
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;
	// $[HFOSC1 Setup]
	// Ensure SYSCLK is > 24 MHz before switching to HFOSC1

	//if SYSCLK > 50MHZ,must set PFE0CN register
	SFRPAGE = 0x10;
	PFE0CN = PFE0CN_FLRT__SYSCLK_BELOW_75_MHZ ;

	SFRPAGE = 0x00;
	CLKSEL = CLKSEL_CLKSL__HFOSC0 | CLKSEL_CLKDIV__SYSCLK_DIV_1;
	CLKSEL = CLKSEL_CLKSL__HFOSC0 | CLKSEL_CLKDIV__SYSCLK_DIV_1;
	while (CLKSEL & CLKSEL_DIVRDY__BMASK == CLKSEL_DIVRDY__NOT_READY);

	CLKSEL = CLKSEL_CLKSL__HFOSC1 | CLKSEL_CLKDIV__SYSCLK_DIV_1;
	CLKSEL = CLKSEL_CLKSL__HFOSC1 | CLKSEL_CLKDIV__SYSCLK_DIV_1;
	while (CLKSEL & CLKSEL_DIVRDY__BMASK == CLKSEL_DIVRDY__NOT_READY);
	SFRPAGE  = SFRPAGE_SAVE;
}


void test1()
{
	//printf("\r\n TEST1");
}

void test2()
{
	//printf("\r\n TEST2");
}

void test3()
{
	//printf("\r\n TEST3");
}

void test4()
{
	//printf("\r\n TEST4");
}


int main (void)
{
	static TASK_STRUCT_T 	test1_task;
	static TASK_STRUCT_T 	test2_task;
	static TASK_STRUCT_T 	test3_task;
	static TASK_STRUCT_T 	test4_task;
	static TASK_STRUCT_T 	terminal_task;
	static TASK_STRUCT_T 	timer_task;

	IE_EA = 0;  			 	//Disable global interrupts

	timer_hw_wdt_disable();	 	 //Disable WatchDog

	timer_hw_wdt_init(7);		 //Set WatchDog timer to 7s

	mcu_sysclk_init();  	 	//Sysclk Init

	port_init();			 	//Port Init

	mcu_sal_init();

	cmd_init();

	IE_EA = 1;      		//Enable global interrupts

	printf("%s","\r\nPeripheral init OK");


	printf("%s","\r\nTestBoard is ready");
	printf("%s","\r\n#McuTestBoard:");


	test1_task.func = test1;
	test1_task.id   = TASK_ID_TESE1;
	test1_task.pri  = PRI0;

	test4_task.func = test4;
	test4_task.id   = TASK_ID_TESE4;
	test4_task.pri  = PRI11;

	test3_task.func = test3;
	test3_task.id   = TASK_ID_TESE3;
	test3_task.pri  = PRI8;

	test2_task.func = test2;
	test2_task.id   = TASK_ID_TESE2;
	test2_task.pri  = PRI5;

	terminal_task.func = terminal_event_handle;
	terminal_task.id   = TASK_ID_TERMINAL_HANDLE;
	terminal_task.pri  = PRI15;

	timer_task.func = timer_run;
	timer_task.id   = TASK_ID_TIMER_TASK;
	timer_task.pri  = PRI14;

	task_add(&test3_task);
	task_add(&test2_task);
	task_add(&test4_task);
	task_add(&test1_task);
	task_add(&terminal_task);
	task_add(&timer_task);

	while(1)
	{
		task_run();
	}
}
