/*
 * File:   app_power_control_isr.c
 * Author: M91406
 *
 * Created on May 26, 2020, 7:32 PM
 */


#include "config/hal.h"
#include "app_power_control.h"

/*********************************************************************************
 * @fn      void _BUCK_VLOOP_Interrupt(void)
 * @ingroup app-layer-power-control-events
 * @brief   Main Control Interrupt
 * @param   void
 * @return  void
 *   
 * @details
 * The control interrupt is calling the control loop. The point in time where
 * this interrupt is thrown is determined by selecting the BUCK_VOUT_TRIGGER_MODE
 * option. 
 * 
 *********************************************************************************/

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

// end of file
