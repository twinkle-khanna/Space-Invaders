// SysTick.c

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "SysTick.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts

void SysTick_Init(void){
	DisableInterrupts();
	NVIC_ST_CTRL_R = 0;					// disable SysTick for init
	NVIC_ST_RELOAD_R = 1333334;	// 60 Hz on 80MHz clock
	NVIC_ST_CURRENT_R = 0;			// clear CURRENT
	NVIC_ST_CTRL_R = 0x07;
	EnableInterrupts();
}

void SysTick_Handler(void){
}

void SysTick_Wait(unsigned long delay){
	NVIC_ST_RELOAD_R = delay-1;
	NVIC_ST_CURRENT_R = 0;
	while ((NVIC_ST_CTRL_R&0x00010000)==0){}
}

void SysTick_Wait3s(unsigned long delay){
	unsigned long i;
	for(i=0; i<delay; i++){
		SysTick_Wait(240000000);
	}
}
