/* Microchip Technology Inc. and its subsidiaries.  You may use this software 
 * and any derivatives exclusively with Microchip products. 
 * 
 * THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS".  NO WARRANTIES, WHETHER 
 * EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED 
 * WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A 
 * PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION 
 * WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
 *
 * IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
 * INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
 * WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS 
 * BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE 
 * FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS 
 * IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF 
 * ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 * MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE 
 * TERMS. 
 */

/* 
 * @file   dpsk3_hwdescr.h
 * Author: M91406
 * @brief  DPSK3 Hardware Descriptor header file
 * Revision history: 
 */

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef DPSK3_HARDWARE_DESCRIPTOR_H
#define	DPSK3_HARDWARE_DESCRIPTOR_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stdint.h> // include standard integer data types
#include <stdbool.h> // include standard boolean data types
#include <stddef.h> // include standard definition data types
#include <math.h> // include standard math functions library

/**************************************************************************************************
 * @ingroup hardware-id-macro
 * @{
 * @brief Global macro identifying the hardware version supported by 
 * 
 * @details
 * This global macro is used to identify the hardware version supported by this hardware 
 * abstraction layer header file. 
 * 
 **************************************************************************************************/

#ifndef __DPSK3_R30__
  #define __DPSK3_R30__
#endif

/** @} */ // end of group hardware-id-macro

/**************************************************************************************************
 * @ingroup special-options
 * @{
 * @brief Global defines used to enable/disable special firmware options
 * 
 * @details
 * This section is used to enable/disable special options of the firmware. 
 * 
 **************************************************************************************************/

/* CUSTOM RUNTIME OPTIONS */
#define PLANT_MEASUREMENT   false ///< If enabled, replaces the common voltage control loop by a simple P-control loop to perform measurements of the plant transfer function.

#define DBGPIN1_ENABLE      true ///< Enables debug pin indicating control loop execution timing
#define DBGPIN2_ENABLE      true ///< Enables debug pin indicating control loop execution timing
#define DBGPIN3_ENABLE      true ///< Enables debug pin indicating task scheduler execution timing
#define DBGPIN4_ENABLE      true ///< Enables debug pin indicating task scheduler execution timing


/**************************************************************************************************
 * @ingroup device-abstraction-settings
 * @{ 
 * @brief Fundamental microcontroller device settings
 * 
 * @details
 * This section is used to define device specific parameters like ADC reference and
 * resolution, main execution clock frequency and peripheral time base settings. 
 * All parameters are defined using physical quantities. 
 * 
 **************************************************************************************************/
#define CPU_FREQUENCY       (float)100000000.0  ///< CPU frequency in [Hz]

// ADC(DAC Reference and Resolution Settings    
#define ADC_REFERENCE       (float)3.300 ///< ADC reference voltage in V
#define ADC_RESOLUTION      (float)12.0  ///< ADC resolution in [bit]

// PWM/ADC Clock Settings    
#define PWM_CLOCK_FREQUENCY (float)4.0e+9   ///< PWM Clock Frequency in [Hz]


/** @} */ // end of group device-abstraction-settings

/**
 * @ingroup device-abstraction-macros
 * @{ 
 * @brief Conversion macros of fundamental microcontroller device settings
 * 
 * @details
 * This section is used to convert device specific parameters like ADC reference and
 * resolution, main execution clock frequency and peripheral time base settings, declared 
 * in physical quantities, into binary (integer) numbers to be written to variables and SFRs.
 */

#define CPU_TCY             (float)(1.0/CPU_FREQUENCY) ///< Instruction period
#define ADC_GRANULARITY     (float)(ADC_REFERENCE / pow(2.0, ADC_RESOLUTION)) ///< ADC granularity in [V/tick]
#define ADC_VALUE_MAX       (uint16_t) (pow(2.0, ADC_RESOLUTION) - 1.0) // DO NOT CHANGE
#define PWM_CLOCK_PERIOD    (float)(1.0/PWM_CLOCK_FREQUENCY) ///< PWM Clock Period in [sec]

/** @} */ // end of group device-abstraction-macros


// PWM/ADC Clock Settings  
#define PWM_CLOCK_HIGH_RESOLUTION   true
    
#if (PWM_CLOCK_HIGH_RESOLUTION)
    #define PWM_CLOCK_FREQUENCY (float)4.0e+9   ///< PWM Clock Frequency in [Hz]
    #define PWM_CLOCK_PERIOD    (float)(1.0/PWM_CLOCK_FREQUENCY) // PWM Clock Period in [sec]
#else
    #define PWM_CLOCK_FREQUENCY (float)500.0e+6   ///< PWM Clock Frequency in [Hz]
    #define PWM_CLOCK_PERIOD    (float)(1.0/PWM_CLOCK_FREQUENCY) // PWM Clock Period in [sec]
#endif

/*!State Machine Settings
 * *************************************************************************************************
 * Summary:
 * Global defines for state-machine specific parameters
 * 
 * Description:
 * This section is used to define state-machine settings such as the main execution call interval. 
 * Pre-compiler macros are used to translate physical values into binary (integer) numbers to be 
 * written to SFRs and variables.
 * 
 * *************************************************************************************************/
