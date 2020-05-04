//Menu.c


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "Switches.h"
#include "ST7735.h"
#include "Menu.h"
#include "SysTick.h"

int startFlag = 0;
int EnglishorSpanish = 0;


//Draws menu screen
void DrawMenu(void){
	ST7735_FillScreen(0x0000);   // set screen to black
  ST7735_SetCursor(3, 1);
  ST7735_OutString("Space Invaders");
	ST7735_SetCursor(1, 9); //14
  ST7735_OutString("Press A for English");
  ST7735_SetCursor(1, 11); //16
  ST7735_OutString("Presione B para ");
	ST7735_SetCursor(1, 12); //17
	ST7735_OutString("Espanol");
}

int GetStatus(void){
	return startFlag;
}

int GetLanguage(void){
	return EnglishorSpanish; //english=0; spanish=1;
}

void Menu(void){

	while ((Button0Press() || Button1Press())== 0){}
	
	if (Button0Press()){
		ST7735_FillScreen(0x0000);   // set screen to black
		ST7735_SetCursor(3, 5);
		ST7735_OutString("Space Invaders");
		ResetButton1(0);
		EnglishorSpanish = 0;
		SysTick_Wait3s(30);
		startFlag = 1;
	}


	if (Button1Press()){
		ST7735_FillScreen(0x0000);   // set screen to black
		ST7735_SetCursor(5, 3);
		ST7735_OutString("Invasores");
		ST7735_SetCursor(5, 4);
		ST7735_OutString("Espaciales");
		ResetButton1(0);
		EnglishorSpanish = 1;
		SysTick_Wait3s(30);
		startFlag = 1;
	}
}
