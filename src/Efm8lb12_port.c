#include "Efm8lb12_port.h"
#include "mcu_sal.h"

static void port0_init();
static void port1_init();
static void port2_init();
static void port3_init();
static void port_crossbar_xbr0();
static void port_crossbar_xbr1();
static void port_crossbar_xbr2();

volatile bool intl_flag = false;

void port_match_init()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE   = 0x10;
	EIP1     &= ~(EIP1_PMAT__HIGH);  	//set port match interrupt the lowest pri
	EIP1H    &= ~(EIP1H_PHMAT__HIGH);
	EIE1     &= ~EIE1_EMAT__ENABLED;    //disable interrupt requests generated by a Port Match
	SFRPAGE   = SFRPAGE_SAVE;
}

void port_match_all_interrupt_enable(uint8_t enable)
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE  = PG2_PAGE;

	if (enable)
	{
		EIE1  |= EIE1_EMAT__ENABLED;    //Enable interrupt requests generated by a Port Match
	}
	else
	{
		EIE1  &= ~EIE1_EMAT__BMASK;     //Disable interrupt requests generated by a Port Match
	}

	SFRPAGE = SFRPAGE_SAVE;
	return;
}

void port_init()
{
	port_crossbar_xbr2();
	port0_init();
	port1_init();
	port2_init();
	port3_init();
	port_crossbar_xbr0();
	port_crossbar_xbr1();
}

void port_mido_output(uint8_t mod)
{
	if(mod == TYPE_MDC)
	{
		P1MDOUT |= P1MDOUT_B2__PUSH_PULL;
	}
	else if (mod == TYPE_MDIO)
	{
		P1MDOUT |= P1MDOUT_B3__PUSH_PULL;
	}
}

void port_mido_input(uint8_t mod)
{
	if(mod == TYPE_MDC)
	{
		P1 |= P1_B2__HIGH;
		P1MDOUT &= ~P1MDOUT_B2__PUSH_PULL;
	}
	else if (mod == TYPE_MDIO)
	{
		P1 |= P1_B3__HIGH;
		P1MDOUT &= ~P1MDOUT_B3__PUSH_PULL;
	}
}

static void port_crossbar_xbr0()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x0;

	XBR0 = XBR0_URT0E__DISABLED | XBR0_SPI0E__DISABLED  //disable UART0 I/O
		 | XBR0_SMB0E__DISABLED | XBR0_CP0E__DISABLED   //disable SMB0 I/O
		 | XBR0_SMB0E__DISABLED | XBR0_CP0E__DISABLED
		 | XBR0_CP0AE__DISABLED | XBR0_CP1E__DISABLED
		 | XBR0_CP1AE__DISABLED | XBR0_SYSCKE__DISABLED;

	SFRPAGE = SFRPAGE_SAVE;
}

static void port_crossbar_xbr2()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x0;

	XBR2 = XBR2_URT1E__DISABLED    			//disable UART1 I/O
		| XBR2_URT1RTSE__DISABLED  			//disable UART1 RTS Output
		| XBR2_URT1CTSE__DISABLED  			//disable UART1 CTS Input
		| XBR2_XBARE__ENABLED	   			//Crossbar Enable
		| XBR2_WEAKPUD__PULL_UPS_ENABLED;  //Weak Pullups enable

	SFRPAGE = SFRPAGE_SAVE;
}

static void port_crossbar_xbr1()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x0;

	XBR1 = XBR1_PCA0ME__DISABLED  //disable PCA Module I/O
		| XBR1_ECIE__DISABLED     //disable PCA0 External Counter Input
		| XBR1_T0E__DISABLED	  //disable T0
		| XBR1_T1E__DISABLED      //disable T1
		| XBR1_T2E__DISABLED;     //disable T2

	SFRPAGE = SFRPAGE_SAVE;
}

