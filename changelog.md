# dpsk3-power-buck-adaptive-gain-control, release v1.0.0

### Release Highlights
This is the initial release of the adaptive gain control code example demonstrating the enhancement of a basic 
digital voltage mode control implementation regulating the output voltage of a synchronous buck converter. 
Adaptive gain control is a special version of a feed forward implementation modulating the feedback loops gain
at runtime based on the voltage across the main inductor. This implementation responds to changes of the input
voltage as well as the output voltage, effectively stabilizing the open loop gain of the closed loop system 
across all operating conditions.

This example is based on the basic voltage mode code examples for the synchronous buck converter on the 
Digital Power Starter Kit 3.

### Features Added\Updated
This code example runs automatically after power has been applied to the development board. In this initial version 
the on-board LED of the dsPIC33CK Digital Power Plug-In Module is toggled with a interval period of 300 ms, when the 
controller is running at maximum speed of 100 MIPS. After startup, PWM generator #1 is generating a 500 kHz complementary 
waveform at the PWM1H/PWM1L outputs. The output voltage feedback is read through ADC input #13 and processed through the 
digital control loop in the respective interrupt service routine. The control loop also calls and extension algorithm, 
which reads the most recent input voltage through ADC input #12 and calculates the most recent voltage across the main 
inductor (VL). The ratio of the instantaneous VL versus the nominal VL at nominal operating conditions of VIN = 9V and 
VOUT = 3.3V is converted into a modulation factor k(AGC), which gets multiplied with the error delay line of the compensation
filter. At the end of the control loop execution the result is written to the PWM duty cycle register, eventually regulating 
the output voltage to the nominal 3.3 V output voltage.

During operation the most recent values of input voltage, output voltage, output current and temperature are displayed on 
the on-board LCD. The screens can be switched by pressing the User button for more than 1 second.

### Supported Hardware Revisions:
Digital Power Starter Kit 3, Revision 3.0 (Part-No. 330017-3)
Visit [www.microchip.com/330017-3](https://www.microchip.com/330017-3) for more information
