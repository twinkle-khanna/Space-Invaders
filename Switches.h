//Header file for Switches.c

void Button0_Init(void);
void Button1_Init(void);

void GPIOPortF_Handler(void);
int Button0Press(void);
void ResetButton0(int state);

void GPIOPortA_Handler(void);
int Button1Press(void);
void ResetButton1(int state);

