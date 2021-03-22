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

#include "NETWORK/tcp.h"
#include "NETWORK/eth0.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "NETWORK/ip.h"
#include "NETWORK/mqtt.h"

TCP_STATE currentTCPState = CLOSED;
uint16_t source_port = 0, dest_port = 0;
uint32_t seq = 0, ack = 0;
uint8_t dest_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t dest_ip[IP_ADD_LENGTH] = {0,0,0,0};

void etherBuildTcpHeader(etherHeader *ether, TCP_TYPE type)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);

    tcp->sourcePort = htons(source_port);
    tcp->destPort = htons(dest_port);
    tcp->sequenceNumber = htonl(seq);
    tcp->acknowledgementNumber = htonl(ack);
    tcp->dataOffset = (TCP_HEADER_LENGTH / 4) << 4;
    tcp->controllBits = type;
    tcp->windowSize = ntohs(0x05B4);
    tcp->checksum = 0x0;
    tcp->urgentPointer = 0x0;

    etherCalcTcpChecksum(ether);
}

bool etherOpenTCPConnection(etherHeader *ether, uint8_t local_dest_addr[], uint8_t local_dest_ip[], uint16_t local_dest_port)
{
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
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + 0x4, dest_ip);
    etherBuildTcpHeader(ether, SYN);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);

    tcp->dataOffset = ((TCP_HEADER_LENGTH + 0x4) / 4) << 4;

    tcp->data[0] = 0x02;
    tcp->data[1] = 0x04;
    tcp->data[2] = 0x05;
    tcp->data[3] = 0xB4;

    etherCalcTcpChecksum(ether);

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH + 0x4);

    currentTCPState = SYN_SENT;

    seq++;

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

        if (!URG_BIT && ACK_BIT && !PSH_BIT && !RST_BIT && !SYN_BIT && FIN_BIT)
        {
            ack++;
            etherTcpAck(ether);
            currentTCPState = CLOSED;
        }
        if (!URG_BIT && !ACK_BIT && !PSH_BIT && !RST_BIT && SYN_BIT && !FIN_BIT)
        {}
        if (!URG_BIT && ACK_BIT && PSH_BIT && !RST_BIT && !SYN_BIT && !FIN_BIT)
        {
            if (currentTCPState == ESTABLISHED)
            {
                //if(MQTTisPingResp[ether])
                MQTThandlePingResponse(ether);
                ack += MQTTgetPacketLength(ether);
                etherTcpAck(ether);
            }
        }
        if (!URG_BIT && ACK_BIT && !PSH_BIT && !RST_BIT && !SYN_BIT && !FIN_BIT)
        {}
        if (!URG_BIT && ACK_BIT && !PSH_BIT && !RST_BIT && SYN_BIT && !FIN_BIT)
        {
            ack = ntohl(tcp->sequenceNumber) + 1;
            etherTcpAck(ether);
            currentTCPState = ESTABLISHED;
            mqttSendConnect(ether, dest_addr, dest_ip);
        }
    }
}

void etherTcpAck(etherHeader *ether)
{
    etherBuildEtherHeader(ether, dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH, dest_ip);
    etherBuildTcpHeader(ether, ACK);

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH);
}

void etherCalcTcpChecksum(etherHeader *ether)//(tcpHeader *tcp, ipHeader *ip)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);

    uint16_t tcpHeaderLength = ntohs(ip->length) - IP_HEADER_LENGTH;
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

    uint16_t tcpHeaderLength = ntohs(ip->length) - IP_HEADER_LENGTH;
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

uint32_t etherIncrementSeq(uint32_t num)
{
    seq += num;
    return seq;
}
