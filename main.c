// IOT Project #1
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

// Pinning for IoT projects with wireless modules:
// N24L01+ RF transceiver
//   MOSI (SSI0Tx) on PA5
//   MISO (SSI0Rx) on PA4
//   SCLK (SSI0Clk) on PA2
//   ~CS on PE0
//   INT on PB2
// Xbee module
//   DIN (UART1TX) on PC5
//   DOUT (UART1RX) on PC4


//-----------------------------------------------------------------------------
// Configuring Wireshark to examine packets
//-----------------------------------------------------------------------------

// sudo ethtool --offload eno2 tx off rx off
// in wireshark, preferences->protocol->ipv4->validate the checksum if possible
// in wireshark, preferences->protocol->udp->validate the checksum if possible

//-----------------------------------------------------------------------------
// Sending UDP test packets
//-----------------------------------------------------------------------------

// test this with a udp send utility like sendip
//   if sender IP (-is) is 192.168.1.1, this will attempt to
//   send the udp datagram (-d) to 192.168.1.199, port 1024 (-ud)
// sudo sendip -p ipv4 -is 192.168.1.1 -p udp -ud 1024 -d "on" 192.168.1.199
// sudo sendip -p ipv4 -is 192.168.1.1 -p udp -ud 1024 -d "off" 192.168.1.199

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "SYSTEM/clock.h"
#include "SYSTEM/gpio.h"
#include "SYSTEM/spi0.h"
#include "SYSTEM/uart0.h"
#include "NETWORK/tcp.h"
#include "NETWORK/ip.h"
#include "NETWORK/arp.h"
#include "SYSTEM/wait.h"
#include "NETWORK/eth0.h"
#include "SYSTEM/eeprom.h"
#include "tm4c123gh6pm.h"

// Pins
#define RED_LED PORTF,1
#define BLUE_LED PORTF,2
#define GREEN_LED PORTF,3
#define PUSH_BUTTON PORTF,4

#define IP_ADD_LENGTH 4
#define HW_ADD_LENGTH 6

#define IP_EEPROM_ADD 0
#define MQTT_EEPROM_ADD 1

//#define ipAddressLocal 192,168,1,113
//#define MQTT_IP 192,168,1,1
#define GATEWAY_IP 192,168,1,1


typedef enum _STATE
{
    IDLE,
    CONNECTING,
    CONNECTED,
    DISCONNECTING
} STATE;

uint8_t ipAddressLocal[IP_ADD_LENGTH] = {0,0,0,0};
uint8_t macAddressMQTT[HW_ADD_LENGTH] = {2,3,4,5,6,7};
uint8_t ipAddressMQTT[IP_ADD_LENGTH] = {0,0,0,0};

STATE currentState = IDLE;

void initHw();
void displayConnectionInfo();
void printIP(uint8_t * IP);
void printMAC(uint8_t * MAC);
void connectMQTT(etherHeader *data);

//-----------------------------------------------------------------------------
// Subroutines                
//-----------------------------------------------------------------------------

// Initialize Hardware
void initHw()
{
    // Initialize system clock to 40 MHz
    initSystemClockTo40Mhz();

    // Enable clocks
    enablePort(PORTF);
    _delay_cycles(3);

    // Configure LED and pushbutton pins
    selectPinPushPullOutput(RED_LED);
    selectPinPushPullOutput(GREEN_LED);
    selectPinPushPullOutput(BLUE_LED);
    selectPinDigitalInput(PUSH_BUTTON);
}

void displayConnectionInfo()
{
    uint8_t mac[6];
    uint8_t ip[4];
    etherGetMacAddress(mac);
    putsUart0("HW: ");
    printMAC(mac);
    putcUart0('\n');
    etherGetIpAddress(ip);
    putsUart0("IP: ");
    printIP(ip);
    putsUart0("\nMQTT IP: ");
    printIP(ipAddressMQTT);

    if (etherIsDhcpEnabled())
        putsUart0(" (dhcp)");
    else
        putsUart0(" (static)");
    putcUart0('\n');

    etherGetIpSubnetMask(ip);
    putsUart0("SN: ");
    printIP(ip);
    putcUart0('\n');
    etherGetIpGatewayAddress(ip);
    putsUart0("GW: ");
    printIP(ip);

    putcUart0('\n');
    if (etherIsLinkUp())
        putsUart0("Link is up\n");
    else
        putsUart0("Link is down\n");
}

void printIP(uint8_t * IP)
{
    uint8_t i;
    char str[10];
    for (i = 0; i < IP_ADD_LENGTH; i++)
    {
        sprintf(str, "%u", IP[i]);
        putsUart0(str);
        if (i < IP_ADD_LENGTH-1)
            putcUart0('.');
    }
}

