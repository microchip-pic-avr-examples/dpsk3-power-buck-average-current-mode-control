/*
 * File:   app_power_control_isr.c
 * Author: M91406
 *
 * Created on May 26, 2020, 7:32 PM
 */


#include "config/hal.h"
#include "app_power_control.h"

/*!Power Converter Control Loop Interrupt
 * **************************************************************************************************
 * 
 * **************************************************************************************************/

/* @@_BUCK_VLOOP_Interrupt
 * ********************************************************************************
 * Summary: Main Control Interrupt
 * 
 * Parameters:
 *  (none)
 * 
 * Returns:
 *  (none)
 * 
 * Description:
 * The control interrupt is calling the control loop. The point in time where
 * this interrupt is thrown is determined by selecting the BUCK_VOUT_TRIGGER_MODE
 * option. 
 * 
 * ********************************************************************************/

/*!Power Converter Control Loop Interrupt
 * **************************************************************************************************
 * 
 * **************************************************************************************************/

void __attribute__((__interrupt__, no_auto_psv, context))_BUCK_VLOOP_Interrupt(void)
{
DBGPIN_2_SET;

    buck.status.bits.adc_active = true;
    #if (PLANT_MEASUREMENT == false)
    buck.v_loop.ctrl_Update(buck.v_loop.controller);
    #else
    v_loop_PTermUpdate(&v_loop);
    #endif
    PG1STATbits.UPDREQ = 1;  // Force PWM timing update
    _BUCK_VLOOP_ISR_IF = 0;  // Clear the ADCANx interrupt flag 

DBGPIN_2_CLEAR;
}
