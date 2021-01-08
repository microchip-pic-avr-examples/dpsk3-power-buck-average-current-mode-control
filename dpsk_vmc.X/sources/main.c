/*
 * File:   main.c
 * Author: M91406
 *
 * Created on July 8, 2019, 1:52 PM
 */

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h> // include standard integer data types
#include <stdbool.h> // include standard boolean data types

#include "config/apps.h" // include application layer header
#include "config/hal.h" // include hardware abstraction layer header

// Task managing variables
#define TMR1_TIMEOUT        30000   // Timeout protection for Timer1 interrupt flag bit
volatile bool LOW_PRIORITY_GO = false;  // Flag allowing low priority tasks to be executed

// 

/* @@main
 * ********************************************************************************
 * Summary:
 *   Main application function
 * 
 * Parameters:
 *   (none)
 * 
 * Returns:
 *   signed integer int
 * 
 * Description:
 *   This is the main application function executed after the device came
 *   out of RESET. After the fundamental device configuration and initialization
 *   of all required peripherals and software components, the function enters
 *   the endless main() loop where the device will remain executing the 
 *   application.
 * 
 * ********************************************************************************/

int main(void) {

    volatile uint16_t retval = 0;
    volatile uint16_t timeout = 0;

    // Fundamental device initialization
    retval &= Fosc_Initialize();        // Set up system oscillator for 100 MIPS operation
    retval &= Aclk_Initialize();        // Set up Auxiliary PLL for 500 MHz (source clock to PWM module)
    retval &= Dsp_Initialize();         // Set up DSP for compensator computation
    retval &= Timer1_Initialize();      // Set up Timer1 as scheduler time base
    retval &= Gpio_Initialize();        // Initialize common device GPIOs
    
    // Initialize firmware modules
    retval &= appLCD_Initialize(); // Initialize LC Display task
    retval &= appLED_Initialize(); // Initialize Debugging LED task
    retval &= appSwitch_Initialize(); // Initialize user switch button
    retval &= appPowerSupply_Initialize(); // Initialize BUCK converter object and state machine
    retval &= appFaultMonitor_Initialize(); // Initialize fault objects and fault handler task
    
    
    // Enable Timer1 generating the task execution clock
    T1CONbits.TON = 1; 

    _T1IP = 1;  // Set interrupt priority to zero
    _T1IF = 0;  // Reset interrupt flag bit
    _T1IE = 1;  // Disable Timer1 interrupt
    
    DBGPIN_2_CLEAR; // Clear debug pin #2
    DBGPIN_3_CLEAR; // Clear debug pin #3
    
    while (1) {

        // wait for timer1 to overrun
        while ((!LOW_PRIORITY_GO) && (timeout++ < TMR1_TIMEOUT));
        LOW_PRIORITY_GO = false;
        timeout = 0;    // Reset timeout counter

        DBGPIN_3_SET; // Set DEBUG-PIN

        // Execute non-time critical, low-priority tasks
        appLCD_Execute();
        appLED_Execute();
        appSwitch_Execute();
        
        DBGPIN_3_CLEAR; // Clear DEBUG-PIN
        Nop();
    }


    return (0);
}

/* @@_T1Interrupt
 * ********************************************************************************
 * Summary:
 *   High priority task sequence executed on a fixed 100 usec pace
 * 
 * Parameters:
 *   (none)
 * 
 * Returns:
 *   (none)
 * 
 * Description:
 *   This application executes five major tasks of which two are time 
 *   critical and three, which are insensitive against execution time
 *   jitter. The following interrupt will enforce the execution of the
 *   two time critical tasks over the execution of non-time critical
 *   tasks.
 * 
 * ********************************************************************************/

void __attribute__((__interrupt__, no_auto_psv)) _T1Interrupt(void)
{
    DBGPIN_1_SET; // Set DEBUG-PIN
    
    // Execute high priority, time critical tasks
    appPowerSupply_Execute(); // Execute power supply state machine
    appFaultMonitor_Execute(); // Execute the fault handler
    
    LOW_PRIORITY_GO = true; // Set GO trigger for low priority tasks
    
    _T1IF = 0; // Reset the interrupt flag bit
    DBGPIN_1_CLEAR; // Clear DEBUG-PIN
}


