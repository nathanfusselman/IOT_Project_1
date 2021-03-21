// IP Library
// Nathan Fusselman and Deborah Jahaj

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL w/ ENC28J60
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// ENC28J60 Ethernet controller on SPI0
//   MOSI (SSI0Tx) on PA5
//   MISO (SSI0Rx) on PA4
//   SCLK (SSI0Clk) on PA2
//   ~CS (SW controlled) on PA3
//   WOL on PB3
//   INT on PC6

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#ifndef ARP_H_
#define ARP_H_

#include "NETWORK/eth0.h"
#include <stdint.h>
#include <stdbool.h>

bool etherIsArpRequest(etherHeader *ether);
bool etherIsArpResponse(etherHeader *ether);
void etherSendArpResponse(etherHeader *ether);
void etherSendArpRequest(etherHeader *ether, uint8_t ip[]);
uint8_t* etherParseArpResponse(etherHeader *ether);


#endif