#define MAIN_EXECUTION_PERIOD   (float)100.0e-6     // main state machine pace period in [sec]
#define MAIN_EXEC_PER           (uint16_t)((CPU_FREQUENCY * MAIN_EXECUTION_PERIOD)-1)

/**
 * @ingroup state-machine-mcal
 * @{
 * @brief Global state-machine peripheral assignments
 * 
 * @details
 * The main task scheduler time base required a timer interrupt to separate high-priority
 * from low-priority tasks. The high priority task timer interrupt is configured here.
 */

#define _OsTimerInterrupt      _T1Interrupt ///< Interrupt serivce routine label 
#define _OSTIMER_IP            _T1IP ///< interrupt priority register
#define _OSTIMER_IE            _T1IE ///< interrupt enable bit
#define _OSTIMER_IF            _T1IF ///< interrupt flag bit
#define _OSTIMER_PRIORITY       2    ///< interrupt priority (1 ... 7, default = 2)
    

/** @} */ // end of group state-machine-mcal ~~~~~~~~~~
    
/*!Hardware Abstraction
 * *************************************************************************************************
 * Summary:
 * Global defines for hardware specific parameters
 * 
 * Description:
 * This section is used to define hardware specific parameters such as output voltage dividers,
 * reference levels or feedback gains. Pre-compiler macros are used to translate physical  
 * values into binary (integer) numbers to be written to SFRs
 * 
 * *************************************************************************************************/

#ifdef __DPSK3_R30__
    
    // TP51 on DPSK3 (Red LED)
    #define DBGLED_Set()        { _LATB6 = 1; }
    #define DBGLED_Clear()      { _LATB6 = 0; }
    #define DBGLED_Toggle()     { _LATB6 ^= 1; }
    #define DBGLED_Init()       { _LATB6 = 0; _TRISB6 = 0; }
    #define DBGLED_Dispose()    { _LATB6 = 1; _TRISB6 = 1; }
    
    // TP50 on DPSK3
    #define DBGPIN1_Set()       { _LATB5 = 1; }
    #define DBGPIN1_Clear()     { _LATB5 = 0; }
    #define DBGPIN1_Toggle()	{ _LATB5 ^= 1; }
    #define DBGPIN1_Init()      { _LATB5 = 0; _TRISB5 = 0; }

    // TP52 on DPSK3
    #define DBGPIN2_Set()       { _LATB11 = 1; }
    #define DBGPIN2_Clear()     { _LATB11 = 0; }
    #define DBGPIN2_Toggle()	{ _LATB11 ^= 1; }
    #define DBGPIN2_Init()      { _LATB11 = 0; _TRISB11 = 0; }

    // TP53 on DPSK3
    #define PWRGOOD_PORT        1   // GPIO port declaration where 0=A, 1=B, 2=C, etc.
    #define PWRGOOD_PIN         12  // GPIO port pin declaration where 0=Rx0, 1=Rx1, 2=Rx3, etc.
    #define PWRGOOD_Set()       { _LATB12 = 1; }
    #define PWRGOOD_Clear()     { _LATB12 = 0; }
    #define PWRGOOD_Toggle()	{ _LATB12 ^= 1; }
    #define PWRGOOD_Init()      { _ANSELB12 = 0; _LATB12 = 0; _TRISB12 = 0; }
    
    // User Switch Input
    #define SW_USER_TRISx       _TRISD1
    #define SW_USER_LATx        _LATD1
    #define SW_USER_PORTx       _RD1
    #define SW_USER_Init()      { SW_USER_LATx = 1; SW_USER_TRISx = 1; }

#endif
    
/*!Power Control Parameter Declaration
 * *************************************************************************************************
 * Summary:
 * Global defines for Buck Converter Power Control parameters
 * 
 * Description:
 * This section is used to define hardware specific parameters such as output voltage dividers,
 * reference levels or feedback gains. Pre-compiler macros are used to translate physical  
 * values into binary (integer) numbers to be written to SFRs
 * 
 * *************************************************************************************************/

#define SWITCHING_FREQUENCY     (float)500.0e+3   // Switching frequency in [Hz]
#define SWITCHING_PERIOD        (float)(1.0/SWITCHING_FREQUENCY)    // Switching period in [sec]
#define SWITCHING_PHASE_SHIFT   (float)0.0        // Phase Shift of PWM output in [sec]

/*!Fundamental PWM Settings
 * *************************************************************************************************
 * Summary:
 * Global defines for PWM settings of DV330101
 * 
 * Description:
 * This section defines fundamental PWM settings required for the low-voltage interleaved boost
 * PFC development board. These settings are determined by hardware. User-selectable settings 
 * are made on application layer level (e.g. in apl.h), outside this header file.
 * 
 * Pre-compiler macros are used to convert physical values into binary (integer) numbers to 
 * be written to SFRs
 * 
 * *************************************************************************************************/

#define BUCK_NO_OF_PHASES              1U     // Number of power converter phases of this design
    
