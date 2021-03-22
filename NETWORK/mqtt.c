// MQTT Library
// Nathan Fusselan & Deborah Jahaj

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
#include "NETWORK/ip.h"
#include "NETWORK/mqtt.h"

uint8_t mqtt_dest_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t mqtt_dest_ip[IP_ADD_LENGTH] = {0,0,0,0};

void mqttSendDisconnect(etherHeader *ether)
{
    uint16_t MQTTLength = 0x02; //2bytes

    etherBuildEtherHeader(ether, mqtt_dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + MQTTLength, mqtt_dest_ip);
    etherBuildTcpHeader(ether, PSH_ACK);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTDisconnectFrame *mqttDisconnect = (MQTTDisconnectFrame*)tcp->data;

    mqttDisconnect->typeFlags = DISCONNECT | 0x0;
    mqttDisconnect->remainingLength = MQTTLength - 0x02;

    etherCalcTcpChecksum(ether);
    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH+ MQTTLength);
    etherIncrementSeq(MQTTLength);

}

void mqttSendConnect(etherHeader *ether, uint8_t *local_dest_addr, uint8_t *local_dest_ip)
{
    uint8_t i = 0;

    for (i = 0; i < HW_ADD_LENGTH; i++)
        mqtt_dest_addr[i] = local_dest_addr[i];
    for (i = 0; i < IP_ADD_LENGTH; i++)
        mqtt_dest_ip[i] = local_dest_ip[i];

    uint16_t ClientNameLength = 7;
    char name[] = "TAsRule";
    uint16_t MQTTLength = 0x02 + 0x0C + ClientNameLength;

    etherBuildEtherHeader(ether, mqtt_dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + MQTTLength, mqtt_dest_ip);
    etherBuildTcpHeader(ether, PSH_ACK);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTConnectFrame *mqttConnect = (MQTTConnectFrame*)tcp->data;

    mqttConnect->typeFlags = CONNECT | 0x0;
    mqttConnect->remainingLength = MQTTLength - 0x02;

    mqttConnect->nameLength = htons(0x0004);
    mqttConnect->protocolName[0] = 'M';
    mqttConnect->protocolName[1] = 'Q';
    mqttConnect->protocolName[2] = 'T';
    mqttConnect->protocolName[3] = 'T';

    mqttConnect->level = 0x04;
    mqttConnect->flags = CLEAN_SESSION;
    mqttConnect->keepAlive = htons(0xFFFF);

    mqttConnect->clientIDLength = htons(ClientNameLength);
    for (i = 0; i < ClientNameLength; i++)
        mqttConnect->clientID[i] = name[i];

    etherCalcTcpChecksum(ether);

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH+ MQTTLength);

    etherIncrementSeq(MQTTLength);

}

bool MQTTisPacket(etherHeader *ether)
{

    uint8_t i = 0;

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTConnectFrame *mqttConnect = (MQTTConnectFrame*)tcp->data;

    char test[] = "MQTT";

    if (mqttConnect->nameLength != htons(0x0004))
        return false;
    for (i = 0; i < 4; i++)
        if (mqttConnect->protocolName[i] != test[i])
            return false;
    return true;
}

uint8_t MQTTgetPacketLength(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTConnectFrame *mqttConnect = (MQTTConnectFrame*)tcp->data;

    return mqttConnect->remainingLength + 0x2;
}
