// IP Library
// Nathan Fusselman and Deborah Jahaj


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
#include <ip.h>

uint16_t id = 0;

void etherBuildIpHeader(etherHeader *ether, uint16_t dataLength, uint8_t *dest_ip)
{

    uint8_t i = 0;
    uint8_t source_ip[IP_ADD_LENGTH];
    etherGetIpAddress(source_ip);
    ipHeader *ip = (ipHeader*)ether->data;

       ip->revSize = 0x40 | (IP_HEADER_LENGTH / 4);
       ip->typeOfService = 0x0;
       ip->length = htons(IP_HEADER_LENGTH + dataLength);
       ip->id = htons(id);
       ip->flagsAndOffset = htons(0x4000);
       ip->ttl = IP_TTL;
       ip->protocol = 0x06;
       ip->headerChecksum = htons(0x0000); //Before it is calculated

       for (i = 0; i < IP_ADD_LENGTH; i++)
       {
           ip->sourceIp[i] = source_ip[i];
           ip->destIp[i] = dest_ip[i];
       }

       etherCalcIpChecksum(ether);
}

void etherCalcIpChecksum(etherHeader *ether)//(ipHeader *ip)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    uint32_t sum = 0;
    // 32-bit sum over ip header
    ip->headerChecksum = 0;
    etherSumWords(ip, ipHeaderLength, &sum);
    ip->headerChecksum = getEtherChecksum(sum);
}

// Determines whether packet is IP datagram
bool etherIsIp(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    uint32_t sum = 0;
    bool ok;
    ok = (ether->frameType == htons(0x0800));
    if (ok)
    {
        etherSumWords(&ip->revSize, ipHeaderLength, &sum);
        ok = (getEtherChecksum(sum) == 0);
    }
    return ok;
}

// Determines whether packet is unicast to this ip
// Must be an IP packet
bool etherIsIpUnicast(etherHeader *ether)
{
    uint8_t ipAddress[IP_ADD_LENGTH];
    etherGetIpAddress(ipAddress);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t i = 0;
    bool ok = true;
    while (ok & (i < IP_ADD_LENGTH))
    {
        ok = (ip->destIp[i] == ipAddress[i]);
        i++;
    }
    return ok;
}

