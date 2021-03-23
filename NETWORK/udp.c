// UDP Library
// IOT Project #1
// Nathan Fusselman and Deborah Jahaj


#include "NETWORK/eth0.h"
#include "NETWORK/tcp.h"
#include "NETWORK/ip.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"
#include "NETWORK/udp.h"


// Determines whether packet is UDP datagram
// Must be an IP packet
bool etherIsUdp(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    udpHeader *udp = (udpHeader*)((uint8_t*)ip + ipHeaderLength);
    bool ok;
    uint16_t tmp16;
    uint32_t sum = 0;
    ok = (ip->protocol == 0x11);
    if (ok)
    {
        // 32-bit sum over pseudo-header
        etherSumWords(ip->sourceIp, 8, &sum);
        tmp16 = ip->protocol;
        sum += (tmp16 & 0xff) << 8;
        etherSumWords(&udp->length, 2, &sum);
        // add udp header and data
        etherSumWords(udp, ntohs(udp->length), &sum);
        ok = (getEtherChecksum(sum) == 0);
    }
    return ok;
}

// Gets pointer to UDP payload of frame
uint8_t * etherGetUdpData(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    udpHeader *udp = (udpHeader*)((uint8_t*)ip + ipHeaderLength);
    return udp->data;
}

// Send responses to a udp datagram
// destination port, ip, and hardware address are extracted from provided data
// uses destination port of received packet as destination of this packet
void etherSendUdpResponse(etherHeader *ether, uint8_t *udpData, uint8_t udpSize)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    udpHeader *udp = (udpHeader*)((uint8_t*)ip + ipHeaderLength);
    uint8_t *copyData;
    uint8_t i, tmp8;
    uint16_t tmp16;
    uint16_t udpLength;
    uint32_t sum = 0;

    // swap source and destination fields
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        tmp8 = ether->destAddress[i];
        ether->destAddress[i] = ether->sourceAddress[i];
        ether->sourceAddress[i] = tmp8;
    }
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        tmp8 = ip->destIp[i];
        ip->destIp[i] = ip->sourceIp[i];
        ip->sourceIp[i] = tmp8;
    }
    // set source port of resp will be dest port of req
    // dest port of resp will be left at source port of req
    // unusual nomenclature, but this allows a different tx
    // and rx port on other machine
    udp->sourcePort = udp->destPort;
    // adjust lengths
    udpLength = 8 + udpSize;
    ip->length = htons(ipHeaderLength + udpLength);


    // 32-bit sum over ip header
    etherCalcIpChecksum(ether);//etherCalcIpChecksum(ip);

    // set udp length
    udp->length = htons(udpLength);
    // copy data
    copyData = udp->data;
    for (i = 0; i < udpSize; i++)
        copyData[i] = udpData[i];
    // 32-bit sum over pseudo-header
    etherSumWords(ip->sourceIp, 8, &sum);
    tmp16 = ip->protocol;
    sum += (tmp16 & 0xff) << 8;
    etherSumWords(&udp->length, 2, &sum);
    // add udp header
    udp->check = 0;
    etherSumWords(udp, udpLength, &sum);
    udp->check = getEtherChecksum(sum);

    // send packet with size = ether + udp hdr + ip header + udp_size
    etherPutPacket(ether, sizeof(etherHeader) + ipHeaderLength + udpLength);
}





