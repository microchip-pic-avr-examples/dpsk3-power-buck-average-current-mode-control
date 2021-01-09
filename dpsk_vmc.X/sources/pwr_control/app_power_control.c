/*
 * File:   pwr_control.c
 * Author: M91406
 *
 * Created on March 12, 2020, 11:55 AM
 */

#include <stdbool.h>

#include "./pwr_control/drivers/v_loop.h"
#include "./pwr_control/devices/dev_buck_typedef.h"
#include "devices/dev_buck_converter.h"
#include "config/hal.h"

#include "fault_handler/app_fault_monitor.h"

/*!BUCK_POWER_CONTROLLER_t data structure
 * *************************************************************************************************
 * Summary:
 * Global data object for a BUCK CONVERTER 
 * 
 * Description:
 * The 'buck' data object holds all status, control and monitoring values of the BUCK power 
 * controller. The BUCK_POWER_CONTROLLER_t data structure is defined in dev_buck_converter.h.
 * Please refer to the comments on top of this file for further information.
 *  
 * *************************************************************************************************/
volatile struct BUCK_POWER_CONTROLLER_s buck;

/* PRIVATE FUNCTION PROTOTYPES */
volatile uint16_t appPowerSupply_ConverterObjectInitialize(void);
volatile uint16_t appPowerSupply_ControllerInitialize(void);
volatile uint16_t appPowerSupply_PeripheralsInitialize(void);

/* *************************************************************************************************
 * PRIVATE VARIABLE DECLARATIONS
 * ************************************************************************************************/
#define ISNS_AVG_BITMASK    (uint16_t)0x0007
volatile uint16_t _isns_sample_count = 0;
volatile uint16_t isns_samples;

/* *************************************************************************************************
 * PUBLIC FUNCTIONS
 * ************************************************************************************************/

volatile uint16_t appPowerSupply_Initialize(void)
{ 
    volatile uint16_t retval=1;

    // Run initialization sequence
    retval &= appPowerSupply_ConverterObjectInitialize();
    retval &= appPowerSupply_ControllerInitialize();
    retval &= appPowerSupply_PeripheralsInitialize();

    // Initialize Control Interrupt
    _BUCK_VLOOP_ISR_IP = 5;
    _BUCK_VLOOP_ISR_IF = 0;
    _BUCK_VLOOP_ISR_IE = 1;
    
    // Enable Buck Converter
    retval &= drv_BuckConverter_Start(&buck);
    //buck.status.bits.enabled = true;
    
    return(retval); 
}

volatile uint16_t appPowerSupply_Execute(void)
{ 
    volatile uint16_t retval=1;

    // Capture data values
    buck.data.v_in = BUCK_VIN_ADCBUF;
    buck.data.i_sns[0] = BUCK_ISNS_ADCBUF;
    buck.data.temp = BUCK_TEMP_ADCBUF;
    
    // Average inductor current value
    isns_samples += buck.data.i_sns[0];
    if(!(++_isns_sample_count & ISNS_AVG_BITMASK))
    {
        buck.data.i_out = (isns_samples >> 3);
        isns_samples = 0;
    }
    
    // Execute buck converter state machine
    retval &= drv_BuckConverter_Execute(&buck);

    // Buck regulation error is only active while controller is running
    // and while being tied to a valid reference
    if( (buck.state_id.bits.opstate_id >= BUCK_OPSTATE_RAMPUP) &&
        (buck.state_id.bits.substate_id >= BUCK_OPSTATE_V_RAMP_UP) )
    {
        fltobj_BuckRegErr.ReferenceObject.ptrObject = buck.v_loop.controller->Ports.ptrControlReference;
        #if (PLANT_MEASUREMENT == false)
        fltobj_BuckRegErr.Status.bits.Enabled = buck.v_loop.controller->status.bits.enabled;
        #endif
    }
    else {
        fltobj_BuckRegErr.Status.bits.Enabled = false;
    }
    
    return(retval); 
}