static void port0_init()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x0;
	//1.set port default value
	P0 = P0_B0__HIGH | P0_B1__HIGH | P0_B2__HIGH | P0_B3__HIGH | P0_B4__HIGH | P0_B5__HIGH | P0_B6__HIGH | P0_B7__HIGH;

	P0MDIN = P0MDIN_B0__DIGITAL
			| P0MDIN_B1__DIGITAL	| P0MDIN_B2__DIGITAL  //set p0.1 ~ p0.2  analog mode is used for modprs pin and modsel pin
			| P0MDIN_B3__DIGITAL    //set p0.3 digital mode is used for reset pin
			| P0MDIN_B4__DIGITAL | P0MDIN_B5__DIGITAL
			| P0MDIN_B6__DIGITAL    //set p0.6  analog mode is used for lpmode pin
			| P0MDIN_B7__DIGITAL;   //set p0.7  digital mode is used for reset_auto pin

	//3. Select the output mode (open-drain or push-pull) for all port pins, using the Port Output Mode register (PnMDOUT).
	P0MDOUT = P0MDOUT_B0__OPEN_DRAIN
			| P0MDOUT_B1__OPEN_DRAIN     //because p0.1 is input pin,so set p0.1 default value(open drain mode)  is used for modprs
			| P0MDOUT_B2__OPEN_DRAIN	     //because p0.2 is input pin,so set p0.2 default value(open drain mode)  is used for modsell
			| P0MDOUT_B3__OPEN_DRAIN     //because p0.3 is input pin,so set p0.3 default value(open drain mode)  is used for reset pin
			| P0MDOUT_B4__OPEN_DRAIN | P0MDOUT_B5__OPEN_DRAIN
			| P0MDOUT_B6__OPEN_DRAIN     //because p0.6 is input pin,so set p0.6 default value(open drain mode)  is used for lpmode pin
			| P0MDOUT_B7__OPEN_DRAIN;     //set p0.7 push pull is used for reset_auto pin,because there is Level conflict ,so modify OPEN_DRAIN

	//4. Select any pins to be skipped by the I/O crossbar using the Port Skip registers (PnSKIP).
	P0SKIP = P0SKIP_B0__SKIPPED   //because P1.2 and p1.3 are used to Smbus,so P0.0 ~ P0.7 must be skipped
			| P0SKIP_B1__SKIPPED| P0SKIP_B2__SKIPPED
			| P0SKIP_B3__SKIPPED
			| P0SKIP_B4__NOT_SKIPPED | P0SKIP_B5__NOT_SKIPPED
			| P0SKIP_B6__SKIPPED
			| P0SKIP_B7__SKIPPED;

	//5. set p0MASK
	P0MASK = P0MASK_B0__IGNORED | P0MASK_B1__IGNORED | P0MASK_B2__IGNORED
			| P0MASK_B3__IGNORED | P0MASK_B4__IGNORED | P0MASK_B5__IGNORED
			| P0MASK_B6__IGNORED | P0MASK_B7__IGNORED;

	//6. set P0MAT
	P0MAT = P0MAT_B0__HIGH | P0MAT_B1__HIGH | P0MAT_B2__HIGH | P0MAT_B3__HIGH | P0MAT_B4__HIGH | P0MAT_B5__HIGH | P0MAT_B6__HIGH | P0MAT_B7__HIGH;

	SFRPAGE = SFRPAGE_SAVE;

}


static void port1_init()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x0;
	//1.set port default value
	P1 = P1_B0__HIGH | P1_B1__HIGH | P1_B2__HIGH | P1_B3__HIGH | P1_B4__LOW | P1_B5__HIGH | P1_B6__HIGH | P1_B7__HIGH;

	//2.Select the input mode (analog or digital) for all port pins, using the Port Input Mode register (PnMDIN).
	P1MDIN = P1MDIN_B0__DIGITAL
			| P1MDIN_B1__DIGITAL
			| P1MDIN_B2__DIGITAL
			| P1MDIN_B3__DIGITAL
			| P1MDIN_B4__DIGITAL
			| P1MDIN_B5__DIGITAL
			| P1MDIN_B6__DIGITAL
			| P1MDIN_B7__DIGITAL;

	//3. Select the output mode (open-drain or push-pull) for all port pins, using the Port Output Mode register (PnMDOUT).
	P1MDOUT = P1MDOUT_B0__OPEN_DRAIN  //set p1.0 push pull is used for lpmode_auto pin,because there is Level conflict ,so modify OPEN_DRAIN
			| P1MDOUT_B1__OPEN_DRAIN
			| P1MDOUT_B2__OPEN_DRAIN | P1MDOUT_B3__OPEN_DRAIN //set p1.2~ p1.3 open drain mode is used for modsel pin and reset pin
			| P1MDOUT_B4__PUSH_PULL | P1MDOUT_B5__PUSH_PULL  | P1MDOUT_B6__OPEN_DRAIN	| P1MDOUT_B7__OPEN_DRAIN;

	//4. Select any pins to be skipped by the I/O crossbar using the Port Skip registers (PnSKIP).
	P1SKIP = P1SKIP_B0__SKIPPED
			| P1SKIP_B1__SKIPPED			//set p1.1 skip in order to p1.2 and p1.3 are used for i2c master mode
			| P1SKIP_B2__NOT_SKIPPED | P1SKIP_B3__NOT_SKIPPED
			| P1SKIP_B4__NOT_SKIPPED | P1SKIP_B5__NOT_SKIPPED| P1SKIP_B6__NOT_SKIPPED | P1SKIP_B7__NOT_SKIPPED;

	//5. set p1MASK
	P1MASK = P1MASK_B0__IGNORED | P1MASK_B1__IGNORED | P1MASK_B2__IGNORED
			| P1MASK_B3__IGNORED | P1MASK_B4__IGNORED | P1MASK_B5__IGNORED
			| P1MASK_B6__COMPARED | P1MASK_B7__IGNORED;

	//6. set P1MAT
	P1MAT = P1MAT_B0__HIGH | P1MAT_B1__HIGH | P1MAT_B2__HIGH | P1MAT_B3__HIGH
			| P1MAT_B4__HIGH | P1MAT_B5__HIGH | P1MAT_B6__LOW | P1MAT_B7__HIGH;

	SFRPAGE = SFRPAGE_SAVE;
}

