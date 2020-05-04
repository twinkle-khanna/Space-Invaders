/*
// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Jonathan Valvano and Daniel Valvano
// This is a starter project for the EE319K Lab 10

// Last Modified: 1/17/2020 
// http://www.spaceinvaders.de/

// sounds at http://www.classicgaming.cc/classics/spaceinvaders/sounds.php
// http://www.classicgaming.cc/classics/spaceinvaders/playguide.php
	This example accompanies the books
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2019

   "Embedded Systems: Introduction to Arm Cortex M Microcontrollers",
   ISBN: 978-1469998749, Jonathan Valvano, copyright (c) 2019

 Copyright 2019 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
// ******* Possible Hardware I/O connections*******************
// Slide pot pin 1 connected to ground
// Slide pot pin 2 connected to PD2/AIN5
// Slide pot pin 3 connected to +3.3V 
// fire button connected to PE0
// special weapon fire button connected to PE1
// 8*R resistor DAC bit 0 on PB0 (least significant bit)
// 4*R resistor DAC bit 1 on PB1
// 2*R resistor DAC bit 2 on PB2
// 1*R resistor DAC bit 3 on PB3 (most significant bit)
// LED on PB4
// LED on PB5

// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ST7735.h"
#include "Print.h"
#include "Random.h"
#include "PLL.h"
#include "ADC.h"
#include "Sound.h"
#include "Timer0.h"
#include "Timer1.h"
#include "Menu.h"
#include "Switches.h"
#include "Switches1.h"
#include "GameEngine.h"
#include "Systick.h"
#include "Dac.h"

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void Delay100ms(uint32_t count); // time delay in 0.1 seconds

/*int main (void){
	DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
  Output_Init();
	ST7735_FillScreen(0x0000);            // set screen to black
	Button0_Init();
	Button1_Init();
	EnableInterrupts();
	DrawMenu();
	Sound_Init();
	while(1){
		if (Button0Press()){
			Sound_Shoot();  // play shoot sound
			//ST7735_FillScreen(0x0000); 
			//ST7735_SetCursor(1, 3);
			//ST7735_OutString("GAME OVER");
		}
	}
	
} */


//REAL MAIN ENGINE
int main (void){
		DisableInterrupts();
		PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
		Random_Init(1);
		ADC_Init();	
		Sound_Init();
		DAC_Init();
		Output_Init();
		ST7735_FillScreen(0x0000);            // set screen to black
		Button0_Init();
		Button1_Init();
	  SysTick_Init();	
		EnableInterrupts();	
    DrawMenu();

    int state = 0;
        while(state==0){
                Menu();
                state = GetStatus();
        }
     

        while(1){
                if(state == 1){                               // state = 1, play space invaders
                        ST7735_FillScreen(0x0000);            // set screen to black
                        SpaceInvader_Init();                  // intialize space invaders
                        ResetButton0(0);
                        ResetButton1(0);
                        Timer1_Init(EnemyMovement, 8000000);   // enemy movement at around 10 Hz
                        while(state){                          // state = 1 infinite loop
                                SpaceInvader();                // play space invaders
                                state = GetGameStatus();       // get status of game
                        }

                        DrawVictoryScreen();                    // draw victory screen
                        while(1){                               // infinite loop
                        }
                }
  }

}


/*
int main (void){
	DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);
  Output_Init();
	ST7735_FillScreen(0x0000);            // set screen to black
	Button0_Init();
	Button1_Init();
	EnableInterrupts();
	DrawMenu();
	int state = 0;
	while(state==0){
		Menu();
		state = GetStatus();
	}
	
}
*/

/*
//debugging the bunkers
int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  while(1){
  ST7735_DrawBitmap(5, 12, Bunker00, 18,5); // most damaged
	ST7735_DrawBitmap(53, 12, Bunker11, 18,5); // medium damaged
	ST7735_DrawBitmap(101, 12, Bunker22, 18,5); // intact
	}
}
*/

/*int main(void){
  DisableInterrupts();
  PLL_Init(Bus80MHz);       // Bus clock is 80 MHz 
  Random_Init(1);

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  
  ST7735_DrawBitmap(22, 130, PlayerShip0, 18,8); // player ship bottom
  ST7735_DrawBitmap(53, 130, Bunker0, 18,5);
  ST7735_DrawBitmap(42, 130, PlayerShip1, 18,8); // player ship bottom
  ST7735_DrawBitmap(62, 130, PlayerShip2, 18,8); // player ship bottom
  ST7735_DrawBitmap(82, 130, PlayerShip3, 18,8); // player ship bottom

  ST7735_DrawBitmap(3, 12, SmallEnemy10pointA, 16,10);
  ST7735_DrawBitmap(23, 12, SmallEnemy10pointB, 16,10);
  ST7735_DrawBitmap(43, 12, SmallEnemy20pointA, 16,10);
  ST7735_DrawBitmap(63, 12, SmallEnemy20pointB, 16,10);
  ST7735_DrawBitmap(83, 12, SmallEnemy30pointA, 16,10);
  ST7735_DrawBitmap(103, 12, SmallEnemy30pointB, 16,10);

  Delay100ms(50);              // delay 5 sec at 80 MHz

  ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(1, 1);
  ST7735_OutString("GAME OVER");
  ST7735_SetCursor(1, 2);
  ST7735_OutString("Nice try,");
  ST7735_SetCursor(1, 3);
  ST7735_OutString("Earthling!");
  ST7735_SetCursor(2, 4);
  LCD_OutDec(1234);
  while(1){
  }

}
*/


// You can't use this timer, it is here for starter code only 
// you must use interrupts to perform delays
void Delay100ms(uint32_t count){uint32_t volatile time;
  while(count>0){
    time = 727240;  // 0.1sec at 80 MHz
    while(time){
	  	time--;
    }
    count--;
  }
}

