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


#ifndef MQTT_H_
#define MQTT_H_

#define MQTT_PORT 1883

#define MAX_MQTT_ID 128
#define MAX_TOPIC_LENGTH 128
#define MAX_DATA_LENGTH 128

// Control Packets Type
typedef enum _mqtt_type
{
    CONNECT     = 0x10,
    CONNACK     = 0x20,
    PUBLISH     = 0x30,
    PUBACK      = 0x40,
    PUBREC      = 0x50,
    PUBREL      = 0x60,
    PUBCOMP     = 0x70,
    SUBSCRIBE   = 0x80,
    SUBACK      = 0x90,
    UNSUBSCRIBE = 0xA0,
    UNSUBACK    = 0xB0,
    PINGREQ     = 0xC0,
    PINGRESP    = 0xD0,
    DISCONNECT  = 0xE0
}MQTT_TYPE;

//=============================================================

// CONNECT

typedef enum _mqtt_connect_flags
{
    USERNAME = 0x80,
    PASSWORD = 0x40,
    WILL_RETAIN = 0x20,
    WILL_QOS = 0x18,
    WILL_FLAG = 0x04,
    CLEAN_SESSION = 0x02
}MQTT_CONNECT_FLAGS;

typedef struct _MQTTConnectFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
    uint16_t nameLength;
    char protocolName[4];
    uint8_t level;
    uint8_t flags;
    uint16_t keepAlive;
    uint16_t clientIDLength;
    char clientID[];
}MQTTConnectFrame;

typedef struct _MQTTConnectAckFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
    uint8_t flags;
    uint8_t returnCode;
}MQTTConnectAckFrame;

//=============================================================

// DISCONNECT

typedef struct _MQTTDisconnectFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
}MQTTDisconnectFrame;

//=============================================================

// PING

typedef struct _MQTTPingReqFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
}MQTTPingReqFrame;

typedef struct _MQTTPingRespFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
}MQTTPingRespFrame;

//=============================================================

// PUBLISH

typedef struct _MQTTPublishFrameP1
{
    uint8_t typeFlags;
    uint8_t remainingLength;
    uint16_t topicLength;
    char topic[];
}MQTTPublishFrameP1;

typedef struct _MQTTPublishFrameP2
{
    uint16_t ID;
    uint8_t data[];
}MQTTPublishFrameP2;

typedef struct _MQTTPublishRecFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
    uint16_t topicLength;
    char topic[];
}MQTTPublishRecFrame;

//=============================================================

typedef struct _MQTTString
{
    uint16_t length;
    char string[];
}MQTTString;

//=============================================================

// SUBSCRIBE

typedef struct _MQTTSubscribeFrameP1
{
    uint8_t typeFlags;
    uint8_t remainingLength;
    uint16_t ID;
    uint16_t topicLength;
    char topic[];
}MQTTSubscribeFrameP1;

typedef struct _MQTTSubscribeFrameP2
{
    uint16_t QOS;
}MQTTSubscribeFrameP2;

//=============================================================

// UNSUBSCRIBE

typedef struct _MQTTUnsubscribeFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
    uint16_t ID;
    uint16_t topicLength;
    char topic[];
}MQTTUnsubscribeFrame;

//=============================================================

void mqttSendConnect(etherHeader *ether, uint8_t *local_dest_addr, uint8_t *local_dest_ip, char * ID);
void mqttSendConnectReturn(etherHeader *ether);
bool MQTTisPacket(etherHeader *ether);
uint8_t MQTTgetPacketLength(etherHeader *ether);

void mqttSendDisconnect(etherHeader *ether);

void mqttSendPublish(etherHeader *ether, char *topic, char *data);
void mqttHandlePublish(etherHeader *ether);

void mqttSendSubscribe(etherHeader *ether, char *topic);
void mqttSendUnsubscribe(etherHeader *ether, char *topic);

void mqttSendPingReq(etherHeader *ether);
void MQTThandlePingResponse(etherHeader *ether);

bool MQTThandleConnect(etherHeader *ether);
bool MQTThandleDisconnect(etherHeader *ether);
bool MQTTisConnected(void);

#endif /* MQTT_H_ */
