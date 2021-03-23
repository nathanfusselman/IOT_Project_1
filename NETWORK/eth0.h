// ETH0 Library
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

#ifndef ETH0_H_
#define ETH0_H_

#include <stdint.h>
#include <stdbool.h>

#define IP_ADD_LENGTH 4
#define HW_ADD_LENGTH 6

// This M4F is little endian (TI hardwired it this way)
// Network byte order is big endian
// Must interpret uint16_t in reverse order

typedef struct _enc28j60Frame   // 4 bytes
{
  uint16_t size;
  uint16_t status;
  uint8_t data[0];
} enc28j60Frame;

typedef struct _etherHeader     // 14 bytes
{
  uint8_t destAddress[6];
  uint8_t sourceAddress[6];
  uint16_t frameType;
  uint8_t data[0];
} etherHeader;



typedef struct _icmpHeader      // 8 bytes
{
  uint8_t type;
  uint8_t code;
  uint16_t check;
  uint16_t id;
  uint16_t seq_no;
  uint8_t data[0];
} icmpHeader;

typedef struct _udpHeader       // 8 bytes
{
  uint16_t sourcePort;
  uint16_t destPort;
  uint16_t length;
  uint16_t check;
  uint8_t  data[0];
} udpHeader;

typedef struct _dhcpFrame       // 240 or more bytes
{
  uint8_t op;
  uint8_t htype;
  uint8_t hlen;
  uint8_t hops;
  uint32_t  xid;
  uint16_t secs;
  uint16_t flags;
  uint8_t ciaddr[4];
  uint8_t yiaddr[4];
  uint8_t siaddr[4];
  uint8_t giaddr[4];
  uint8_t chaddr[16];
  uint8_t data[192];
  uint32_t magicCookie;
  uint8_t options[0];
} dhcpFrame;


//=====================================================================================================

#define ETHER_UNICAST        0x80
#define ETHER_BROADCAST      0x01
#define ETHER_MULTICAST      0x02
#define ETHER_HASHTABLE      0x04
#define ETHER_MAGICPACKET    0x08
#define ETHER_PATTERNMATCH   0x10
#define ETHER_CHECKCRC       0x20

#define ETHER_HALFDUPLEX     0x00
#define ETHER_FULLDUPLEX     0x100

#define LOBYTE(x) ((x) & 0xFF)
#define HIBYTE(x) (((x) >> 8) & 0xFF)

//=====================================================================================================
// Subroutines
//=====================================================================================================

void etherBuildEtherHeader(etherHeader *ether, uint8_t *dest_addr, uint16_t frameType);
void etherCsOn(void);
void etherCsOff(void);

void etherWriteReg(uint8_t reg, uint8_t data);
uint8_t etherReadReg(uint8_t reg);
void etherSetReg(uint8_t reg, uint8_t mask);
void etherClearReg(uint8_t reg, uint8_t mask);

void etherSetBank(uint8_t reg);
void etherWritePhy(uint8_t reg, uint16_t data);
uint16_t etherReadPhy(uint8_t reg);

void etherWriteMemStart(void);
void etherWriteMem(uint8_t data);
void etherWriteMemStop(void);
void etherReadMemStart(void);
uint8_t etherReadMem(void);
void etherReadMemStop(void);

void etherInit(uint16_t mode);

bool etherIsLinkUp(void);
bool etherIsDataAvailable(void);
bool etherIsOverflow(void);

uint16_t etherGetPacket(etherHeader *ether, uint16_t maxSize);
bool etherPutPacket(etherHeader *ether, uint16_t size);

bool etherIsPingRequest(etherHeader *ether);
void etherSendPingResponse(etherHeader *ether);

bool etherIsUdp(etherHeader *ether);
uint8_t* etherGetUdpData(etherHeader *ether);
void etherSendUdpResponse(etherHeader *ether, uint8_t* udpData, uint8_t udpSize);

uint16_t etherGetId(void);
void etherIncId(void);

void etherEnableDhcpMode(void);
void etherDisableDhcpMode(void);
bool etherIsDhcpEnabled(void);

bool etherIsIpValid(void);
void etherSetIpAddress(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3);
void etherGetIpAddress(uint8_t ip[4]);

void etherSetIpGatewayAddress(uint8_t ip0, uint8_t ip1, uint8_t ip2, uint8_t ip3);
void etherGetIpGatewayAddress(uint8_t ip[4]);

void etherSetIpSubnetMask(uint8_t mask0, uint8_t mask1, uint8_t mask2, uint8_t mask3);
void etherGetIpSubnetMask(uint8_t mask[4]);

void etherSetMacAddress(uint8_t mac0, uint8_t mac1, uint8_t mac2, uint8_t mac3, uint8_t mac4, uint8_t mac5);
void etherGetMacAddress(uint8_t mac[6]);

void etherSumWords(void* data, uint16_t sizeInBytes, uint32_t* sum);
uint16_t getEtherChecksum(uint32_t sum);

uint16_t htons(uint16_t value);
uint32_t htonl(uint32_t value);

uint16_t ntohs(uint16_t value);
uint32_t ntohl(uint32_t value);

#endif
