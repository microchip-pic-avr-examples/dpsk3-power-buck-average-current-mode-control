/* *********************************************************************************
 * PowerSmart(TM) Digital Control Library Designer, Version 0.9.12.660
 * *********************************************************************************
 * 4p4z controller function declarations and compensation filter coefficients
 * derived for following operating conditions:
 * *********************************************************************************
 *
 *  Controller Type:    4P4Z - Advanced High-Q Compensator
 *  Sampling Frequency: 500000 Hz
 *  Fixed Point Format: Q15
 *  Scaling Mode:       4 - Fast Floating Point Coefficient Scaling
 *  Input Gain:         0.5
 *
 * *********************************************************************************
 * CGS Version:         3.0.4
 * CGS Date:            01/27/2021
 * *********************************************************************************
 * User:                M91406
 * Date/Time:           01/29/2021 01:17:25
 * ********************************************************************************/

// This is a guard condition so that contents of this file are not included
// more than once.
#ifndef __SPECIAL_FUNCTION_LAYER_V_LOOP_H__
#define __SPECIAL_FUNCTION_LAYER_V_LOOP_H__

#include <xc.h>                                   // include processor files - each processor file is guarded
#include <dsp.h>                                  // include DSP data types (e.g. fractional)
#include <stdint.h>                               // include standard integer number data types
#include <stdbool.h>                              // include standard boolean data types (true/false)

#include "npnz16b.h"                              // include NPNZ library header file

/* *******************************************************************************
 * Data Arrays:
 * The NPNZ16b_t data structure contains pointers to coefficient, control and error
 * history arrays. The pointer target objects (variables and arrays) are defined
 * in controller source file v_loop.c
 *
 * Type definitions for A- and B- coefficient arrays as well as error- and control
 * history arrays are aligned in memory using the 'packed' attribute for optimized
 * addressing during DSP computations. These aligned data structures need to be
 * placed in specific memory locations to allow direct X/Y-access from the DSP.
 * This X/Y-memory placement is covered by the declarations used in controller
 * source file v_loop.c
 * ******************************************************************************/

typedef struct V_LOOP_CONTROL_LOOP_COEFFICIENTS_s
{
    volatile int32_t ACoefficients[4];            // A-Coefficients
    volatile int32_t BCoefficients[5];            // B-Coefficients
} __attribute__((packed)) V_LOOP_CONTROL_LOOP_COEFFICIENTS_t; // Data structure packing A- and B- coefficient arrays in a linear memory space for optimized DSP code execution

typedef struct V_LOOP_CONTROL_LOOP_HISTORIES_s
{
    volatile fractional ControlHistory[4];        // Control History Array
    volatile fractional ErrorHistory[5];          // Error History Array
} __attribute__((packed)) V_LOOP_CONTROL_LOOP_HISTORIES_t; // Data structure packing control and error histories arrays in a linear memory space for optimized DSP code execution

// P-Term Coefficient for Plant Measurements
extern volatile int16_t v_loop_pterm_factor;      // Q15 fractional of the Pterm factor
extern volatile int16_t v_loop_pterm_scaler;      // Bit-shift scaler of the Pterm factor


/*********************************************************************************
 * \ingroup special-function-layer-npnz16-objects
 * \var   v_loop
 * \brief External reference to user-defined NPNZ16b_s controller data object 'v_loop'
 ********************************************************************************/
extern volatile struct NPNZ16b_s v_loop;          // user-controller data object


/* *******************************************************************************
 * Function call prototypes for initialization routines and control loop handling
 * ******************************************************************************/

/*********************************************************************************
 * \fn volatile uint16_t v_loop_Initialize(volatile struct NPNZ16b_s* controller)
 * \ingroup special-function-layer-npnz16-functions
 * \brief Initializes controller coefficient arrays and normalization
 * \param controller: Pointer to NPNZ Controller Data Object of type struct NPNZ16b_s
 ********************************************************************************/
extern volatile uint16_t __attribute__((near)) v_loop_Initialize( // v_loop initialization function call
        volatile struct NPNZ16b_s* controller     // Pointer to NPNZ16b data object
    );

/*********************************************************************************
 * \fn void __attribute__((near)) v_loop_Reset(volatile struct NPNZ16b_s* controller)
 * \ingroup special-function-layer-npnz16-functions
 * \brief Clears the 4P4Z controller output and error histories
 * \param controller: Pointer to NPNZ16b data object of type struct NPNZ16b_s*
 ********************************************************************************/
extern void __attribute__((near)) v_loop_Reset(   // v_loop reset function call (Assembly)
        volatile struct NPNZ16b_s* controller     // Pointer to NPNZ16b data object
    );

/*********************************************************************************
 * \fn void __attribute__((near)) v_loop_Precharge(volatile struct NPNZ16b_s* controller, volatile fractional ctrl_input, volatile fractional ctrl_output)
 * \ingroup special-function-layer-npnz16-functions
 * \brief Loads user-defined values into 4P4Z controller output and error histories
 * \param controller: Pointer to NPNZ16b data object of type struct NPNZ16b_s*
 * \param ctrl_input: user-defined, constant error history value of type fractional
 * \param ctrl_output: user-defined, constant control output history value of type fractional
 ********************************************************************************/
extern void __attribute__((near)) v_loop_Precharge( // v_loop history pre-charge function call (Assembly)
        volatile struct NPNZ16b_s* controller,    // Pointer to NPNZ16b data object
        volatile fractional ctrl_input,           // user-defined, constant error history value
        volatile fractional ctrl_output           // user-defined, constant control output history value
    );

/*********************************************************************************
 * \fn void __attribute__((near)) v_loop_Update(volatile struct NPNZ16b_s* controller)
 * \ingroup special-function-layer-npnz16-functions
 * \brief Calls the v_loop control loop
 * \param controller: Pointer to NPNZ16b data object of type struct NPNZ16b_s*
 ********************************************************************************/
extern void __attribute__((near)) v_loop_Update(  // Calls the 4P4Z controller (Assembly)
        volatile struct NPNZ16b_s* controller     // Pointer to NPNZ16b data object
    );

/*********************************************************************************
 * \fn void __attribute__((near)) v_loop_PTermUpdate(volatile struct NPNZ16b_s* controller)
 * \ingroup special-function-layer-npnz16-functions
 * \brief Calls the v_loop P-Term controller during measurements of plant transfer functions
 * \param controller: Pointer to NPNZ16b data object of type struct NPNZ16b_s*
 * \attention
 * THIS CONTROLLER IS USED FOR MEASUREMENTS OF THE PLANT TRANSFER FUNCTION ONLY.
 * THIS LOOP IS BY DEFAULT UNSTABLE AND ONLY WORKS UNDER STABLE TEST CONDITIONS
 * DO NOT USE THIS CONTROLLER TYPE FOR NORMAL OPERATION
 ********************************************************************************/
extern void __attribute__((near)) v_loop_PTermUpdate( // Calls the P-Term controller (Assembly)
        volatile struct NPNZ16b_s* controller     // Pointer to NPNZ16b data object
    );

#endif                                            // end of __SPECIAL_FUNCTION_LAYER_V_LOOP_H__


//**********************************************************************************
// Download latest version of this tool here: https://areiter128.github.io/DCLD
//**********************************************************************************

