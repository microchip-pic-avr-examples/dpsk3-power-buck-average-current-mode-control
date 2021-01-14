/*
 * File:   init_gpio.c
 * Author: M91406
 *
 * Created on July 8, 2019, 6:26 PM
 */


#include <xc.h>
#include "config/hal.h"
#include "config/init/init_gpio.h"

volatile uint16_t Gpio_Initialize(void) {
    
    // Reset all analog inputs to be Digital I/Os
    ANSELA = 0x0000;
    ANSELB = 0x0000;
    ANSELC = 0x0000;
    ANSELD = 0x0000;
    
    // Initialize debugging Pins
    DBGPIN1_Init();
    DBGPIN2_Init();
    DBGPIN3_Init();
    DBGPIN4_Init();
    
    return(1);
}