#define BUCK_PWM_DUTY_CYCLE_MIN        (float)0.010 // ~1.0% On Time 
#define BUCK_PWM_DUTY_CYCLE_MAX        (float)0.900 // ~80% On Time 
#define BUCK_LEADING_EDGE_BLANKING     (float)120.0e-9 // Leading Edge Blanking in [sec]
#define BUCK_DEAD_TIME_LEADING_EDGE    (float)20.0e-9 // Leading Edge Dead Time in [sec]
#define BUCK_DEAD_TIME_FALLING_EDGE    (float)60.0e-9 // Falling Edge Dead Time in [sec]

// Buck PWM Configuration
#define BUCK_PWM_CHANNEL                1U // PWM Instance Index (e.g. 1=PWM1, 2=PWM2, etc.)
#define BUCK_PWM_GPIO_INSTANCE          1U // Number indicating device port, where 0=A, 1=B, 2=C, etc.
#define BUCK_PWM_GPIO_PORT_PINH         14U // Port Pin Number
#define BUCK_PWM_GPIO_PORT_PINL         15U // Port Pin Number
    
#define BUCK_PWM_PDC                    PG1DC    // PWM Instance Duty Cycle Register
#define BUCK_PWMH_TRIS                  _TRISB14 // Device Port TRIS register
#define BUCK_PWMH_WR                    _LATB14  // Device Pin WRITE
#define BUCK_PWMH_RD                    _RB14    // Device Pin READ
#define BUCK_PWML_TRIS                  _TRISB15 // Device Port TRIS register
#define BUCK_PWML_WR                    _LATB15  // Device Pin WRITE
#define BUCK_PWML_RD                    _RB15    // Device Pin READ

#define _BUCK_PWM_Interrupt             _PWM1Interrupt // PWM Interrupt Service Routine label
#define BUCK_PWM_IF                     _PWM1IF  // PWM Interrupt Flag Bit
#define BUCK_PWM_IE                     _PWM1IE  // PWM Interrupt Enable Bit
#define BUCK_PWM_IP                     _PWM1IP  // PWM Interrupt Priority
#define BUCK_PWM_TRGSRC_TRG1            0b00100  // PWM Trigger #1 Trigger Source of this channel
#define BUCK_PWM_TRGSRC_TRG2            0b00101  // PWM Trigger #2 Trigger Source of this channel
#define BUCK_PWM_PGxTRIGA               PG1TRIGA // PWM trigger register A
#define BUCK_PWM_PGxTRIGB               PG1TRIGB // PWM trigger register B
#define BUCK_PWM_PGxTRIGC               PG1TRIGC // PWM trigger register C
    
#define BUCK_PWM_ADTR1OFS               0U // ADC Trigger 1 Offset:  0...31
#define BUCK_PWM_ADTR1PS                0U // ADC Trigger 1 Postscaler: 0...31

    
#define BUCK_PWM_MASTER_SOCS            0b0001  // PWM1 provides Master Trigger
    
// ~~~ conversion macros ~~~~~~~~~~~~~~~~~~~~~~~~~
#define BUCK_SWITCHING_PERIOD      (float)(1.0/SWITCHING_FREQUENCY)   // Switching period in [sec]
#define BUCK_PWM_PERIOD            (uint16_t)(float)(BUCK_SWITCHING_PERIOD / PWM_CLOCK_PERIOD)
#define BUCK_PWM_PHASE_SHIFT       (uint16_t)((float)BUCK_PWM_PERIOD / (float)BUCK_NO_OF_PHASES)      
#define BUCK_PWM_DC_MIN            (uint16_t)(BUCK_PWM_DUTY_CYCLE_MIN * (float)BUCK_PWM_PERIOD) // This sets the minimum duty cycle
#define BUCK_PWM_DC_MAX            (uint16_t)(BUCK_PWM_DUTY_CYCLE_MAX * (float)BUCK_PWM_PERIOD) // This sets the maximum duty cycle
#define BUCK_LEB_PERIOD            (uint16_t)(BUCK_LEADING_EDGE_BLANKING / (float)PWM_CLOCK_PERIOD) // Leading Edge Blanking = n x PWM resolution (here: 50 x 2ns = 100ns)
#define BUCK_PWM_DEAD_TIME_LE      (uint16_t)(BUCK_DEAD_TIME_LEADING_EDGE / (float)PWM_CLOCK_PERIOD) // Rising edge dead time [tick = 250ps]
#define BUCK_PWM_DEAD_TIME_FE      (uint16_t)(BUCK_DEAD_TIME_FALLING_EDGE / (float)PWM_CLOCK_PERIOD) // Falling edge dead time [tick = 250ps]
// ~~~ conversion macros end ~~~~~~~~~~~~~~~~~~~~~


    
/*!Input Voltage Feedback
 * *************************************************************************************************
 * Summary: Declaration of input voltage limits and nominal operating points
 * 
 * Description:
 * 
 * *************************************************************************************************/
    
#define BUCK_VIN_MINIMUM        (float) 6.500   // Minimum input voltage
#define BUCK_VIN_NOMINAL        (float) 9.000   // Nominal input voltage
#define BUCK_VIN_MAXIMUM        (float)13.800   // Maximum input voltage

