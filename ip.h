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

#ifndef IP_H_
#define IP_H_

#include <eth0.h>
#include <stdint.h>
#include <stdbool.h>

#define IP_HEADER_LENGTH 20
#define IP_TTL 64

void etherBuildIpHeader(etherHeader *ether, uint16_t dataLength, uint8_t *dest_ip);
void etherCalcIpChecksum(etherHeader *ether);

bool etherIsIp(etherHeader *ether);
bool etherIsIpUnicast(etherHeader *ether);


#endif
