// ADC.c
// Runs on LM4F120/TM4C123
// Provide functions that initialize ADC0
// Amanda Vuong and Twinkle Khanna
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ADC.h"


// ADC initialization function 
// Input: none
// Output: none
// measures from PD2, analog channel 5

void ADC_Init(void){
				volatile unsigned long delay;
				SYSCTL_RCGCGPIO_R |= 0x08;	// 1) activate clock for Port D
				delay = SYSCTL_RCGCGPIO_R;  //    allow time for clock to stabilize
        delay = SYSCTL_RCGCGPIO_R;
        delay = SYSCTL_RCGCGPIO_R;
        delay = SYSCTL_RCGCGPIO_R;
        delay = SYSCTL_RCGCGPIO_R;
		    delay = SYSCTL_RCGCGPIO_R;
        delay = SYSCTL_RCGCGPIO_R;
        delay = SYSCTL_RCGCGPIO_R;
        delay = SYSCTL_RCGCGPIO_R; 
				GPIO_PORTD_DIR_R &= ~0x04;   // 2) make PD2 input
				GPIO_PORTD_AFSEL_R |= 0x04;  // 3) enable alternate function on PD2
				GPIO_PORTD_DEN_R &= ~0x04;   // 4) disable digital I/O on PD2
				GPIO_PORTD_AMSEL_R |= 0x04;  // 5) enable analog function on PD2
				SYSCTL_RCGCADC_R |= 0x01;    // 6) activate ADC0
				delay = SYSCTL_RCGCADC_R;
        delay = SYSCTL_RCGCADC_R;
        delay = SYSCTL_RCGCADC_R;
        delay = SYSCTL_RCGCADC_R;		
				delay = SYSCTL_RCGCADC_R;
				delay = SYSCTL_RCGCADC_R;
        delay = SYSCTL_RCGCADC_R;
        delay = SYSCTL_RCGCADC_R;		
				delay = SYSCTL_RCGCADC_R;	
				ADC0_PC_R = 0x03;                       // 7) configure for 250K
				ADC0_SSPRI_R = 0x0123;                  // 8) Sequencer 3 is highest priority
				ADC0_ACTSS_R &= ~0x0008;                // 9) disable sample sequencer 3
				ADC0_EMUX_R &= ~0xF000;                 // 10) seq3 is software trigger
				ADC0_SSMUX3_R=(ADC0_SSMUX3_R & ~0xF)+5; // 11) channel Ain5 (PD2)
				ADC0_SSCTL3_R = 0x0006;                 // 12) no TS0 D0, yes IE0 END0
				ADC0_IM_R &= ~0x0008;										// 13) no interrupts
				ADC0_ACTSS_R |= 0x0008;         				// 14) turn sequencer on
				ADC0_SAC_R = 0x06; 
} 

//------------ADC_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
// measures from PD2, analog channel 5

uint32_t ADC_In(void){
				unsigned long result;
        ADC0_PSSI_R = 0x008;               //start sequencer 3
        while ((ADC0_RIS_R & 0x08)==0) {}; //busy-wait
        result = ADC0_SSFIFO3_R & 0xFFF;   //read data from FIFO buffer
        ADC0_ISC_R = 0x0008;               //clear flag in RIS
        return result;
}