#define BUCK_VIN_UNDER_VOLTAGE  (float) 6.000   // Under Voltage Lock Out Cut Off
#define BUCK_VIN_OVER_VOLTAGE   (float)14.300   // Over Voltage Lock Out Cut Off
#define BUCK_VIN_HYSTERESIS     (float) 1.000   // UVLO/OVLO Hysteresis
    
#define BUCK_VIN_R1             (float)(6.980)  // Upper voltage divider resistor in kOhm
#define BUCK_VIN_R2             (float)(1.000)  // Lower voltage divider resistor in kOhm
#define BUCK_VIN_FEEDBACK_GAIN  (float)((BUCK_VIN_R2) / (BUCK_VIN_R1 + BUCK_VIN_R2))
    
#define BUCK_VIN_FEEDBACK_OFFSET    (float)(0.0)
#define BUCK_VIN_ADC_TRG_DELAY      (float)(20.0e-9) // ADC trigger delay in [sec]
    
// ~ conversion macros ~~~~~~~~~~~~~~~~~~~~~
    
#define BUCK_VIN_MIN            (uint16_t)(BUCK_VIN_MINIMUM * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY)   // Minimum input voltage
#define BUCK_VIN_NOM            (uint16_t)(BUCK_VIN_NOMINAL * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY)   // Nominal input voltage
#define BUCK_VIN_MAX            (uint16_t)(BUCK_VIN_MAXIMUM * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY)   // Maximum input voltage
#define BUCK_VIN_HYST           (uint16_t)(BUCK_VIN_HYSTERESIS * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY)  // Over Voltage LOck Out voltage    
#define BUCK_VIN_UVLO_TRIP      (uint16_t)(BUCK_VIN_UNDER_VOLTAGE * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY) // Under Voltage LOck Out voltage
#define BUCK_VIN_UVLO_RELEASE   (uint16_t)((BUCK_VIN_UNDER_VOLTAGE + BUCK_VIN_HYSTERESIS) * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY) // Under Voltage LOck Out voltage
#define BUCK_VIN_OVLO_TRIP      (uint16_t)(BUCK_VIN_OVER_VOLTAGE * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY)  // Over Voltage LOck Out voltage
#define BUCK_VIN_OVLO_RELEASE   (uint16_t)((BUCK_VIN_OVER_VOLTAGE - BUCK_VIN_HYSTERESIS) * BUCK_VIN_FEEDBACK_GAIN / ADC_GRANULARITY)  // Over Voltage LOck Out voltage
#define BUCK_VIN_ADC_TRGDLY     (uint16_t)(BUCK_VIN_ADC_TRG_DELAY / PWM_CLOCK_PERIOD) // Input voltage ADC trigger delay
#define BUCK_VIN_OFFSET         (uint16_t)(BUCK_VIN_FEEDBACK_OFFSET / ADC_GRANULARITY) // Input voltage feedback offset

#define BUCK_VIN_NORM_INV_G     (float)(1.0/BUCK_VIN_FEEDBACK_GAIN) // Inverted feedback gain required for value normalization
#define BUCK_VIN_NORM_SCALER    (int16_t)(ceil(log(BUCK_VIN_NORM_INV_G)) + 1) // VIN normalization  
#define BUCK_VIN_NORM_FACTOR    (int16_t)((BUCK_VIN_NORM_INV_G / pow(2.0, BUCK_VIN_NORM_SCALER)) * (pow(2.0, 15)-1)) // VIN normalization factor scaled in Q15

#define BUCK_VIN_RANGE_MAX     (float)(ADC_REF * BUCK_VIN_NORM_INV_G)
    
// ~ conversion macros end ~~~~~~~~~~~~~~~~~
    
#define _BUCK_VIN_ADCInterrupt  _ADCAN12Interrupt // Interrupt service routine of input voltage
#define _BUCK_VIN_ADCISR_IF     _ADCAN12IF  // Interrupt flag bit of input voltage

#define BUCK_VIN_ANSEL          _ANSELC0    // GPIO analog function mode enable bit
#define BUCK_VIN_ADCCORE        8           // 0=Dedicated Core #0, 1=Dedicated Core #1, ... , 8=Shared ADC Core
#define BUCK_VIN_ADCIN          12          // Analog input number (e.g. '5' for 'AN5')
#define BUCK_VIN_ADCBUF         ADCBUF12    // ADC input buffer of this ADC channel
#define BUCK_VIN_ADCTRIG        PG1TRIGB    // Register used for trigger placement
#define BUCK_VIN_TRGSRC         BUCK_PWM_TRGSRC_TRG2 // PWM1 (=PG1) Trigger 2 via PGxTRIGB

/*!Output Voltage Feedback
 * *************************************************************************************************
 * Summary:
 * 
 * Description:
 * 
 * *************************************************************************************************/

// Feedback Declarations
#define BUCK_VOUT_NOMINAL           (float)3.300   // Nominal output voltage
#define BUCK_VOUT_TOLERANCE_MAX     (float)0.500   // Output voltage tolerance [+/-]
#define BUCK_VOUT_TOLERANCE_MIN     (float)0.100   // Output voltage tolerance [+/-]
    
