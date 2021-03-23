// ICPM Library
// IOT Project #1
// Nathan Fusselman and Deborah Jahaj

#ifndef NETWORK_ICPM_H_
#define NETWORK_ICPM_H_

#include "NETWORK/eth0.h"
#include "NETWORK/tcp.h"
#include "NETWORK/ip.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"

typedef struct _icmpHeader      // 8 bytes
{
  uint8_t type;
  uint8_t code;
  uint16_t check;
  uint16_t id;
  uint16_t seq_no;
  uint8_t data[0];
} icmpHeader;

bool etherIsPingRequest(etherHeader *ether);
void etherSendPingResponse(etherHeader *ether);


#endif /* NETWORK_ICPM_H_ */
