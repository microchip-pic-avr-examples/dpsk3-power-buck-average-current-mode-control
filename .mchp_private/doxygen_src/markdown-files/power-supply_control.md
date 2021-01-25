# Power Supply Control


<span id="startDoc"> </span> <!-- start tag for internal references -->
<div style="text-align:left">
    <img src="images/startbanner.png" alt="Firmware Quick-Start Guide" height="70">
</div>


### DPSK3 Buck Converter voltage Mode Control Loop Implementation

This firmware demonstrates the implementation of a simple, single loop voltage mode controller used to regulate the constant output voltage of the on-board  step-down converter of the Digital Power Starter Kit 3 (DPSK3). The implementation of the Voltage Feedback Loop requires one Analog-to-Digital Converter (ADC) oversampling the output voltage of the converter and two PWM outputs (PWM high and PWM low) to drive the power converter half-bridge switch node in synchronous mode. 

The following image shows the block diagram of the buck converter voltage mode controller, where the ADC input is used to sample the most recent output voltage feedback signal. Once converted, the value is then compared against the internal reference and the inverse of the deviation is pushed through the discrete compensation filter. 

In the Anti-Windup block, the output of the compensation filter is checked against user-specified minimum thresholds. Should any of these thresholds be exceeded, the controller output will be overwritten with the respective threshold value before being written to the PWM duty cycle register of the PWM logic.

In this example the duty cycle of the PWM output signal will be updated immediately when a new value is written to the PWM logic to ensure a minimum response time of the feedback loop.

<div style="text-align:left">
    <img src="images/voltage-mode-control.png" alt="Firmware Quick-Start Guide" width="800">
</div>


bla bla bla


<div style="text-align:left">
    <img src="images/contol-flow-chart.png" alt="Firmware Quick-Start Guide" width="960">
</div>





_________________________________________________
(c) 2021, Microchip Technology Inc.