#define BUCK_VOUT_DIV_R1            (float)(1.000) // Upper voltage divider resistor in kOhm
#define BUCK_VOUT_DIV_R2            (float)(1.000) // Lower voltage divider resistor in kOhm
#define BUCK_VOUT_FEEDBACK_GAIN     (float)((BUCK_VOUT_DIV_R2) / (BUCK_VOUT_DIV_R1 + BUCK_VOUT_DIV_R2))
#define BUCK_VOUT_FEEDBACK_OFFSET   (float)(0.0)
#define BUCK_VOUT_ADC_TRG_DELAY     (float)(20.0e-9) // Trigger delay in [sec]

// Peripheral Assignments
#define BUCK_VOUT_ANSEL         _ANSELC1    // GPIO analog function mode enable bit
#define BUCK_VOUT_ADCCORE       8           // 0=Dedicated Core #0, 1=Dedicated Core #1, 8=Shared ADC Core
#define BUCK_VOUT_ADCIN         13          // Analog input number (e.g. '5' for 'AN5')
#define BUCK_VOUT_ADCBUF        ADCBUF13    // ADC input buffer of this ADC channel
#define BUCK_VOUT_ADCTRIG       PG1TRIGA    // Register used for trigger placement
#define BUCK_VOUT_TRGSRC        BUCK_PWM_TRGSRC_TRG1 // PWM1 (=PG2) Trigger 1 via PGxTRIGA
    
// ~ conversion macros ~~~~~~~~~~~~~~~~~~~~~

#define BUCK_VOUT_REF           (uint16_t)(BUCK_VOUT_NOMINAL * BUCK_VOUT_FEEDBACK_GAIN / ADC_GRANULARITY)
#define BUCK_VOUT_NOM           BUCK_VOUT_REF
#define BUCK_VOUT_DEV_TRIP      (uint16_t)(BUCK_VOUT_TOLERANCE_MAX * BUCK_VOUT_FEEDBACK_GAIN / ADC_GRANULARITY)
#define BUCK_VOUT_DEV_RELEASE   (uint16_t)(BUCK_VOUT_TOLERANCE_MIN * BUCK_VOUT_FEEDBACK_GAIN / ADC_GRANULARITY)
#define BUCK_VOUT_OFFSET        (uint16_t)(BUCK_VOUT_FEEDBACK_OFFSET / ADC_GRANULARITY)
#define BUCK_VOUT_ADC_TRGDLY    (uint16_t)(BUCK_VOUT_ADC_TRG_DELAY / PWM_CLOCK_PERIOD)

#define BUCK_VOUT_NORM_INV_G    (float)(1.0/BUCK_VOUT_FEEDBACK_GAIN) // Inverted feedback gain required for value normalization
#define BUCK_VOUT_NORM_SCALER   (int16_t)(ceil(log(BUCK_VOUT_NORM_INV_G)) + 1) // VOUT normalization  
#define BUCK_VOUT_NORM_FACTOR   (int16_t)((BUCK_VOUT_NORM_INV_G / pow(2.0, BUCK_VOUT_NORM_SCALER)) * (pow(2.0, 15)-1)) // VOUT normalization factor scaled in Q15

#define BUCK_VOUT_RANGE_MAX     (float)(ADC_REF * BUCK_VOUT_NORM_INV_G)
    
// ~ conversion macros end ~~~~~~~~~~~~~~~~~

/*!Ideal Duty Cycle
 * *************************************************************************************************
 * Summary:
 * 
 * Description:
 * 
 * *************************************************************************************************/
#define BUCK_NORM_IDEAL_DC(x, y) (float)(((float)x/(float)y)*(((float)BUCK_VOUT_NORM_FACTOR/(float)BUCK_VIN_NORM_FACTOR)*pow(2, (BUCK_VOUT_NORM_SCALER-BUCK_VIN_NORM_SCALER))))

/*!Phase Current Feedback
 * *************************************************************************************************
 * Summary:
 * 
 * Description:
 * 
 * *************************************************************************************************/

// Feedback Declarations and Peripheral Assignments

#define BUCK_ISNS_NEED_CALIBRATION  false                   // Flag indicating that current feedback needs calibration
    
#define BUCK_ISNS_CT    0                                   // Use current sense transformer as main current feedback source
#define BUCK_ISNS_AMP   1                                   // Use shunt amplifier as main current feedback source
    
#define BUCK_ISNS_OPTION    BUCK_ISNS_CT                    // Select one of the available current sense feedback options
    

