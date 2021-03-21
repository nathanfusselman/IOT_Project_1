// TCP Library
// Nathan Fusselan

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

#include <tcp.h>
#include <eth0.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ip.h>

#define IP_ADD_LENGTH 4
#define HW_ADD_LENGTH 6

#define TCP_HEADER_LENGTH 20
#define IP_HEADER_LENGTH 20
#define IP_TTL 64

TCP_STATE currentTCPState = CLOSED;
uint16_t source_port = 0, dest_port = 0, id = 0;
uint32_t seq = 0, ack = 0;
uint8_t dest_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
//uint8_t source_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t dest_ip[IP_ADD_LENGTH] = {0,0,0,0};
//uint8_t source_ip[IP_ADD_LENGTH] = {0,0,0,0};

void etherBuildTcpHeader(etherHeader *ether, TCP_TYPE type)
{
    ipHeader *ip = (ipHeader*)ether->data;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + IP_HEADER_LENGTH);

    tcp->sourcePort = htons(source_port);
    tcp->destPort = htons(dest_port);
    tcp->sequenceNumber = htonl(seq);
    tcp->acknowledgementNumber = htonl(ack);
    tcp->dataOffset = (TCP_HEADER_LENGTH / 4) << 4;
    tcp->controllBits = type;
    tcp->windowSize = ntohs(1);
    tcp->checksum = 0x0;
    tcp->urgentPointer = 0x0;

    etherCalcTcpChecksum(ether);
}

bool etherCloseTCPConnection(etherHeader *ether)
{
    if (currentTCPState != ESTABLISHED)
        return false;

    uint32_t temp = ack;

    ack = seq + 1;

    etherBuildEtherHeader(ether, dest_addr, 0x0800);
    id = etherBuildIpHeader(ether, TCP_HEADER_LENGTH, id, dest_ip);
    etherBuildTcpHeader(ether, FIN);

    ack = temp;

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH);

    currentTCPState = CLOSE_WAIT;

    return currentTCPState != ESTABLISHED;
}

bool etherOpenTCPConnection(etherHeader *ether, uint8_t local_dest_addr[], uint8_t local_dest_ip[], uint16_t local_dest_port)
{
    if (currentTCPState != CLOSED)
        return false;

    uint8_t i = 0;

    srand(time(NULL));
    seq = rand() % 0xFFFFFFFF;

    source_port = (rand() % 16383) + 49152; // Number in dynamic port range. 49152 - 65535
    dest_port = local_dest_port;

    for (i = 0; i < HW_ADD_LENGTH; i++)
        dest_addr[i] = local_dest_addr[i];
    for (i = 0; i < IP_ADD_LENGTH; i++)
            dest_ip[i] = local_dest_ip[i];

    etherBuildEtherHeader(ether, dest_addr, 0x0800);
    id = etherBuildIpHeader(ether, TCP_HEADER_LENGTH, id, dest_ip);
    etherBuildTcpHeader(ether, SYN);

    seq++;
    ack++;

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH);

    currentTCPState = SYN_SENT;

    return currentTCPState != CLOSED;
}

void etherHandleTCPPacket(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    bool ok;
    ok = (ip->protocol == 0x06);
    if (ok)
    {
        ok = (etherCheckTcpChecksum(ether) == 0);
    }
    if (ok)
    {
        bool URG_BIT, ACK_BIT, PSH_BIT, RST_BIT, SYN_BIT, FIN_BIT;

        URG_BIT = tcp->controllBits & (1 << 5);
        ACK_BIT = tcp->controllBits & (1 << 4);
        PSH_BIT = tcp->controllBits & (1 << 3);
        RST_BIT = tcp->controllBits & (1 << 2);
        SYN_BIT = tcp->controllBits & (1 << 1);
        FIN_BIT = tcp->controllBits & (1 << 0);

        if (!URG_BIT && ACK_BIT && !PSH_BIT && !RST_BIT && !SYN_BIT && FIN_BIT) // Receive Fin and ACK
        {
            seq = ntohl(tcp->acknowledgementNumber);
            uint32_t temp = ack;
            ack = ntohl(tcp->sequenceNumber) + 1;
            etherTcpAck(ether);
            ack  = temp;
            currentTCPState = CLOSED;
        }
        if (!URG_BIT && !ACK_BIT && !PSH_BIT && !RST_BIT && SYN_BIT && !FIN_BIT)
        {}
        if (!URG_BIT && ACK_BIT && !PSH_BIT && !RST_BIT && SYN_BIT && !FIN_BIT)
        {
            seq = ntohl(tcp->acknowledgementNumber);
            uint32_t temp = ack;
            ack = ntohl(tcp->sequenceNumber) + 1;
            etherTcpAck(ether);
            ack  = temp;
            currentTCPState = ESTABLISHED;
        }
    }
}

void etherTcpAck(etherHeader *ether)
{
    etherBuildEtherHeader(ether, dest_addr, 0x0800);
    id = etherBuildIpHeader(ether, TCP_HEADER_LENGTH, id, dest_ip);
    etherBuildTcpHeader(ether, ACK);

    seq++;
    ack++;

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH);
}

void etherCalcTcpChecksum(etherHeader *ether)//(tcpHeader *tcp, ipHeader *ip)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);

    uint8_t tcpHeaderLength = (tcp->dataOffset >> 4) * 4;
    uint32_t sum = 0;
    // 32-bit sum over ip header
    tcp->checksum = 0;
    etherSumWords(tcp, tcpHeaderLength, &sum);
    etherSumWords(ip->sourceIp, 4, &sum);
    etherSumWords(ip->destIp, 4, &sum);
    uint16_t tmp = ip->protocol;
    tmp = htons(tmp);
    etherSumWords(&tmp, 2, &sum);
    tmp = tcpHeaderLength;
    tmp = htons(tmp);
    etherSumWords(&tmp, 2, &sum);
    tcp->checksum = getEtherChecksum(sum);
}

bool etherCheckTcpChecksum(etherHeader *ether)    //(tcpHeader *tcp, ipHeader *ip)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);

    uint8_t tcpHeaderLength = (tcp->dataOffset >> 4) * 4;
    uint32_t sum = 0;
    // 32-bit sum over ip header
    tcp->checksum = 0;
    etherSumWords(tcp, tcpHeaderLength, &sum);
    etherSumWords(ip->sourceIp, 4, &sum);
    etherSumWords(ip->destIp, 4, &sum);
    uint16_t tmp = ip->protocol;
    tmp = htons(tmp);
    etherSumWords(&tmp, 2, &sum);
    tmp = tcpHeaderLength;
    tmp = htons(tmp);
    etherSumWords(&tmp, 2, &sum);
    return (getEtherChecksum(sum) == 0);
}
