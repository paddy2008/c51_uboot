#ifndef __MCU_SAL_MDIO_H__
#define __MCU_SAL_MDIO_H__

#ifdef MCU_MDIO_MODULE_ENABLED

#define MDIO_DELAY       1
#define MDIO_READ_DELAY  1

#define MDIO_CLAUSE22   (22)
#define MDIO_CLAUSE45   (45)

#define MDIO_CLAUSE   		(MDIO_CLAUSE45)

#define delay_10_nop		{_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}
#define delay_72_nop		{delay_10_nop;delay_10_nop; delay_10_nop;delay_10_nop;delay_10_nop;delay_10_nop;delay_10_nop;_nop_();_nop_();}
#define delay_3us		    {delay_72_nop;delay_72_nop;delay_72_nop}
#define delay_4us		    {delay_72_nop;delay_72_nop;delay_72_nop;delay_72_nop}
#define delay_2us		    {delay_72_nop;delay_72_nop}
#define delay_1us		    {delay_72_nop;}

#define delay_100ns			{_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();_nop_();}  //8 * 1 / 72 * 13 = 100ns
#define delay_200ns			{delay_100ns;delay_100ns;}
#define delay_300ns			{delay_100ns;delay_100ns;delay_100ns;}
#define delay_700ns			{delay_100ns;delay_100ns;delay_100ns;delay_100ns;delay_100ns;delay_100ns;delay_100ns;}


typedef enum
{
	TYPE_MDC  = 0x0,
	TYPE_MDIO = 0x1,
}MDIO_MOD;

uint8_t  mdio_init(void);
uint32_t mdio_read(uint32_t phyAddr, uint32_t devAddr, uint32_t regAddr);
void 	 mdio_write(uint32_t phyAddr, uint32_t devAddr, uint16_t regAddr, uint32_t value);

unsigned int mdio_bb_read(unsigned int phy,unsigned int reg);
unsigned int mdio_bb_write(unsigned int phy,unsigned int reg,unsigned int val);

#endif

#endif
