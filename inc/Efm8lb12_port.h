#ifndef __100g_sfp_EFM8LB1_port_h_
#define __100g_sfp_EFM8LB1_port_h_

#include <SI_EFM8LB1_Register_Enums.h>

void port_init();
void port_match_init();
void port_mido_output(uint8_t mod);
void port_mido_input(uint8_t mod);
void port_match_all_interrupt_enable(uint8_t enable);


#endif