volatile uint16_t appPowerSupply_Dispose(void)
{ 
    volatile uint16_t _i=0;
    volatile uint16_t retval=1;

    // Shut-down Buck Converter for good
    retval &= drv_BuckConverter_Stop(&buck);    
    
    // Clear all data of buck converter
    buck.status.value = 0;
    
    for(_i=0; _i<buck.set_values.no_of_phases; _i++)
    { buck.data.i_sns[0] = 0; }
    
    buck.data.i_out = 0;
    buck.data.v_out = 0;
    buck.data.v_in = 0;
    buck.state_id.value = BUCK_OPSTATE_INITIALIZE; // Reset state machine
    buck.set_values.no_of_phases = 0; // Delete number of phases
        
    return(retval); 
}

volatile uint16_t appPowerSupply_Suspend(void)
{ 
    volatile uint16_t retval=1;

    retval = drv_BuckConverter_Suspend(&buck); // Suspend buck converter operation immediately

    return(retval); 
}

volatile uint16_t appPowerSupply_Resume(void)
{ 
    volatile uint16_t retval=0;

    buck.status.bits.enabled = true;            // Ensure Buck Converter is enabled
    retval = drv_BuckConverter_Resume(&buck);   // Resume power supply operation
        
    return(retval); 
}

/* *************************************************************************************************
 * PRIVATE FUNCTIONS
 * ************************************************************************************************/

