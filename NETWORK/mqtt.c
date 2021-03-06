// MQTT Library
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
#include "NETWORK/ip.h"
#include "NETWORK/mqtt.h"
#include "main.h"

uint16_t mqttID = 1;

bool connected = false;

char mqttClientID[MAX_MQTT_ID];

uint8_t mqtt_dest_addr[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t mqtt_dest_ip[IP_ADD_LENGTH] = {0,0,0,0};

void mqttSendConnect(etherHeader *ether, uint8_t *local_dest_addr, uint8_t *local_dest_ip, char * ID)
{
    uint8_t i = 0;

    for (i = 0; i < HW_ADD_LENGTH; i++)
        mqtt_dest_addr[i] = local_dest_addr[i];
    for (i = 0; i < IP_ADD_LENGTH; i++)
        mqtt_dest_ip[i] = local_dest_ip[i];

    for (i = 0; i < MAX_MQTT_ID && ID[i] != '\0'; i++)
        mqttClientID[i] = ID[i];

    etherOpenTCPConnection(ether, local_dest_addr, local_dest_ip, MQTT_PORT);
}

void mqttSendConnectReturn(etherHeader *ether)
{
    uint8_t i = 0;

    uint16_t ClientNameLength = 0;

    while (mqttClientID[ClientNameLength] != '\0')
        ClientNameLength++;

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
        mqttConnect->clientID[i] = mqttClientID[i];

    etherCalcTcpChecksum(ether);

    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH + MQTTLength);

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

//=====================================================================================================

void mqttSendDisconnect(etherHeader *ether)
{
    if (!connected)
        return;

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
    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH + MQTTLength);
    etherIncrementSeq(MQTTLength);

    connected = false;

}

//=====================================================================================================

void mqttSendPublish(etherHeader *ether, char *topic, char *data)
{

    if (mqttID == 0)
        mqttID = 1;

    uint8_t i = 0;

    uint16_t TopicLength = 0;

    while (topic[TopicLength] != '\0')
        TopicLength++;

    uint16_t DataLength = 0;

    while (data[DataLength] != '\0')
            DataLength++;

    uint16_t MQTTLength = 0x02 + 0x04 + TopicLength + (DataLength + 0x02);

    etherBuildEtherHeader(ether, mqtt_dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + MQTTLength, mqtt_dest_ip);
    etherBuildTcpHeader(ether, PSH_ACK);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTPublishFrameP1 *mqttPublishP1 = (MQTTPublishFrameP1*)tcp->data;
    MQTTPublishFrameP2 *mqttPublishP2 = (MQTTPublishFrameP2*)((uint8_t*)mqttPublishP1 + (MQTTLength - ((DataLength + 0x02) + 0x02)));
    MQTTString *mqttString = (MQTTString*)mqttPublishP2->data;

    mqttPublishP1->typeFlags = PUBLISH | 0x02; //Exactly once delivery
    mqttPublishP1->remainingLength = MQTTLength - 0x02;

    mqttPublishP1->topicLength = htons(TopicLength);
    for (i = 0; i < TopicLength; i++)
        mqttPublishP1->topic[i] = topic[i];

    mqttPublishP2->ID = htons(mqttID);

    mqttString->length = htons(DataLength);
    for (i = 0; i < DataLength; i++)
        mqttString->string[i] = data[i];

    etherCalcTcpChecksum(ether);
    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH + MQTTLength);
    etherIncrementSeq(MQTTLength);

    mqttID++;
}

void mqttHandlePublish(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTPublishRecFrame *mqttPublishRec = (MQTTPublishRecFrame*)tcp->data;

    if ((mqttPublishRec->typeFlags & 0xF0) != PUBLISH)
        return;

    uint16_t topicLength = ntohs(mqttPublishRec->topicLength);

    MQTTString *mqttString = (MQTTString*)((uint8_t*)mqttPublishRec + (0x04 + topicLength));

    char topic[MAX_TOPIC_LENGTH];
    char data[MAX_DATA_LENGTH];
    uint8_t i = 0;

    for (i = 0; i < ntohs(mqttPublishRec->topicLength); i++)
        topic[i] = mqttPublishRec->topic[i];
    topic[ntohs(mqttPublishRec->topicLength)] = '\0';

    for (i = 0; i < ntohs(mqttString->length); i++)
        data[i] = mqttString->string[i];
    data[ntohs(mqttString->length)] = '\0';

    printPublish(topic, data);
}

//=====================================================================================================

