#include "mcu_sal.h"

void flash_hw_erase_page (uint16_t addr)
{
   bit EA_SAVE = IE_EA;                // Preserve IE_EA

   uint8_t xdata * data pwrite;           // Flash write pointer

   uint8_t i = 0;

   IE_EA = 0;                          // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   pwrite = (uint8_t xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x03;                      // PSWE = 1; PSEE = 1

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   *pwrite = 0;                        // Initiate page erase

   PSCTL &= ~0x03;                     // PSWE = 0; PSEE = 0

   IE_EA = EA_SAVE;                    // Restore interrupts
}

void flash_hw_write_byte (uint16_t addr, uint8_t byte)
{
   bit  EA_SAVE = IE_EA;                // Preserve IE_EA
   uint8_t xdata * data pwrite;        // Flash write pointer

   IE_EA = 0;                          // Disable interrupts

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source

   pwrite = (uint8_t xdata *) addr;

   FLKEY  = 0xA5;                      // Key Sequence 1
   FLKEY  = 0xF1;                      // Key Sequence 2
   PSCTL |= 0x01;                      // PSWE = 1 which enables writes

   VDM0CN = 0x80;                      // Enable VDD monitor

   RSTSRC = 0x02;                      // Enable VDD monitor as a reset source
   *pwrite = byte;                     // Write the byte

   PSCTL &= ~0x01;                     // PSWE = 0 which disable writes

   IE_EA = EA_SAVE;                    // Restore interrupts
}


uint8_t flash_hw_read_byte (uint16_t addr)
{
   bit EA_SAVE = IE_EA;                // Preserve IE_EA

   uint8_t code * data pread;          // Flash read pointer
   uint8_t byte;

   IE_EA = 0;                          // Disable interrupts

   pread = (uint8_t code *) addr;

   byte = *pread;                      // Read the byte

   IE_EA = EA_SAVE;                    // Restore interrupts

   return byte;
}
