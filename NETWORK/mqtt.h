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


#ifndef MQTT_H_
#define MQTT_H_

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

typedef struct _MQTTDisconnectFrame
{
    uint8_t typeFlags;
    uint8_t remainingLength;
}MQTTDisconnectFrame;

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

typedef struct _MQTTString
{
    uint16_t length;
    char string[];
}MQTTString;

typedef struct _MQTTSubscribeFrame1
{
    uint8_t typeFlags;
    uint8_t remainingLength;
    uint16_t ID;
    uint16_t topicLength;
    char topic[];
}MQTTSubscribeFrameP1;

typedef struct _MQTTSubscribeFrame2
{
    uint16_t QOS;
}MQTTSubscribeFrameP2;

void mqttSendSubscribe(etherHeader *ether, char *topic);

void mqttSendPublish(etherHeader *ether, char *topic, char *data);

void mqttSendPingReq(etherHeader *ether);
void mqttSendDisconnect(etherHeader *ether);
void mqttSendConnect(etherHeader *ether, uint8_t *local_dest_addr, uint8_t *local_dest_ip);

bool MQTTisPacket(etherHeader *ether);

uint8_t MQTTgetPacketLength(etherHeader *ether);
void MQTThandlePingResponse(etherHeader *ether);

#endif /* MQTT_H_ */
