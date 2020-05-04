#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Switches1.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int pressed1 = 0;

void Button1_Init(void){    
	DisableInterrupts();
  SYSCTL_RCGCGPIO_R |= 0x00000001; 	// activate clock for port A
	__nop();
	__nop();
  GPIO_PORTA_DIR_R &= ~0x10;    		// make PA4
  GPIO_PORTA_AFSEL_R &= ~0x10;  		// disable alt funct on PA4
  GPIO_PORTA_DEN_R |= 0x10;     		// enable digital I/O on PA4 
  GPIO_PORTA_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTA_AMSEL_R = 0;       		// disable analog functionality on PA4
	GPIO_PORTA_PUR_R |= 0x10;     		// enable weak pull-up on PA4
  GPIO_PORTA_IS_R &= ~0x10;     		// PA4 is edge-sensitive
  GPIO_PORTA_IBE_R &= ~0x10;    		// PA4 is not both edges
  GPIO_PORTA_IEV_R &= ~0x10;    		// PA4 falling edge event
  GPIO_PORTA_ICR_R = 0x10;      		// clear flag for PA4
  GPIO_PORTA_IM_R |= 0x10;      		// arm interrupt on PA4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00010000; // priority 1
  NVIC_EN0_R = 0x00000001;      		// enable interrupt 0 in NVIC
  EnableInterrupts();           		// Clears the I bit
}

void GPIOPortA_Handler(void){
	GPIO_PORTA_ICR_R = 0x10;     			// acknowledge flag for PA4
  pressed1 ^= 1;
}

/* Button1Press
input: none
output: state of button1
Return state of button attached to PA4
*/
int Button1Press(void){
	return pressed1;
}

/* ResetButton1
input: Desired state of Button1
output: none
Set state of Button1
*/
void ResetButton1(int state){
	pressed1 = state;
}