#if (BUCK_ISNS_OPTION == BUCK_ISNS_CT)

    // Feedback Declarations
    #define BUCK_ISNS_FEEDBACK_GAIN     (float) 1.000       // Current Gain in V/A
    #define BUCK_ISNS_MINIMUM           (float) 0.000       // absolute minimum output current (average)
    #define BUCK_ISNS_MAXIMUM           (float) 2.500       // absolute maximum output current (average)
    #define BUCK_ISNS_REFERENCE         (float) 2.500       // output current reference (average)
    #define BUCK_ISNS_ADC_TRG_DELAY     (float) 80.0e-9     // ADC trigger delay for current sense in [sec]
    #define BUCK_ISNS_FEEDBACK_OFFSET   (float) 0.025       // current sense feedback offset (average)

    // Peripheral Assignments
    #define _BUCK_ISNS_ADCInterrupt     _ADCAN0Interrupt   
    #define _BUCK_ISNS_ADCISR_IF        _ADCAN0IF

    #define BUCK_ISNS_ANSEL             _ANSELA0    // GPIO analog function mode enable bit
    #define BUCK_ISNS_ADCCORE           0           // 0=Dedicated Core #0, 1=Dedicated Core #1, 2=Shared ADC Core
    #define BUCK_ISNS_ADCIN             0           // Analog input number (e.g. '5' for 'AN5')
    #define BUCK_ISNS_ADCBUF            ADCBUF0     // ADC input buffer of this ADC channel
    #define BUCK_ISNS_ADCTRIG           PG1TRIGB    // Register used for trigger placement
    #define BUCK_ISNS_TRGSRC            BUCK_PWM_TRGSRC_TRG2 // PWM1 (=PG1) Trigger 1 via PGxTRIGA
    
    
#elif (BUCK_ISNS_OPTION == BUCK_ISNS_AMP)

    // Feedback Declarations
    #define BUCK_ISNS_FEEDBACK_GAIN     (float) 0.600       // Current Gain in V/A
    #define BUCK_ISNS_MINIMUM           (float) 0.000       // absolute minimum output current (average)
    #define BUCK_ISNS_MAXIMUM           (float) 2.500       // absolute maximum output current (average)
    #define BUCK_ISNS_REFERENCE         (float) 2.500       // output current reference (average)
    #define BUCK_ISNS_ADC_TRG_DELAY     (float) 240.0e-9    // ADC trigger delay for current sense in [sec]
    #define BUCK_ISNS_FEEDBACK_OFFSET   (float) 1.650       // current sense feedback offset (average)

    // Peripheral Assignments
    #define _BUCK_ISNS_ADCInterrupt     _ADCAN1Interrupt   
    #define _BUCK_ISNS_ADCISR_IF        _ADCAN1IF

    #define BUCK_ISNS_ANSEL             _ANSELB2    // GPIO analog function mode enable bit
    #define BUCK_ISNS_ADCCORE           1           // 0=Dedicated Core #0, 1=Dedicated Core #1, 2=Shared ADC Core
    #define BUCK_ISNS_ADCIN             1           // Analog input number (e.g. '5' for 'AN5')
    #define BUCK_ISNS_ADCBUF            ADCBUF1     // ADC input buffer of this ADC channel
    #define BUCK_ISNS_ADCTRIG           PG1TRIGB    // Register used for trigger placement
    #define BUCK_ISNS_TRGSRC            BUCK_PWM_TRGSRC_TRG2 // PWM1 (=PG1) Trigger 1 via PGxTRIGA
    
#else

    #pragma message "hardware abstraction layer warning: no current sense feedback selected."

#endif
    

// ~ conversion macros ~~~~~~~~~~~~~~~~~~~~~

#define BUCK_ISNS_MIN           (uint16_t)(BUCK_ISNS_MINIMUM * BUCK_ISNS_FEEDBACK_GAIN / ADC_GRANULARITY)  // Minimum Current Limit
#define BUCK_ISNS_OCP           (uint16_t)(BUCK_ISNS_MAXIMUM * BUCK_ISNS_FEEDBACK_GAIN / ADC_GRANULARITY)  // Over Current Limit
#define BUCK_ISNS_OCP_RELEASE   (uint16_t)(BUCK_ISNS_MINIMUM * BUCK_ISNS_FEEDBACK_GAIN / ADC_GRANULARITY)  // Over Current Limit Release Level
#define BUCK_ISNS_REF           (uint16_t)(BUCK_ISNS_REFERENCE * BUCK_ISNS_FEEDBACK_GAIN / ADC_GRANULARITY)  // Output Current Reference
#define BUCK_ISNS1_OFFFSET      (uint16_t)(BUCK_ISNS_FEEDBACK_OFFSET / ADC_GRANULARITY)
#define BUCK_ISNS2_OFFFSET      (uint16_t)(BUCK_ISNS_FEEDBACK_OFFSET / ADC_GRANULARITY)
#define BUCK_ISNS_ADC_TRGDLY    (uint16_t)(BUCK_ISNS_ADC_TRG_DELAY / PWM_CLOCK_PERIOD)

#define BUCK_ISNS_NORM_INV_G    (float)(1.0/BUCK_ISNS_FEEDBACK_GAIN) // Inverted feedback gain required for value normalization
#define BUCK_ISNS_NORM_SCALER   (int16_t)(ceil(log(BUCK_ISNS_NORM_INV_G)) + 1) // ISNS normalization  
#define BUCK_ISNS_NORM_FACTOR   (int16_t)((BUCK_ISNS_NORM_INV_G / pow(2.0, BUCK_ISNS_NORM_SCALER)) * (pow(2.0, 15)-1)) // ISNS normalization factor scaled in Q15
    
// ~ conversion macros end ~~~~~~~~~~~~~~~~~