volatile uint16_t appPowerSupply_ConverterObjectInitialize(void)
{
    volatile uint16_t retval = 1;
    
    // Initialize Buck Converter Object Status
    buck.status.bits.ready = false; // Clear READY flag
    buck.status.bits.adc_active = false; // Clear ADC STARTED flag
    buck.status.bits.pwm_active = false; // clear PWM STARTED flag
    buck.status.bits.fault_active = true; // Set global FAULT flag
    buck.status.bits.autorun = true;    // Allow the buck converter to start automatically when cleared of faults
    buck.status.bits.enabled = false; // Disable buck converter
 
    // Set Initial State Machine State
    buck.state_id.value = BUCK_OPSTATE_INITIALIZE; // Reset Buck State Machine
    
    // Set Reference values
    buck.set_values.control_mode = BUCK_CONTROL_MODE_VMC; // Set Control Mode
    buck.set_values.no_of_phases = BUCK_NO_OF_PHASES; // Set number of power train phases
    buck.set_values.i_ref = BUCK_ISNS_REF; // Set current loop reference
    buck.set_values.v_ref = BUCK_VOUT_REF; // Set voltage loop reference
    
    // Clear Runtime Data
    buck.data.v_out = 0; // Reset output voltage value
    buck.data.i_sns[0] = 0; // Reset output current value
    buck.data.v_in = 0;  // Reset input voltage value
    buck.data.temp = 0;  // Reset output temperature value
    
    // Initialize Switch Node
    buck.sw_node[0].pwm_instance = BUCK_PWM_CHANNEL;
    buck.sw_node[0].gpio_instance = BUCK_PWM_GPIO_INSTANCE;
    buck.sw_node[0].gpio_high = BUCK_PWM_GPIO_PORT_PINH;
    buck.sw_node[0].gpio_low = BUCK_PWM_GPIO_PORT_PINL;
    buck.sw_node[0].master_period_enable = false;
    buck.sw_node[0].period = BUCK_PWM_PERIOD;
    buck.sw_node[0].phase = BUCK_PWM_PHASE_SHIFT;
    buck.sw_node[0].duty_ratio_min = BUCK_PWM_DC_MIN;
    buck.sw_node[0].duty_ratio_init = BUCK_PWM_DC_MIN;
    buck.sw_node[0].duty_ratio_max = BUCK_PWM_DC_MAX;
    buck.sw_node[0].dead_time_rising = BUCK_PWM_DEAD_TIME_LE;
    buck.sw_node[0].dead_time_falling = BUCK_PWM_DEAD_TIME_FE;
    buck.sw_node[0].leb_period = BUCK_LEB_PERIOD;
    buck.sw_node[0].trigger_offset = BUCK_PWM_ADTR1OFS;
    buck.sw_node[0].trigger_scaler = BUCK_PWM_ADTR1PS;
    buck.sw_node[0].high_resolution_enable = PWM_CLOCK_HIGH_RESOLUTION;

    // Initialize additional GPIOs 
    
    // ~~~ EXTERNAL ENABLE INPUT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    buck.gpio.EnableInput.enabled = false; // this converter doesn't support external enable control
    // ~~~ EXTERNAL ENABLE INPUT END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // ~~~ POWER GOOD OUTPUT ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    buck.gpio.PowerGood.enabled = true; // This converter supports an additional POWER GOOD output
    buck.gpio.PowerGood.port = PWRGOOD_PORT; // Number of the GPIO port (0=A, 1=B, 2=C, etc.)
    buck.gpio.PowerGood.pin = PWRGOOD_PIN; // Number of the GPIO port pin
    buck.gpio.PowerGood.polarity = 0;   // This pin is ACTIVE HIGH (only required if io_type = OUTPUT)
    buck.gpio.PowerGood.io_type = 0;    // This pin is configured as OUTPUT
    
    // ~~~ POWER GOOD OUTPUT END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


    // Initialize Feedback Channels
    
    // ~~~ OUTPUT VOLTAGE FEEDBACK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    buck.feedback.ad_vout.enabled = true;   // Use this channel

    buck.feedback.ad_vout.adc_input = BUCK_VOUT_ADCIN;
    buck.feedback.ad_vout.adc_core = BUCK_VOUT_ADCCORE;
    buck.feedback.ad_vout.adc_buffer = &BUCK_VOUT_ADCBUF;
    buck.feedback.ad_vout.trigger_source = BUCK_VOUT_TRGSRC;

    buck.feedback.ad_vout.differential_input = false;
    buck.feedback.ad_vout.interrupt_enable = true;
    buck.feedback.ad_vout.early_interrupt_enable = true;
    buck.feedback.ad_vout.level_trigger = true;
    buck.feedback.ad_vout.signed_result = false;
    
    buck.feedback.ad_vout.scaling.factor = BUCK_VOUT_NORM_FACTOR;
    buck.feedback.ad_vout.scaling.scaler = BUCK_VOUT_NORM_SCALER;
    buck.feedback.ad_vout.scaling.offset = BUCK_VOUT_OFFSET;

    BUCK_VOUT_ANSEL = buck.feedback.ad_vout.enabled;
    
    // ~~~ OUTPUT VOLTAGE FEEDBACK END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // ~~~ INPUT VOLTAGE FEEDBACK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    buck.feedback.ad_vin.enabled = true;   // Use this channel

    buck.feedback.ad_vin.adc_input = BUCK_VIN_ADCIN;
    buck.feedback.ad_vin.adc_core = BUCK_VIN_ADCCORE;
    buck.feedback.ad_vin.adc_buffer = &BUCK_VIN_ADCBUF;
    buck.feedback.ad_vin.trigger_source = BUCK_VIN_TRGSRC;

    buck.feedback.ad_vin.differential_input = false;
    buck.feedback.ad_vin.interrupt_enable = false;
    buck.feedback.ad_vin.early_interrupt_enable = false;
    buck.feedback.ad_vin.level_trigger = false;
    buck.feedback.ad_vin.signed_result = false;

    buck.feedback.ad_vin.scaling.factor = BUCK_VIN_NORM_FACTOR;
    buck.feedback.ad_vin.scaling.scaler = BUCK_VIN_NORM_SCALER;
    buck.feedback.ad_vin.scaling.offset = BUCK_VIN_OFFSET;

    BUCK_VIN_ANSEL = buck.feedback.ad_vin.enabled;
    
    // ~~~ INPUT VOLTAGE FEEDBACK END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // ~~~ OUTPUT CURRENT FEEDBACK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    buck.feedback.ad_isns[0].enabled = true;   // Use this channel

    buck.feedback.ad_isns[0].adc_input = BUCK_ISNS_ADCIN;
    buck.feedback.ad_isns[0].adc_core = BUCK_ISNS_ADCCORE;
    buck.feedback.ad_isns[0].adc_buffer = &BUCK_ISNS_ADCBUF;
    buck.feedback.ad_isns[0].trigger_source = BUCK_ISNS_TRGSRC;

    buck.feedback.ad_isns[0].differential_input = false;
    buck.feedback.ad_isns[0].interrupt_enable = false;
    buck.feedback.ad_isns[0].early_interrupt_enable = false;
    buck.feedback.ad_isns[0].level_trigger = true;
    buck.feedback.ad_isns[0].signed_result = false;

    buck.feedback.ad_isns[0].scaling.factor = BUCK_ISNS_NORM_FACTOR;
    buck.feedback.ad_isns[0].scaling.scaler = BUCK_ISNS_NORM_SCALER;
    buck.feedback.ad_isns[0].scaling.offset = BUCK_ISNS1_OFFFSET;

    BUCK_ISNS_ANSEL = buck.feedback.ad_isns[0].enabled;
    
    // ~~~ OUTPUT CURRENT FEEDBACK END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // ~~~ TEMPERATURE FEEDBACK ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    buck.feedback.ad_temp.enabled = true;   // Use this channel

    buck.feedback.ad_temp.adc_input = BUCK_TEMP_ADCIN;
    buck.feedback.ad_temp.adc_core = BUCK_TEMP_ADCCORE;
    buck.feedback.ad_temp.adc_buffer = &BUCK_TEMP_ADCBUF;
    buck.feedback.ad_temp.trigger_source = BUCK_TEMP_TRGSRC;

    buck.feedback.ad_temp.differential_input = false;
    buck.feedback.ad_temp.interrupt_enable = false;
    buck.feedback.ad_temp.early_interrupt_enable = false;
    buck.feedback.ad_temp.level_trigger = false;
    buck.feedback.ad_temp.signed_result = false;

    BUCK_TEMP_ANSEL = buck.feedback.ad_temp.enabled;
    
    // ~~~ TEMPERATURE FEEDBACK END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

    // Initialize Startup Settings
    
    buck.startup.power_on_delay.counter = 0;
    buck.startup.power_on_delay.period = BUCK_POD;
    buck.startup.power_on_delay.ref_inc_step = 0;
    buck.startup.power_on_delay.reference = 0;
    
    buck.startup.v_ramp.counter = 0;
    buck.startup.v_ramp.period = BUCK_VRAMP_PER;
    buck.startup.v_ramp.ref_inc_step = BUCK_VREF_STEP;
    if (buck.startup.v_ramp.ref_inc_step == 0)
        buck.startup.v_ramp.ref_inc_step = 1;
    buck.startup.v_ramp.reference = 0;
    
    if (buck.set_values.control_mode == BUCK_CONTROL_MODE_ACMC) 
    {
        buck.startup.i_ramp.counter = 0;
        buck.startup.i_ramp.period = BUCK_IRAMP_PER;
        buck.startup.i_ramp.ref_inc_step = BUCK_IREF_STEP;
        buck.startup.i_ramp.reference = 0;
    }
    
    buck.startup.power_good_delay.counter = 0;
    buck.startup.power_good_delay.period = BUCK_PGD;
    buck.startup.power_good_delay.ref_inc_step = 0;
    buck.startup.power_good_delay.reference = BUCK_VOUT_REF;
    
    
    return(retval);
}