void printMAC(uint8_t * MAC)
{
    uint8_t i;
    char str[10];
    for (i = 0; i < HW_ADD_LENGTH; i++)
    {
        sprintf(str, "%02x", MAC[i]);
        putsUart0(str);
        if (i < HW_ADD_LENGTH-1)
            putcUart0(':');
    }
}

void connectMQTT(etherHeader *data)
{
    etherSendArpRequest(data, ipAddressMQTT);
    currentState = CONNECTING;
}

void disconnectMQTT(etherHeader *data)
{
    mqttSendDisconnect(data);
    currentState = IDLE;
}

void readIPfromEeprom(uint16_t loc, uint8_t *ip)
{
    uint32_t temp = readEeprom(loc);
    ip[0] = temp >> 24;
    ip[1] = temp >> 16;
    ip[2] = temp >> 8;
    ip[3] = temp;
}

void getIPfromUser(USER_DATA *serialData, uint8_t ip[IP_ADD_LENGTH], uint16_t eepromAdd)
{

    ip[0] = getFieldInteger(serialData, 0);
    ip[1] = getFieldInteger(serialData, 1);
    ip[2] = getFieldInteger(serialData, 2);
    ip[3] = getFieldInteger(serialData, 3);

    //etherSetIpAddress(ip[0], ip[1], ip[2], ip[3]);
    uint32_t temp = ip[3] | (ip[2] << 8) | (ip[1] << 16) | (ip[0] << 24);
    writeEeprom(eepromAdd, temp);
}

void getSetIPfromUser(USER_DATA *serialData, uint8_t ip[IP_ADD_LENGTH], uint16_t eepromAdd)
{

    ip[0] = getFieldInteger(serialData, 2);
    ip[1] = getFieldInteger(serialData, 3);
    ip[2] = getFieldInteger(serialData, 4);
    ip[3] = getFieldInteger(serialData, 5);

    //etherSetIpAddress(ip[0], ip[1], ip[2], ip[3]);
    uint32_t temp = ip[3] | (ip[2] << 8) | (ip[1] << 16) | (ip[0] << 24);
    writeEeprom(eepromAdd, temp);
}

void handlePingResp()
{
    putsUart0("PONG\n");
}


//=============================================================================================
// Main
//=============================================================================================

// Max packet is calculated as:
// Ether frame header (18) + Max MTU (1500) + CRC (4)
#define MAX_PACKET_SIZE 1522

