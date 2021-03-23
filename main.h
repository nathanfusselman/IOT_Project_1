// MAIN Library
// IOT Project #1
// Nathan Fusselman and Deborah Jahaj

#ifndef MAIN_H_
#define MAIN_H_

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

//============================================================================================

typedef enum _STATE
{
    IDLE,
    CONNECTING,
    CONNECTED,
    DISCONNECTING
} STATE;

// Max packet is calculated as:
// Ether frame header (18) + Max MTU (1500) + CRC (4)
#define MAX_PACKET_SIZE 1522
#define IP_ADD_LENGTH 4
#define HW_ADD_LENGTH 6
#define GATEWAY_IP 192,168,1,1

#define IP_EEPROM_ADD 0
#define MQTT_EEPROM_ADD 1

//============================================================================================

void initHw();

void displayConnectionInfo();
void printIP(uint8_t * IP);
void printMAC(uint8_t * MAC);

void connectMQTT(etherHeader *data);
void connectMQTTReturn();
void disconnectMQTT(etherHeader *data);
void handlePingResp();

void readIPfromEeprom(uint16_t loc, uint8_t *ip);
void SetIPfromStartup(USER_DATA *serialData, uint8_t ip[IP_ADD_LENGTH], uint16_t eepromAdd);
void SetIPfromCommand(USER_DATA *serialData, uint8_t ip[IP_ADD_LENGTH], uint16_t eepromAdd);


#endif