volatile uint16_t appPowerSupply_PeripheralsInitialize(void)
{
    volatile uint16_t retval=1;
    
    retval &= drv_BuckConverter_Initialize(&buck);
 
    return(retval);
}

volatile uint16_t appPowerSupply_ControllerInitialize(void)
{
    volatile uint16_t retval = 1;
    
    // ~~~ VOLTAGE LOOP CONFIGURATION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    // Initialize Default Loop Configuration
    buck.v_loop.feedback_offset = BUCK_VOUT_OFFSET;
    buck.v_loop.reference = BUCK_VOUT_REF;
    buck.v_loop.minimum = BUCK_PWM_DC_MIN;
    buck.v_loop.maximum = BUCK_PWM_DC_MAX;
    buck.v_loop.trigger_offset = ((BUCK_PWM_PERIOD >> 1) + BUCK_VOUT_ADC_TRGDLY);
    
     // Set Controller Object of Voltage Loop
    buck.v_loop.controller = &v_loop;
    buck.v_loop.ctrl_Initialize = &v_loop_Initialize;
    buck.v_loop.ctrl_Update = &v_loop_Update;
    buck.v_loop.ctrl_Reset = &v_loop_Reset;
    buck.v_loop.ctrl_Precharge = &v_loop_Precharge;
    
    // Configure Voltage Loop Controller Object
    buck.v_loop.ctrl_Initialize(&v_loop);   // Call Initialization Routine setting histories and scaling
    
    // Configure controller input ports
    buck.v_loop.controller->Ports.Source.ptrAddress = &BUCK_VOUT_ADCBUF; // Output Voltage is Common Source
    buck.v_loop.controller->Ports.Source.Offset = buck.v_loop.feedback_offset; // Output Voltage feedback signal offset 
    buck.v_loop.controller->Ports.Source.NormScaler = BUCK_VOUT_NORM_SCALER; // Output voltage normalization factor bit-shift scaler 
    buck.v_loop.controller->Ports.Source.NormFactor = BUCK_VOUT_NORM_FACTOR; // Output voltage normalization factor fractional
    
    buck.v_loop.controller->Ports.AltSource.ptrAddress = &BUCK_VIN_ADCBUF; // Input Voltage Is Alternate Source
    buck.v_loop.controller->Ports.AltSource.Offset = BUCK_VIN_OFFSET; // Input Voltage feedback signal offset 
    buck.v_loop.controller->Ports.AltSource.NormScaler = BUCK_VIN_NORM_SCALER; // Input voltage normalization factor bit-shift scaler 
    buck.v_loop.controller->Ports.AltSource.NormFactor = BUCK_VIN_NORM_FACTOR; // Input voltage normalization factor fractional

    // Configure controller output ports
    buck.v_loop.controller->Ports.Target.ptrAddress = &BUCK_PWM_PDC; // PWM Duty Cycle is Control Target
    buck.v_loop.controller->Ports.Target.Offset = 0; // Static primary output value offset
    buck.v_loop.controller->Ports.Target.NormScaler = 0; // Primary control output normalization factor bit-shift scaler 
    buck.v_loop.controller->Ports.Target.NormFactor = 0x7FFF; // Primary control output normalization factor fractional 

    buck.v_loop.controller->Ports.AltTarget.ptrAddress = NULL; // No alternate target used
    buck.v_loop.controller->Ports.AltTarget.Offset = 0; // Static secondary output value offset
    buck.v_loop.controller->Ports.AltTarget.NormScaler = 0; // Secondary control output normalization factor bit-shift scaler
    buck.v_loop.controller->Ports.AltTarget.NormFactor = 0x7FFF; // Secondary control output normalization factor fractional 
    
    // Configure controller control ports
    buck.v_loop.controller->Ports.ptrControlReference = &buck.set_values.v_ref; // Set pointer to Reference
    
    // Data Input/Output Limit Configuration
    buck.v_loop.controller->Limits.MinOutput = buck.v_loop.minimum;
    buck.v_loop.controller->Limits.MaxOutput = buck.v_loop.maximum;
    buck.v_loop.controller->Limits.AltMinOutput = 0; // not used
    buck.v_loop.controller->Limits.AltMaxOutput = 0; // not used

    // ADC Trigger Control Configuration
    buck.v_loop.controller->ADCTriggerControl.ptrADCTriggerARegister = &BUCK_VOUT_ADCTRIG;
    buck.v_loop.controller->ADCTriggerControl.ADCTriggerAOffset = buck.v_loop.trigger_offset;
    buck.v_loop.controller->ADCTriggerControl.ptrADCTriggerBRegister = &BUCK_VIN_ADCTRIG;
    buck.v_loop.controller->ADCTriggerControl.ADCTriggerBOffset = BUCK_ISNS_ADC_TRGDLY; //buck.v_loop.trigger_offset;
    
    // Data Provider Configuration
    buck.v_loop.controller->DataProviders.ptrDProvControlInput = &buck.data.control_input; //NULL;
    buck.v_loop.controller->DataProviders.ptrDProvControlInputCompensated = &buck.data.v_out; 
    buck.v_loop.controller->DataProviders.ptrDProvControlError = &buck.data.control_error; //NULL;
    buck.v_loop.controller->DataProviders.ptrDProvControlOutput = &buck.data.control_output; //NULL;
    
    // Cascaded Function Configuration
    buck.v_loop.controller->ExtensionHooks.ptrExtHookStartFunction = NULL;
    buck.v_loop.controller->ExtensionHooks.ExtHookStartFunctionParam = 0;
    buck.v_loop.controller->ExtensionHooks.ptrExtHookSourceFunction = NULL;
    buck.v_loop.controller->ExtensionHooks.ExtHookSourceFunctionParam = 0;
    buck.v_loop.controller->ExtensionHooks.ptrExtHookTargetFunction = NULL;
    buck.v_loop.controller->ExtensionHooks.ExtHookTargetFunctionParam = 0;
    buck.v_loop.controller->ExtensionHooks.ptrExtHookStopFunction = NULL;
    buck.v_loop.controller->ExtensionHooks.ExtHookStopFunctionParam = 0;
    buck.v_loop.controller->ExtensionHooks.ptrExtHookEndFunction = NULL;
    buck.v_loop.controller->ExtensionHooks.ExtHookEndFunctionParam = 0;
    
    // Initialize Advanced Control Settings (not used in this code example)
    buck.v_loop.controller->GainControl.AgcFactor = 0x7FFF; // Adaptive Gain Control factor fractional
    buck.v_loop.controller->GainControl.AgcScaler = 0x0000; // Adaptive Gain Control factor bit-shift scaler
    buck.v_loop.controller->GainControl.AgcMedian = 0x0000; // Q15 number representing normalized Nominal Operating Point

    // Custom Advanced Control Settings
    buck.v_loop.controller->Advanced.usrParam0 = 0; // No additional advanced control options used
    buck.v_loop.controller->Advanced.usrParam1 = 0; // No additional advanced control options used
    buck.v_loop.controller->Advanced.usrParam2 = 0; // No additional advanced control options used
    buck.v_loop.controller->Advanced.usrParam3 = 0; // No additional advanced control options used
    buck.v_loop.controller->Advanced.usrParam4 = 0; // No additional advanced control options used
    buck.v_loop.controller->Advanced.usrParam5 = 0; // No additional advanced control options used
    buck.v_loop.controller->Advanced.usrParam6 = 0; // No additional advanced control options used
    buck.v_loop.controller->Advanced.usrParam7 = 0; // No additional advanced control options used
    
    // Reset Controller Status
    buck.v_loop.controller->status.bits.enabled = false; // Keep controller disabled
    buck.v_loop.controller->status.bits.swap_source = false; // use SOURCE as major control input
    buck.v_loop.controller->status.bits.swap_target = false; // use TARGET as major control output
    buck.v_loop.controller->status.bits.invert_input = false; // Do not invert input value
    buck.v_loop.controller->status.bits.lower_saturation_event = false; // Reset Anti-Windup Minimum Status bit
    buck.v_loop.controller->status.bits.upper_saturation_event = false; // Reset Anti-Windup Minimum Status bits
    buck.v_loop.controller->status.bits.agc_enabled = false;   // Enable Adaptive Gain Modulation by default

    // ~~~ VOLTAGE LOOP CONFIGURATION END ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    
    
    return(retval);
}