void mqttSendSubscribe(etherHeader *ether, char *topic)
{
    uint8_t i = 0;

    uint16_t TopicLength = 0;

    while (topic[TopicLength] != '\0')
        TopicLength++;

    uint16_t MQTTLength = 0x02 + 0x05 + TopicLength;
    etherBuildEtherHeader(ether, mqtt_dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + MQTTLength, mqtt_dest_ip);
    etherBuildTcpHeader(ether, PSH_ACK);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTSubscribeFrameP1 *mqttSubscribeP1 = (MQTTSubscribeFrameP1*)tcp->data;
    MQTTSubscribeFrameP2 *mqttSubscribeP2 = (MQTTSubscribeFrameP2*)((uint8_t*)mqttSubscribeP1 + (MQTTLength - 0x01));

    mqttSubscribeP1->typeFlags = SUBSCRIBE | 0x02; //Exactly once delivery
    mqttSubscribeP1->remainingLength = MQTTLength - 0x02;

    mqttSubscribeP1-> ID = htons(mqttID);

    mqttSubscribeP1->topicLength = htons(TopicLength);
    for (i = 0; i < TopicLength; i++)
        mqttSubscribeP1->topic[i] = topic[i];

    mqttSubscribeP2->QOS = 0x00;

    etherCalcTcpChecksum(ether);
    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH + MQTTLength);
    etherIncrementSeq(MQTTLength);

    mqttID++;

}

void mqttSendUnsubscribe(etherHeader *ether, char *topic)
{
    uint8_t i = 0;

    uint16_t TopicLength = 0;

    while (topic[TopicLength] != '\0')
        TopicLength++;

    uint16_t MQTTLength = 0x02 + 0x04 + TopicLength;
    etherBuildEtherHeader(ether, mqtt_dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + MQTTLength, mqtt_dest_ip);
    etherBuildTcpHeader(ether, PSH_ACK);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTUnsubscribeFrame *mqttUnsubscribe = (MQTTUnsubscribeFrame*)tcp->data;

    mqttUnsubscribe->typeFlags = UNSUBSCRIBE | 0x02; //Exactly once delivery
    mqttUnsubscribe->remainingLength = MQTTLength - 0x02;

    mqttUnsubscribe-> ID = htons(mqttID);

    mqttUnsubscribe->topicLength = htons(TopicLength);
    for (i = 0; i < TopicLength; i++)
        mqttUnsubscribe->topic[i] = topic[i];

    etherCalcTcpChecksum(ether);
    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH + MQTTLength);
    etherIncrementSeq(MQTTLength);

    mqttID++;

}

//=====================================================================================================

void mqttSendPingReq(etherHeader *ether)
{
    uint16_t MQTTLength = 0x02; //2bytes

    etherBuildEtherHeader(ether, mqtt_dest_addr, 0x0800);
    etherBuildIpHeader(ether, TCP_HEADER_LENGTH + MQTTLength, mqtt_dest_ip);
    etherBuildTcpHeader(ether, PSH_ACK);

    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTPingReqFrame *mqttPingReq = (MQTTPingReqFrame*)tcp->data;

    mqttPingReq->typeFlags = PINGREQ | 0x0;
    mqttPingReq->remainingLength = MQTTLength - 0x02;

    etherCalcTcpChecksum(ether);
    etherPutPacket(ether, sizeof(etherHeader) + IP_HEADER_LENGTH + TCP_HEADER_LENGTH + MQTTLength);
    etherIncrementSeq(MQTTLength);
}

void MQTThandlePingResponse(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTPingRespFrame *mqttPingResp = (MQTTPingRespFrame*)tcp->data;

    if (mqttPingResp->typeFlags == PINGRESP)
        handlePingResp();

}

//=====================================================================================================

bool MQTThandleConnect(etherHeader *ether)
{
    ipHeader *ip = (ipHeader*)ether->data;
    uint8_t ipHeaderLength = (ip->revSize & 0xF) * 4;
    tcpHeader *tcp = (tcpHeader*)((uint8_t*)ip + ipHeaderLength);
    MQTTConnectAckFrame *mqttConnectAck = (MQTTConnectAckFrame*)tcp->data;

    if ((mqttConnectAck->typeFlags & 0xF0) == CONNACK)
    {
        if (mqttConnectAck->returnCode == 0)
        {
            connected = true;
            connectMQTTReturn();
        }
    }

    return connected;
}

bool MQTThandleDisconnect(etherHeader *ether)
{
    connected = false;
    disconnectMQTTReturn();
    return connected;
}

bool MQTTisConnected()
{
    return connected;
}

