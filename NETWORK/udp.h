// UDP Library
// IOT Project #1
// Nathan Fusselman and Deborah Jahaj

#ifndef NETWORK_UDP_H_
#define NETWORK_UDP_H_

#include "NETWORK/eth0.h"
#include "NETWORK/tcp.h"
#include "NETWORK/ip.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"

typedef struct _udpHeader       // 8 bytes
{
  uint16_t sourcePort;
  uint16_t destPort;
  uint16_t length;
  uint16_t check;
  uint8_t  data[0];
} udpHeader;

bool etherIsUdp(etherHeader *ether);
uint8_t* etherGetUdpData(etherHeader *ether);
void etherSendUdpResponse(etherHeader *ether, uint8_t* udpData, uint8_t udpSize);

#endif /* NETWORK_UDP_H_ */
