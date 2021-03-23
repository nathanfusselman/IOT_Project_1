// TCP Library
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

#ifndef TCP_H_
#define TCP_H_

#include "NETWORK/eth0.h"
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

#define TCP_HEADER_LENGTH 20

typedef enum _tcp_state
{
    CLOSED,
    LISTEN,
    SYN_RCVD,
    SYN_SENT,
    ESTABLISHED,
    CLOSE_WAIT,
    LAST_ACK,
    FIN_WAIT_1,
    FIN_WAIT_2,
    CLOSING,
    TIME_WAIT
} TCP_STATE;

typedef enum _tcp_type
{
    NONE = 0x00,
    FIN  = 0x01,
    SYN  = 0x02,
    PSH = 0x08,
    ACK  = 0x10,
    FIN_ACK = 0x11,
    SYN_ACK = 0x12,
    PSH_ACK = 0x18
} TCP_TYPE;

typedef struct _tcpHeader       // 20 or more bytes
{
  uint16_t sourcePort;
  uint16_t destPort;
  uint32_t sequenceNumber;
  uint32_t acknowledgementNumber;
  uint8_t  dataOffset;
  uint8_t  controllBits;
  uint16_t windowSize;
  uint16_t checksum;
  uint16_t urgentPointer;
  uint8_t  data[0];
} tcpHeader;

void etherBuildTcpHeader(etherHeader *ether, TCP_TYPE type);

bool etherOpenTCPConnection(etherHeader *ether, uint8_t dest_addr[], uint8_t dest_ip[], uint16_t dest_port);

void etherHandleTCPPacket(etherHeader *ether);
void etherTcpAck(etherHeader *ether);

void etherCalcTcpChecksum(etherHeader *ether);
bool etherCheckTcpChecksum(etherHeader *ether);

uint32_t etherIncrementSeq(uint32_t num);

#endif /* TCP_H_ */