#define BUCK_TEMPCAL_ZERO       (float) 0.500   // Temperature sense signal zero point voltage in [V]
#define BUCK_TEMPCAL_SLOPE      (float) 0.010   // Temperature sense signal slope in [V/K]
    
    
    
#define _BUCK_TEMP_ADCInterrupt     _ADCAN2Interrupt   
#define _BUCK_TEMP_ADCISR_IF        _ADCAN2IF

#define BUCK_TEMP_ANSEL             _ANSELB7
#define BUCK_TEMP_ADCCORE           8           // 0=Dedicated Core #0, 1=Dedicated Core #1, 8=Shared ADC Core
#define BUCK_TEMP_ADCIN             2           // Analog input number (e.g. '5' for 'AN5')
#define BUCK_TEMP_ADCBUF            ADCBUF2
#define BUCK_TEMP_ADCTRIG           PG1TRIGB
#define BUCK_TEMP_TRGSRC            BUCK_PWM_TRGSRC_TRG2     // PWM1 Trigger 2

// ~ conversion macros ~~~~~~~~~~~~~~~~~~~~~
    
#define BUCK_FB_TEMP_ZERO       (uint16_t)(BUCK_TEMPCAL_ZERO / ADC_GRANULARITY)
#define BUCK_FB_TEMP_SLOPE      (float)(BUCK_TEMPCAL_SLOPE / ADC_GRANULARITY)
    
// ~ conversion macros end ~~~~~~~~~~~~~~~~~
    
    
/*!Adaptive Gain Control Feed Forward
 * *************************************************************************************************
 * Summary:
 * 
 * Description:
 * 
 * *************************************************************************************************/
    
#define BUCK_VL_MINIMUM         (float)(BUCK_VIN_UNDER_VOLTAGE - BUCK_VOUT_RANGE_MAX) // Minimum input voltage - maximum output voltate
#define BUCK_VL_NOMINAL         (float)(BUCK_VIN_NOMINAL       - BUCK_VOUT_NOMINAL) // Nominal input voltage - nominal output voltate
#define BUCK_VL_MAXIMUM         (float)(BUCK_VIN_RANGE_MAX     - 0) // Maximum input voltage - 0

#define BUCK_VIN_NORM_FCT       (float)(BUCK_VOUT_FEEDBACK_GAIN / BUCK_VIN_FEEDBACK_GAIN)   // VIN-2-VOUT Normalization Factor

// ~ conversion macros ~~~~~~~~~~~~~~~~~~~~~
    
#define BUCK_AGC_IO_NORM_SCALER (int16_t)(ceil(log(BUCK_VIN_NORM_FCT)) + 1) // Nominal VL Q15 scaler  
#define BUCK_AGC_IO_NORM_FACTOR (int16_t)((BUCK_VIN_NORM_FCT / pow(2.0, BUCK_AGC_IO_NORM_SCALER)) * (pow(2.0, 15)-1)) // Nominal VL Q15 factor 

// The AGC compare value is defined at the lowest input voltage and highest output voltage 
// (= lowest voltage across inductor)
// ToDo: Remove - min VIN turned out to be too cumbersome when tuning coefficients to nominal conditions
//#define BUCK_AGC_MEDIAN         (int16_t)((float)BUCK_VIN_UVLO_TRIP * BUCK_VIN_NORM_FCT)

// The AGC compare value is defined at nominal input voltage and output voltage 
#define BUCK_AGC_MEDIAN         (int16_t)(((float)BUCK_VIN_NOM * BUCK_VIN_NORM_FCT) - BUCK_VOUT_NOM)

#define BUCK_AGC_FACTOR_MAX     (float)(BUCK_VL_NOMINAL / BUCK_VL_MINIMUM)
#define BUCK_AGC_NOM_SCALER     (uint16_t)(ceil(log(BUCK_AGC_FACTOR_MAX)) + 1)
#define BUCK_AGC_NOM_FACTOR     (uint16_t)(0x7FFF >> BUCK_AGC_NOM_SCALER)
// ~ conversion macros end ~~~~~~~~~~~~~~~~~

/*!Startup Behavior
 * *************************************************************************************************
 * Summary:
 * Global defines for soft-start specific parameters
 * 
 * Description:
 * This section is used to define power supply startup timing setting. The soft-start sequence 
 * is part of the power controller. It allows to program specific timings for Power On Delay,
 * Ramp Period and Power Good Delay. After the startup has passed these three timing periods,
 * the power supply is ending up in "normal" operation, continuously regulating the output until 
 * a fault is detected or the operating state is changed for any other reason.
 * 
 * Pre-compiler macros are used to translate physical values into binary (integer) numbers to 
 * be written to SFRs and variables.
 * 
 * *************************************************************************************************/

// On EPC9143 Rev 4 it takes roughly 50 ms until the auxiliary power has been started
// and the controller completes self-test and peripheral configuration. After this period
// the controller starts to execute the power control state machine.
    
// This additional startup delay of ~50 ms is not considered here and needs to be taken into 
// account when adjusting startup timing.
    
