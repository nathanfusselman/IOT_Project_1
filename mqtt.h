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

typedef struct _MQTTFixedFrame
{
    uint8_t typeFlags;
    uint8_t  data[0];
}MQTTFixedFrame;

// Control Packets Type
typedef enum _mqtt_type
{
    CONNECT     = 0x1,
    CONNACK     = 0x2,
    PUBLISH     = 0x3,
    PUBACK      = 0x4,
    PUBREC      = 0x5,
    PUBREL      = 0x6,
    PUBCOMP     = 0x7,
    SUBSCRIBE   = 0x8,
    SUBACK      = 0x9,
    UNSUBSCRIBE = 0xA,
    UNSUBACK    = 0xB,
    PINGREQ     = 0xC,
    PINGRESP    = 0xD,
    DISCONNECT  = 0xE
}MQTT_TYPE;

#endif /* MQTT_H_ */
