// EEPROM functions
// Jason Losh

//=====================================================================================================
// Hardware Target
//=====================================================================================================

// Target uC:       TM4C123GH6PM
// System Clock:    -

// Stored:
//      IP address       @ addr 0
//      MQTT address     @ addr 1

//=====================================================================================================
// Device includes, defines, and assembler directives
//=====================================================================================================

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "SYSTEM/eeprom.h"


//=====================================================================================================
// Subroutines
//=====================================================================================================

void initEeprom(void)
{
    SYSCTL_RCGCEEPROM_R = 1;
    _delay_cycles(3);
    while (EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);
}

void writeEeprom(uint16_t add, uint32_t data)
{
    EEPROM_EEBLOCK_R = add >> 4;
    EEPROM_EEOFFSET_R = add & 0xF;
    EEPROM_EERDWR_R = data;
    while (EEPROM_EEDONE_R & EEPROM_EEDONE_WORKING);
}

uint32_t readEeprom(uint16_t add)
{
    EEPROM_EEBLOCK_R = add >> 4;
    EEPROM_EEOFFSET_R = add & 0xF;
    return EEPROM_EERDWR_R;
}

// Clear static IP and MQTT IP address
void clearEeprom()
{
    writeEeprom(0, 0);
    writeEeprom(1, 0);
}