static void port2_init()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x20;
	//1.set port default value
	P2 = P2_B0__LOW | P2_B1__HIGH | P2_B2__HIGH | P2_B3__HIGH | P2_B4__HIGH | P2_B5__HIGH | P2_B6__HIGH ;

	//2.Select the input mode (analog or digital) for all port pins, using the Port Input Mode register (PnMDIN).
	P2MDIN = P2MDIN_B0__DIGITAL | P2MDIN_B1__DIGITAL | P2MDIN_B2__DIGITAL | P2MDIN_B3__DIGITAL | P2MDIN_B4__DIGITAL | P2MDIN_B5__DIGITAL | P2MDIN_B6__DIGITAL ;
	//3. Select the output mode (open-drain or push-pull) for all port pins, using the Port Output Mode register (PnMDOUT).

	P2MDOUT = P2MDOUT_B0__PUSH_PULL | P2MDOUT_B1__OPEN_DRAIN | P2MDOUT_B2__OPEN_DRAIN | P2MDOUT_B3__OPEN_DRAIN | P2MDOUT_B4__OPEN_DRAIN | P2MDOUT_B5__OPEN_DRAIN | P2MDOUT_B6__OPEN_DRAIN;

	//4. Select any pins to be skipped by the I/O crossbar using the Port Skip registers (PnSKIP).
	P2SKIP = P2SKIP_B0__NOT_SKIPPED | P2SKIP_B1__NOT_SKIPPED
			| P2SKIP_B2__NOT_SKIPPED | P2SKIP_B3__SKIPPED | 0x10 | 0x20 | 0x40;

	//5. set p2MASK
	P2MASK = P2MASK_B0__IGNORED | P2MASK_B1__IGNORED | P2MASK_B2__IGNORED
			| P2MASK_B3__IGNORED | P2MASK_B4__IGNORED | P2MASK_B5__IGNORED
			| P2MASK_B6__IGNORED ;

	//6. set P2MAT
	P2MAT = P2MAT_B0__HIGH | P2MAT_B1__HIGH | P2MAT_B2__HIGH | P2MAT_B3__HIGH | P2MAT_B4__HIGH | P2MAT_B5__HIGH | P2MAT_B6__HIGH ;

	SFRPAGE = SFRPAGE_SAVE;
}

static void port3_init()
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE = 0x20;
	//1.set port default value
	P3 = P3_B0__HIGH | P3_B1__HIGH | P3_B2__HIGH | P3_B3__HIGH | P3_B4__HIGH;

	//2.Select the input mode (analog or digital) for all port pins, using the Port Input Mode register (PnMDIN).
	P3MDIN = P3MDIN_B0__DIGITAL

			| P3MDIN_B1__DIGITAL

			| P3MDIN_B2__DIGITAL

			| P3MDIN_B3__DIGITAL

			| P3MDIN_B4__DIGITAL;

	//3. Select the output mode (open-drain or push-pull) for all port pins, using the Port Output Mode register (PnMDOUT).
	P3MDOUT = P3MDOUT_B0__OPEN_DRAIN

			| P3MDOUT_B1__OPEN_DRAIN

			| P3MDOUT_B2__OPEN_DRAIN

			| P3MDOUT_B3__OPEN_DRAIN

			| P3MDOUT_B4__OPEN_DRAIN;  //because p3.4 is input pin,so set p3.4 default value(open drain mode)  is used for intl pin

	//4. Select any pins to be skipped by the I/O crossbar using the Port Skip registers (PnSKIP).

	//p3 do't have p3SKIP registers

	//5. set p2MASK
	//p3 do't have P3MASK registers

	//6. set P2MAT
	//p3 do't have P3MAT registers
	SFRPAGE = SFRPAGE_SAVE;

}

static void port_match_reverse(uint8_t port, uint8_t pin)
{
	uint8_t SFRPAGE_SAVE = SFRPAGE;

	SFRPAGE  = PG3_PAGE;

	if(port == 0)
	{
		if (P0MAT & (1 << pin))
			P0MAT &= ~(1 << pin);
		else
			P0MAT |= (1 << pin);
	}
	else if(port == 1)
	{
		if (P1MAT & (1 << pin))
			P1MAT &= ~(1 << pin);
		else
			P1MAT |= (1 << pin);
	}
	else if(port == 2)
	{
		if (P1MAT & (1 << pin))
			P1MAT &= ~(1 << pin);
		else
			P1MAT |= (1 << pin);
	}
	SFRPAGE = SFRPAGE_SAVE;
	return;
}

SI_INTERRUPT (PMATCH_ISR, PMATCH_IRQn)
{
	intl_flag = true;
	port_match_reverse(1,6);
}

