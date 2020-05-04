#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Switches.h"
#include "GameEngine.h"



void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
long StartCritical (void);    // previous I bit, disable interrupts
void EndCritical(long sr);    // restore I bit to previous value
void WaitForInterrupt(void);  // low power mode

int pressed0 = 0;
//int pressed1 = 0;

//----------------Initialization----------------------------

void Button0_Init(void){    //Initialization for PF4
	DisableInterrupts();
  SYSCTL_RCGCGPIO_R |= 0x00000020; 	// activate clock for port F
	__nop();
	__nop();
  GPIO_PORTF_DIR_R &= ~0x10;    		// make PF4 in (built-in button)
  GPIO_PORTF_AFSEL_R &= ~0x10;  		// disable alt funct on PF4
  GPIO_PORTF_DEN_R |= 0x10;     		// enable digital I/O on PF4 
  GPIO_PORTF_PCTL_R &= ~0x000F0000; // configure PF4 as GPIO
  GPIO_PORTF_AMSEL_R &= ~0x10;       		// disable analog functionality on PF4
  GPIO_PORTF_PUR_R |= 0x10;     		// pull-up on PF4
  GPIO_PORTF_IS_R &= ~0x10;     		// PF4 is edge-sensitive
  GPIO_PORTF_IBE_R &= ~0x10;    		// PF4 is not both edges
  GPIO_PORTF_IEV_R &= ~0x10;    		// PF4 falling edge event
  GPIO_PORTF_ICR_R = 0x10;      		// clear flag for PF4
  GPIO_PORTF_IM_R |= 0x10;      		// arm interrupt on PF4
  NVIC_PRI7_R = (NVIC_PRI7_R&0xFF00FFFF)|0x00A00000; // priority 5
  NVIC_EN0_R = 0x40000000;      		// enable interrupt 30 in NVIC
  EnableInterrupts();           		// Clears the I bit
}
/*
void Button1_Init(void){    
		DisableInterrupts();
  SYSCTL_RCGCGPIO_R |= 0x00000001; 	// activate clock for port A
	__nop();
	__nop();
  GPIO_PORTA_DIR_R &= ~0x10;    		// make PF4 in (built-in button)
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
*/
//---------------PF4 BUTTON--------------------------
//When PF4 is hit, laser is fired

void GPIOPortF_Handler(void){
  GPIO_PORTF_ICR_R = 0x10;     			// acknowledge flag for PF4
	pressed0 ^= 1;
	PlayerFire();
}

int Button0Press(void){
	return pressed0;
}

void ResetButton0(int state){
	pressed0 = state;
}

/*
//---------------PA4 BUTTON----------------------------
//When PA4 is hit, the game is paused

void GPIOPortA_Handler(void){
	GPIO_PORTA_ICR_R = 0x10;     			// acknowledge flag for PA4
  pressed1 ^= 1;
}

int Button1Press(void){
	return pressed1;
}


void ResetButton1(int state){
	pressed1 = state;
}
*/

