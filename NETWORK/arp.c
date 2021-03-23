// ARP Library
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

#include "NETWORK/tcp.h"
#include "NETWORK/eth0.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include "NETWORK/ip.h"

// Determines whether packet is ARP - Request
bool etherIsArpRequest(etherHeader *ether)
{
    uint8_t ipAddress[IP_ADD_LENGTH];
    etherGetIpAddress(ipAddress);

    arpPacket *arp = (arpPacket*)ether->data;
    bool ok;
    uint8_t i = 0;
    ok = (ether->frameType == htons(0x0806));
    while (ok & (i < IP_ADD_LENGTH))
    {
        ok = (arp->destIp[i] == ipAddress[i]);
        i++;
    }
    if (ok)
        ok = (arp->op == htons(1));
    return ok;
}

// Determines whether packet is ARP - Response
bool etherIsArpResponse(etherHeader *ether)
{
    uint8_t ipAddress[IP_ADD_LENGTH];
    etherGetIpAddress(ipAddress);

    arpPacket *arp = (arpPacket*)ether->data;
    bool ok;
    uint8_t i = 0;
    ok = (ether->frameType == htons(0x0806));
    while (ok & (i < IP_ADD_LENGTH))
    {
        ok = (arp->destIp[i] == ipAddress[i]);
        i++;
    }
    if (ok)
        ok = (arp->op == htons(2));
    return ok;
}

//=====================================================================================================

// Sends an ARP response given the request data
void etherSendArpResponse(etherHeader *ether)
{
    uint8_t macAddress[HW_ADD_LENGTH];
    etherGetMacAddress(macAddress);

    arpPacket *arp = (arpPacket*)ether->data;
    uint8_t i, tmp;
    // set op to response
    arp->op = htons(2);
    // swap source and destination fields
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        arp->destAddress[i] = arp->sourceAddress[i];
        ether->destAddress[i] = ether->sourceAddress[i];
        ether->sourceAddress[i] = arp->sourceAddress[i] = macAddress[i];
    }
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        tmp = arp->destIp[i];
        arp->destIp[i] = arp->sourceIp[i];
        arp->sourceIp[i] = tmp;
    }
    // send packet
    etherPutPacket(ether, sizeof(etherHeader) + sizeof(arpPacket));
}

// Sends an ARP request
void etherSendArpRequest(etherHeader *ether, uint8_t ip[])
{
    uint8_t ipAddress[IP_ADD_LENGTH];
    uint8_t macAddress[HW_ADD_LENGTH];
    etherGetIpAddress(ipAddress);
    etherGetMacAddress(macAddress);

    arpPacket *arp = (arpPacket*)ether->data;
    uint8_t i;
    // fill ethernet frame
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        ether->destAddress[i] = 0xFF;
        ether->sourceAddress[i] = macAddress[i];
    }
    ether->frameType = 0x0608;
    // fill arp frame
    arp->hardwareType = htons(1);
    arp->protocolType = htons(0x0800);
    arp->hardwareSize = HW_ADD_LENGTH;
    arp->protocolSize = IP_ADD_LENGTH;
    arp->op = htons(1);
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        arp->sourceAddress[i] = macAddress[i];
        arp->destAddress[i] = 0xFF;
    }
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        arp->sourceIp[i] = ipAddress[i];
        arp->destIp[i] = ip[i];
    }
    // send packet
    etherPutPacket(ether, sizeof(etherHeader) + sizeof(arpPacket));
}

//=====================================================================================================

// Gathers ARP MAC address
uint8_t * etherParseArpResponse(etherHeader *ether)
{
    arpPacket *arp = (arpPacket*)ether->data;
    return arp->sourceAddress;
}


