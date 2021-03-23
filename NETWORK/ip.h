// IP Library
// IOT Project #1
// Nathan Fusselman and Deborah Jahaj

//=====================================================================================================

// Hardware Target
//=====================================================================================================


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

//=====================================================================================================

// Device includes, defines, and assembler directives
//=====================================================================================================


#ifndef IP_H_
#define IP_H_

#include "NETWORK/eth0.h"
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

typedef struct _ipHeader        // 20 or more bytes
{
  uint8_t revSize; // 4msb = version, 4lsb = header length
  uint8_t typeOfService;
  uint16_t length;
  uint16_t id;
  uint16_t flagsAndOffset; //3msb = flags, 13lsb = fragment offset
  uint8_t ttl;
  uint8_t protocol;
  uint16_t headerChecksum;
  uint8_t sourceIp[4];
  uint8_t destIp[4];
  uint8_t data[0]; // optional bytes or udp/tcp/icmp header
} ipHeader;

#define IP_HEADER_LENGTH 20
#define IP_TTL 64

void etherBuildIpHeader(etherHeader *ether, uint16_t dataLength, uint8_t *dest_ip);

bool etherIsIp(etherHeader *ether);
bool etherIsIpUnicast(etherHeader *ether);

void etherCalcIpChecksum(etherHeader *ether);

#endif
