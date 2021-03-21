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

#include <tcp.h>
#include <eth0.h>
#include <stdint.h>
#include <stdbool.h>
#include <ip.h>
#include <mqtt.h>

uint8_t mqtt_dest_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t mqtt_dest_ip[IP_ADD_LENGTH] = {0,0,0,0};

void mqttSendConnect(etherHeader *ether, uint8_t *local_dest_addr, uint8_t *local_dest_ip)
{
    uint8_t i = 0;

    for (i = 0; i < HW_ADD_LENGTH; i++)
        mqtt_dest_addr[i] = local_dest_addr[i];
    for (i = 0; i < IP_ADD_LENGTH; i++)
        mqtt_dest_ip[i] = local_dest_ip[i];

    uint16_t MQTTLength = 160;

    etherBuildEtherHeader(ether, mqtt_dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + MQTTLength, mqtt_dest_ip);
    etherBuildTcpHeader(ether, MQTTLength, NONE);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTFixedFrame *mqttFixed = (MQTTFixedFrame*)tcp->data;
    MQTTConnectFrame *mqttConnect = (MQTTConnectFrame*)mqttFixed->data;

    mqttFixed->typeFlags = CONNECT | 0x0;

    mqttConnect->nameLength = 0x0004;
    mqttConnect->protocolName[0] = 'M';
    mqttConnect->protocolName[1] = 'Q';
    mqttConnect->protocolName[2] = 'T';
    mqttConnect->protocolName[3] = 'T';

    mqttConnect->level = 0x04;
    mqttConnect->flags = CLEAN_SESSION;
    mqttConnect->keepAlive = 0x000A;

    MQTTPayloadString *clientID = (MQTTPayloadString*)mqttConnect->data;

    clientID->length = 0x0007;
    clientID->string[0] = 'T';
    clientID->string[1] = 'A';
    clientID->string[2] = 's';
    clientID->string[3] = 'R';
    clientID->string[4] = 'U';
    clientID->string[5] = 'L';
    clientID->string[6] = 'E';

    etherCalcTcpChecksum(ether);

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH+ MQTTLength);

}
