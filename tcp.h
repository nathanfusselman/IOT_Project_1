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
    NONE,
    SYN,
    SYN_ACK
} TCP_TYPE;

bool etherOpenTCPConnection(etherHeader *ether, uint8_t dest_addr[], uint8_t dest_ip[], uint16_t source_port, uint16_t dest_port);

TCP_TYPE etherIsTCPPacket(etherHeader *ether);
void etherHandleTCPPacket(etherHeader *ether, TCP_TYPE type);
void etherTCPACK(etherHeader *ether);

void etherCalcTcpChecksum(tcpHeader *tcp, ipHeader *ip);
bool etherCheckTcpChecksum(tcpHeader *tcp, ipHeader *ip);

#endif /* TCP_H_ */
