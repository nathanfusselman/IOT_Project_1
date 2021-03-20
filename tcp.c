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
#include <stdlib.h>


#define IP_ADD_LENGTH 4
#define HW_ADD_LENGTH 6

#define TCP_HEADER_LENGTH 20
#define IP_HEADER_LENGTH 20
#define IP_TTL 64

TCP_STATE currentTCPState = CLOSED;
uint16_t source_port = 0, dest_port = 0, id = 0;
uint32_t seq = 0;
uint8_t dest_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t source_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t dest_ip[IP_ADD_LENGTH] = {0,0,0,0};
uint8_t source_ip[IP_ADD_LENGTH] = {0,0,0,0};

bool etherOpenTCPConnection(etherHeader *ether, uint8_t local_dest_addr[], uint8_t local_dest_ip[], uint16_t local_source_port, uint16_t local_dest_port)
{
    if (currentTCPState != CLOSED)
        return false;

    uint8_t i;

    seq = rand() % 0xFFFFFFFF;

    etherGetMacAddress(source_addr);
    etherGetIpAddress(source_ip);

    source_port = local_source_port;
    dest_port = local_dest_port;

    //Build Ethernet Header
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        dest_addr[i] = local_dest_addr[i];
        ether->sourceAddress[i] = source_addr[i];
        ether->destAddress[i] = dest_addr[i];
    }

    ether->frameType = htons(0x0800);


    //Build IP Header
    ipHeader *ip = (ipHeader*)ether->data;

    ip->revSize = 0x45;
    ip->typeOfService = 0x0;
    ip->length = htons(IP_HEADER_LENGTH + TCP_HEADER_LENGTH);
    ip->id = htons(id);
    ip->flagsAndOffset = htons(0x4000);
    ip->ttl = IP_TTL;
    ip->protocol = 0x06;
    ip->headerChecksum = 0x0; //Before it is calculated

    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        dest_ip[i] = local_dest_ip[i];
        ip->sourceIp[i] = source_ip[i];
        ip->destIp[i] = dest_ip[i];
    }

    etherCalcIpChecksum(ip);

    //Build TCP Header
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + IP_HEADER_LENGTH);

    tcp->sourcePort = htons(source_port);
    tcp->destPort = htons(dest_port);
    tcp->sequenceNumber = htonl(seq);
    tcp->acknowledgementNumber = htonl(seq + 1);
    tcp->dataOffset = (TCP_HEADER_LENGTH / 4) << 4;
    tcp->controllBits = 0x02;
    tcp->windowSize = ntohs(1);
    tcp->checksum = 0x0;
    tcp->urgentPointer = 0x0;

    etherCalcTcpChecksum(tcp, ip);

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH);

    currentTCPState = SYN_SENT;

    id++;
    seq++;

    return currentTCPState != CLOSED;
}

TCP_TYPE etherIsTCPPacket(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    bool ok;
    ok = (ip->protocol == 0x06);
    if (ok)
    {
        ok = (etherCheckTcpChecksum(tcp, ip) == 0);
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

        if (!URG_BIT && !ACK_BIT && !PSH_BIT && !RST_BIT && SYN_BIT && !FIN_BIT)
            return SYN;
        if (!URG_BIT && ACK_BIT && !PSH_BIT && !RST_BIT && SYN_BIT && !FIN_BIT)
            seq = tcp->acknowledgementNumber;
            return SYN_ACK;
    }
    return NONE;
}

void etherHandleTCPPacket(etherHeader *ether, TCP_TYPE type)
{
    if (type == NONE)
        return;
    if (type == SYN)
        return;
    if (type == SYN_ACK)
        etherTCPACK(ether);
    return;
}

void etherTCPACK(etherHeader *ether)
{

    uint8_t i;

    //Build Ethernet Header
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        ether->sourceAddress[i] = source_addr[i];
        ether->destAddress[i] = dest_addr[i];
    }

    ether->frameType = htons(0x0800);


    //Build IP Header
    ipHeader *ip = (ipHeader*)ether->data;

    ip->revSize = 0x45;
    ip->typeOfService = 0x0;
    ip->length = htons(IP_HEADER_LENGTH + TCP_HEADER_LENGTH);
    ip->id = htons(id);
    ip->flagsAndOffset = htons(0x4000);
    ip->ttl = IP_TTL;
    ip->protocol = 0x06;
    ip->headerChecksum = 0x0; //Before it is calculated

    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        ip->sourceIp[i] = source_ip[i];
        ip->destIp[i] = dest_ip[i];
    }

    etherCalcIpChecksum(ip);

    //Build TCP Header
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + IP_HEADER_LENGTH);

    tcp->sourcePort = htons(source_port);
    tcp->destPort = htons(dest_port);
    tcp->sequenceNumber = htonl(seq);
    tcp->acknowledgementNumber = htonl(seq + 1);
    tcp->dataOffset = (TCP_HEADER_LENGTH / 4) << 4;
    tcp->controllBits = 0x10;
    tcp->windowSize = ntohs(1);
    tcp->checksum = 0x0;
    tcp->urgentPointer = 0x0;

    etherCalcTcpChecksum(tcp, ip);

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH);

    currentTCPState = ESTABLISHED;
}
