// ICPM Library
// IOT Project #1
// Nathan Fusselman and Deborah Jahaj

#include "NETWORK/eth0.h"
#include "NETWORK/tcp.h"
#include "NETWORK/ip.h"
#include "NETWORK/icpm.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "main.h"


// Determines whether packet is ping request
// Must be an IP packet
bool etherIsPingRequest(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    icmpHeader *icmp = (icmpHeader*)((uint8_t*)ip + ipHeaderLength);
    return (ip->protocol == 0x01 & icmp->type == 8);
}

// Sends a ping response given the request data
void etherSendPingResponse(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    icmpHeader *icmp = (icmpHeader*)((uint8_t*)ip + ipHeaderLength);
    uint8_t i, tmp;
    uint16_t icmp_size;
    uint32_t sum = 0;
    // swap source and destination fields
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        tmp = ether->destAddress[i];
        ether->destAddress[i] = ether->sourceAddress[i];
        ether->sourceAddress[i] = tmp;
    }
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        tmp = ip->destIp[i];
        ip->destIp[i] = ip ->sourceIp[i];
        ip->sourceIp[i] = tmp;
    }
    // this is a response
    icmp->type = 0;
    // calc icmp checksum
    icmp->check = 0;
    icmp_size = ntohs(ip->length) - ipHeaderLength;
    etherSumWords(icmp, icmp_size, &sum);
    icmp->check = getEtherChecksum(sum);
    // send packet
    etherPutPacket(ether, sizeof(etherHeader) + ntohs(ip->length));
}
