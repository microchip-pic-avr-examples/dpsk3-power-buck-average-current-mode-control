/*
 * File:   app_lcd.c
 * Author: M91406
 *
 * Created on March 12, 2020, 12:10 PM
 */

#include "lcd/app_lcd.h"
#include <math.h>

// Additional header files required by this app
#include "config/apps.h"
#include "config/hal.h"

// PRIVATE VARIABLE DELARATIONS
volatile uint16_t lcd_cnt = 0;  // local counter determining LCD refresh rate
#define LCD_STARTUP   30000
#define LCD_REFRESH   2000

volatile LCD_t lcd;

// Special section for average current calculation

volatile uint16_t appLCD_Initialize(void) 
{
    volatile uint16_t retval = 1;
    
    if (lcd.refresh == 0)
        lcd.refresh = LCD_STARTUP;
    
    Dev_Lcd_Init();
    Dev_Lcd_WriteStringXY(0,0,"==== DPSK-3 ====");
    Dev_Lcd_WriteStringXY(0,1," CE200 BUCK VMC ");

    lcd_cnt = 0;
    lcd.enabled = true;
    
    return(retval);
}

volatile uint16_t appLCD_Execute(void) 
{
    volatile uint16_t retval = 1;
    volatile float vi=0.0, vo=0.0, isns=0.0, temp=0.0;
    
    // IF LCD output is disabled, exit here
    if(!lcd.enabled)
        return(retval);
    
    // Refresh LCD and reset refresh counter
    lcd_cnt++;
    
    // If REFRESH period has expired, update LCD contents
    if(lcd_cnt == lcd.refresh) {
        
        // Calculate output values
        temp = ((float)(buck.data.temp - BUCK_FB_TEMP_ZERO) / BUCK_FB_TEMP_SLOPE); // Scale ADC value to physical unit
        temp = (float)(int)(100.0 * temp);  // Rounding operation required to prevent display 
        temp /= 100.0;                      // rounding issues around 9.99 and 10.0 V
        vi = ((buck.data.v_in << 3) * ADC_GRAN); // Scale ADC value to physical unit
        vi = (float)(int)(100.0 * vi);      // Rounding operation required to prevent display
        vi /= 100.0;                        // rounding issues around 9.99 and 10.0 ° C
        vo = ((buck.data.v_out << 1) * ADC_GRAN); // Scale ADC value to physical unit
        isns = (((buck.data.i_out - BUCK_ISNS_FEEDBACK_OFFSET) * ADC_GRAN) /  BUCK_ISNS_FEEDBACK_GAIN);; // Scale ADC value to physical unit
        
        // Input voltage display
        if((double)vi < 10.000)
            PrintLcd(0, "VIN     = %2.2f V", (double)vi);
        else
            PrintLcd(0, "VIN     = %2.1f V", (double)vi);

        switch (lcd.screen)
        {
            case 1:     // Show Temperature Output
                if((double)temp < 10.000)
                    PrintLcd(1, "TEMP    = %2.2f C", (double)temp);
                else
                    PrintLcd(1, "TEMP    = %2.1f C", (double)temp);
                break;
            
            case 2:     // Show Current Output
                if((double)isns < 1.000)
                {
                    isns *= 1000.0;
                    PrintLcd(1, "ISNS    = %3d mA", (int)isns);
                }
                else
                    PrintLcd(1, "ISNS    = %1.2f A", (double)isns);
                break;

            default:    // Output voltage display
                
                if (!buck.status.bits.fault_active)
                    PrintLcd(1, "VOUT    = %2.2f V", (double)vo);
                else {
                    if (fltobj_BuckUVLO.Status.bits.FaultStatus)
                        PrintLcd(1, "VOUT(UV)= %2.2f V", (double)vo);
                    else if (fltobj_BuckOVLO.Status.bits.FaultStatus)
                        PrintLcd(1, "VOUT(OV)= %2.2f V", (double)vo);
                    else if (fltobj_BuckRegErr.Status.bits.FaultStatus)
                        PrintLcd(1, "VOUT(RE)= %2.2f V", (double)vo);
                }
                break;
        }
        
        lcd.refresh = LCD_REFRESH;
        
        lcd_cnt = 0; // Reset internal interval counter
    }
    
    return(retval);
}

volatile uint16_t appLCD_Dispose(void) 
{
    volatile uint16_t retval = 1;
    
    /* PLACE DISPOSE CODE HERE */

    return(retval);
}
