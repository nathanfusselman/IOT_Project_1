// TCP Library
// Nathan Fusselman

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

#ifndef TCP_H_
#define TCP_H_

#include <eth0.h>
#include <stdint.h>
#include <stdbool.h>

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
    ACK  = 0x10,
    FIN_ACK = 0x11,
    SYN_ACK = 0x12
} TCP_TYPE;

void etherBuildTcpHeader(etherHeader *ether, uint16_t dataLength, TCP_TYPE type);
bool etherCloseTCPConnection(etherHeader *ether);
bool etherOpenTCPConnection(etherHeader *ether, uint8_t dest_addr[], uint8_t dest_ip[], uint16_t dest_port);

void etherHandleTCPPacket(etherHeader *ether);
void etherTcpAck(etherHeader *ether);

void etherCalcTcpChecksum(etherHeader *ether);
bool etherCheckTcpChecksum(etherHeader *ether);

#endif /* TCP_H_ */