#define BUCK_POWER_ON_DELAY          (float) 250e-3 // power on delay in [sec]
#define BUCK_VRAMP_PERIOD            (float)  50e-3 // ramp period in [sec]
#define BUCK_IRAMP_PERIOD            (float)  50e-3 // ramp period in [sec]
#define BUCK_POWER_GOOD_DELAY        (float)  50e-3 // power good in [sec]

// ~ conversion macros ~~~~~~~~~~~~~~~~~~~~~

#define BUCK_POD       (uint16_t)(((float)  BUCK_POWER_ON_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_VRAMP_PER (uint16_t)(((float)    BUCK_VRAMP_PERIOD / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_VREF_STEP (uint16_t)((float)         BUCK_VOUT_REF / (float)(BUCK_VRAMP_PER + 1.0))
#define BUCK_IRAMP_PER (uint16_t)(((float)    BUCK_IRAMP_PERIOD / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_IREF_STEP (uint16_t)((float)         BUCK_ISNS_REF / (float)(BUCK_VRAMP_PER + 1.0))
#define BUCK_PGD       (uint16_t)(((float)BUCK_POWER_GOOD_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)

// ~ conversion macros end ~~~~~~~~~~~~~~~~~

/*!Fault Object Declarations
 * *************************************************************************************************
 * Summary:
 *   Global defines for fault-monitor related parameters
 * 
 * Description:
 * This section is used to define power supply fault object timing setting. The fault monitor 
 * is 
 * 
 * *************************************************************************************************/
    
#define BUCK_UVLO_TRIP_DELAY         (float) 5e-3   ///< under voltage lock out trip delay in [sec]
#define BUCK_UVLO_RECOVERY_DELAY     (float) 250e-3 ///< under voltage lock out recovery delay in [sec]
#define BUCK_OVLO_TRIP_DELAY         (float) 5e-3   ///< over voltage lock out trip delay in [sec]
#define BUCK_OVLO_RECOVERY_DELAY     (float) 250e-3 ///< over voltage lock out recovery delay in [sec]
#define BUCK_REGERR_TRIP_DELAY       (float) 25e-3  ///< regulation error trip delay in [sec]
#define BUCK_REGERR_RECOVERY_DELAY   (float) 250e-3 ///< regulation error recovery delay in [sec]
#define BUCK_OCP_TRIP_DELAY          (float) 2e-3   ///< over current proection trip delay in [sec]
#define BUCK_OCP_RECOVERY_DELAY      (float) 250e-3 ///< over current proection recovery delay in [sec]

// ~ conversion macros ~~~~~~~~~~~~~~~~~~~~~

#define BUCK_UVLO_TDLY   (uint16_t)(((float)      BUCK_UVLO_TRIP_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_UVLO_RDLY   (uint16_t)(((float)  BUCK_UVLO_RECOVERY_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_OVLO_TDLY   (uint16_t)(((float)      BUCK_OVLO_TRIP_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_OVLO_RDLY   (uint16_t)(((float)  BUCK_OVLO_RECOVERY_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_REGERR_TDLY (uint16_t)(((float)    BUCK_REGERR_TRIP_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_REGERR_RDLY (uint16_t)(((float)BUCK_REGERR_RECOVERY_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_OCP_TDLY    (uint16_t)(((float)       BUCK_OCP_TRIP_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)
#define BUCK_OCP_RDLY    (uint16_t)(((float)   BUCK_OCP_RECOVERY_DELAY / (float)MAIN_EXECUTION_PERIOD)-1.0)

// ~ conversion macros end ~~~~~~~~~~~~~~~~~
    
/*!Controller Declarations
 * *************************************************************************************************
 * Summary:
 * 
 * 
 * Description:
 * 
 * 
 * *************************************************************************************************/

    
// Hardware-dependent defines
#define BUCK_VOUT_TRIG_PWM  0   ///< Buck VOUT control loop is called in PWM interrupt
#define BUCK_VOUT_TRIG_ADC  1   ///< Buck VOUT control loop is called in ADC interrupt

#define BUCK_VOUT_TRIGGER_MODE  BUCK_VOUT_TRIG_PWM
#define BUCK_VOUT_ISR_PRIORITY  5     ///< Voltage loop interrupt vector priority (valid settings between 0...6 with 6 being the highest priority)
    
#if (BUCK_VOUT_TRIGGER_MODE == BUCK_VOUT_TRIG_ADC)    
  #define _BUCK_VLOOP_Interrupt     _ADCAN13Interrupt   
  #define _BUCK_VLOOP_ISR_IP        _ADCAN13IP
  #define _BUCK_VLOOP_ISR_IF        _ADCAN13IF
  #define _BUCK_VLOOP_ISR_IE        _ADCAN13IE
#elif (BUCK_VOUT_TRIGGER_MODE == BUCK_VOUT_TRIG_PWM)
  #define _BUCK_VLOOP_Interrupt     _PWM1Interrupt   
  #define _BUCK_VLOOP_ISR_IP        _PWM1IP
  #define _BUCK_VLOOP_ISR_IF        _PWM1IF
  #define _BUCK_VLOOP_ISR_IE        _PWM1IE
#endif


#endif	/* DPSK3_HARDWARE_DESCRIPTOR_H */