int main(void)
{
    initEeprom();
    uint8_t* udpData;
    uint8_t buffer[MAX_PACKET_SIZE];
    etherHeader *data = (etherHeader*) buffer;

    USER_DATA serialData;
    //Get Static IP and MQTT address from eeprom
    readIPfromEeprom(IP_EEPROM_ADD, ipAddressLocal);
    readIPfromEeprom(MQTT_EEPROM_ADD, ipAddressMQTT);

    // Init controller
    initHw();

    // Setup UART0
    initUart0();
    setUart0BaudRate(115200, 40e6);

    // Init ethernet interface (eth0)
    putsUart0("\nStarting eth0\n");
    etherSetMacAddress(2, 3, 4, 5, 6, 113);
    etherDisableDhcpMode();

    //etherSetIpAddress(ipAddressLocal);
    etherSetIpAddress(ipAddressLocal[0], ipAddressLocal[1], ipAddressLocal[2], ipAddressLocal[3]);
    etherSetIpSubnetMask(255, 255, 255, 0);
    etherSetIpGatewayAddress(GATEWAY_IP);
    etherInit(ETHER_UNICAST | ETHER_BROADCAST | ETHER_HALFDUPLEX);
    waitMicrosecond(100000);
    displayConnectionInfo();

    //checkIPs
    if((ipAddressLocal[0] == 0) && (ipAddressLocal[1] == 0) && (ipAddressLocal[2] == 0))
    {
        putsUart0("\nMissing static IP. Type IP address below:\n");
        getsUart0(&serialData);
        parseFields(&serialData);
        getIPfromUser(&serialData, ipAddressLocal, IP_EEPROM_ADD);
        etherSetIpAddress(ipAddressLocal[0], ipAddressLocal[1], ipAddressLocal[2], ipAddressLocal[3]);
    }
    if((ipAddressMQTT[0] == 0) && (ipAddressMQTT[1] == 0) && (ipAddressMQTT[2] == 0))
    {
        putsUart0("Missing MQTT IP address. Type IP address below:\n");
        getsUart0(&serialData);
        parseFields(&serialData);
        getIPfromUser(&serialData,  ipAddressMQTT, MQTT_EEPROM_ADD);
    }

    // Flash LED
    setPinValue(GREEN_LED, 1);
    waitMicrosecond(100000);
    setPinValue(GREEN_LED, 0);
    waitMicrosecond(100000);

    // Main Loop
    while (true)
    {
        if (kbhitUart0())
        {
            getsUart0(&serialData);
            parseFields(&serialData);
            if (isCommand(&serialData, "HELP", 0))
            {
                putsUart0("List of available commands:\n");
                putsUart0("\tHELP\n");
                putsUart0("\tREBOOT\n");
                putsUart0("\tSTATUS\n");
                putsUart0("\tSET [IP/MQTT] [IP]\n");
                putsUart0("\tPUBLISH [TOPIC] [DATA]\n");
                putsUart0("\tSUBSCRIBE [TOPIC]\n");
                putsUart0("\tUNSUBSCRIBE [TOPIC]\n");
                putsUart0("\tCONNECT\n");
                putsUart0("\tDISCONNECT\n");
                putsUart0("\tCLEAR\n");
            }
            if (isCommand(&serialData, "REBOOT", 0))
            {
                putsUart0("Not Done Yet...\n");
            }
            if (isCommand(&serialData, "STATUS", 0))
            {
                displayConnectionInfo();
                putcUart0('\n');
            }
            if (isCommand(&serialData, "SET", 5))
            {
                if (stringCompare(getFieldString(&serialData, 1),"IP"))
                {
                    getSetIPfromUser(&serialData, ipAddressLocal, IP_EEPROM_ADD);
                    etherSetIpAddress(ipAddressLocal[0], ipAddressLocal[1], ipAddressLocal[2], ipAddressLocal[3]);
                    putsUart0("*IP saved and set to: ");
                    etherGetIpAddress(ipAddressLocal);
                    printIP(ipAddressLocal);
                    putcUart0('\n');
                }
                if (stringCompare(getFieldString(&serialData, 1),"MQTT"))
                {
                    getSetIPfromUser(&serialData, ipAddressMQTT, MQTT_EEPROM_ADD);
                    putsUart0("*MQTT saved and set to: ");
                    printIP(ipAddressMQTT);
                    putcUart0('\n');
                }
            }
            if (isCommand(&serialData, "PUBLISH", 2))
            {
                putsUart0("Not Done Yet!\n");
            }
            if (isCommand(&serialData, "SUBSCRIBE", 1))
            {
                putsUart0("Not Done Yet!\n");
            }
            if (isCommand(&serialData, "UNSUBSCRIBE", 1))
            {
                putsUart0("Not Done Yet!\n");
            }
            if (isCommand(&serialData, "CONNECT", 0))
            {
                putsUart0("Connecting...\n");
                connectMQTT(data);
            }
            if (isCommand(&serialData, "DISCONNECT", 0))
            {
                putsUart0("Disconnecting...\n");
                disconnectMQTT(data);
            }
            if (isCommand(&serialData, "CLEAR", 0))
            {
                putsUart0("Clearing Eeeprom...\n");
                clearEeprom();
            }
            if (isCommand(&serialData, "PING", 0))
            {
                mqttSendPingReq(data);
            }
        }

        // Packet processing
        if (etherIsDataAvailable())
        {
            if (etherIsOverflow())
            {
                setPinValue(RED_LED, 1);
                waitMicrosecond(100000);
                setPinValue(RED_LED, 0);
            }

            // Get packet
            etherGetPacket(data, MAX_PACKET_SIZE);

            if (currentState == CONNECTING && etherIsArpResponse(data))
            {
                uint8_t i;
                uint8_t * localMacAddressMQTT = etherParseArpResponse(data);
                for (i = 0; i < HW_ADD_LENGTH; i++)
                    macAddressMQTT[i] = localMacAddressMQTT[i];
                currentState = CONNECTED;
                putsUart0("Connected: ");
                printMAC(macAddressMQTT);
                putcUart0('\n');
                etherOpenTCPConnection(data, macAddressMQTT, ipAddressMQTT, 1883);
            }

            // Handle ARP request
            if (etherIsArpRequest(data))
            {
                etherSendArpResponse(data);
            }

            // Handle IP datagram
            if (etherIsIp(data))
            {
            	if (etherIsIpUnicast(data))
            	{
            		// handle icmp ping request
					if (etherIsPingRequest(data))
					{
					  etherSendPingResponse(data);
					}

					// Handle TCP
					etherHandleTCPPacket(data);

					// Process UDP datagram
					if (etherIsUdp(data))
					{
						udpData = etherGetUdpData(data);
						if (strcmp((char*)udpData, "on") == 0)
			                setPinValue(GREEN_LED, 1);
                        if (strcmp((char*)udpData, "off") == 0)
			                setPinValue(GREEN_LED, 0);
						etherSendUdpResponse(data, (uint8_t*)"Received", 9);
					}
                }
            }
        }
    }
}
