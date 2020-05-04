//GameEngine.c

#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "ADC.h"
#include "Random.h"
#include "ST7735.h"
#include "Sound.h"
#include "LCD.h"
#include "Images.h"
#include "GameEngine.h"
#include "Switches1.h"
#include "Menu.h"

#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))

//------------------Player Initialization------------------------------------------
#define MAX_PLAYER_LASER = 1;                // maximum number of player lasers on screen
#define PLAYER_WIDTH = 12;                   // default width of player
#define PLAYER_HEIGHT = 8;                   // default width of player
#define PLAYER_X = 56;                       // default player x position
#define PLAYER_Y = 159;                      // default player y position

//------------------Enemy Initialization--------------------------------------------
#define MAX_ENEMY = 18;                       // maximum number of enemies on screen, multiple of 3 for 3 rows of enemies
#define MAX_ENEMY_LASER = 4;                  // maximum number of enemy lasers on screen
#define ENEMY_WIDTH = 16;                     // default enemy width
#define ENEMY_HEIGHT = 10;                    // default enemy height
#define ENEMY_Y_OFFSET = 9;                   // default enemy y-axis offset

//-----------------_Bunker Initialization-----------------------------------------------
#define MAX_BUNKER = 3;                       // maximum number of bunkers
#define BUNKER_WIDTH = 18;
#define BUNKER_HEIGHT = 5;
#define BUNKER_X = 5;                         // left most bunker position
#define BUNKER_Y = 151;                       // default bunker y position

//-----------------Laser Initialization----------------------------------------------------
#define LASER_HEIGHT = 8;                     // height of laser model

//-----------------ADC Initilization--------------------------------------------------------
#define LEFT_THRESH = 850;                    // ADC threshold to move left
#define RIGHT_THRESH = 1250;                  // ADC threshold to move right

//----------------Game initialization----------------------------------------------------------
uint32_t gameOver = 0;                        // gameOver flag
uint32_t gameSuccess = 0;                     // gameSuccess flag
uint32_t score = 0;                           // player score
uint32_t playerLaserCount = 0;                // number of player lasers on screen
uint32_t enemyLaserCount = 0;                 // number of enemy lasers on screen
uint32_t enemiesAlive = 0;                    // number of alive enemies
uint32_t enemiesKilled = 0;                   // number of dead enemies
uint32_t frame = 0;                           // current frame
uint32_t enemyFireDelay = 0;                  // delay that the enemy fires at
uint32_t horizontalCounter = 0;               // enemy horizontal movement counter
uint32_t verticalCounter = 0;                 // enemy vertical movement counter
uint32_t gameRunning = 0;                     // check if game is running
uint32_t stage = 0;                           // current stage of game

//---------------Player Sprite---------------------------------------------------------
struct PlayerSprite{
        uint32_t xPos;                        // x-axis position
        uint32_t yPos;                        // y-axis position
        uint32_t hp;                          // health of player
        int dead;                             // status of recent death
        const unsigned short *image;          // pointer to bitmaps
};
typedef struct PlayerSprite PlayerSprite_t;   // player sprite datatype

//---------------Enemy Sprite------------------------------------------------------------
struct EnemySprite{
        uint32_t xPos;                        // x-axis position
        uint32_t yPos;                        // y-axis position
        uint32_t hp;                          // health of enemy
        uint32_t points;                      // point value of enemy
        int dead;                             // status of recent death
        const unsigned short *image[2];       // pointer to bitmaps
};
typedef struct EnemySprite EnemySprite_t;     // enemy sprite datatype

//--------------Bunker Sprite-----------------------------------------------------------------
struct BunkerSprite{
        uint32_t xPos;                        // x-axis position
        uint32_t yPos;                        // y-axis position
        uint32_t hp;                          // health of bunker
        int dead;                             // status of player (alive/dead)
        const unsigned short *image[3]; 			// pointer to bitmaps
        /* bunker has 3 states: alive, damaged, severely damaged*/
};
typedef struct BunkerSprite BunkerSprite_t;   // bunker sprite datatype

//-------------Laser Sprite-------------------------------------------------------------------
struct LaserSprite{
        int xPos;                             // x-axis position
        int yPos;                             // y-axis position
        int dead;                             // status of laser (alive/dead)
        const unsigned short *image;          // pointer to bitmaps
        uint8_t speed;                        // vertical speed of laser
};
typedef struct LaserSprite PlayerLaser_t;     // player laser sprite datatype
typedef struct LaserSprite EnemyLaser_t;      // enemy laser sprite datatype

//------------All Sprites----------------------------------------------------------------------
PlayerSprite_t Player;                        // 1 player sprite
EnemySprite_t Enemies[18];                    // maximum # of enemies is 18 in 3 rows of 6
BunkerSprite_t Bunkers[3];                    // maximum # of bunkers is 3
PlayerLaser_t PlayerLasers[4];                // maximum # of player lasers on screen is 4
EnemyLaser_t EnemyLasers[18];                 

void Draw(void);        // function prototype for Draw
void GameOver(void);    // function prototype for GameOver
void Victory(void);     // function prototype for Victory

/* RandomNumber
input: int
output: int
returns random integer from 0 to (number - 1)
*/
uint32_t RandomNumber(uint32_t number){
  Random_Init(NVIC_ST_CURRENT_R);
  return ((Random32() >> 24) % number); /*0 - (number - 1)*/
}

/* UpdateScore
Updates score on screen.
*/
void UpdateScore(int currentScore){ 
        if (GetLanguage()){
					ST7735_SetCursor(4, 1);
					ST7735_OutString("Puntuacion:");
				} else if (GetLanguage() == 0){
					ST7735_SetCursor(7, 1);
					ST7735_OutString("Score:");
				}
        LCD_OutDec(currentScore);
}

/* SpaceInvader_Init
input: none
output: none
Initializes space invader portion of game
*/
void SpaceInvader_Init(void){
        uint8_t k = 0;                                  // index

        gameRunning = 1;                                // game is running
        score = 0;                                      // score reset
        frame = 0;                                      // current frame state
        playerLaserCount = 0;                           // player lasers reset
        enemyLaserCount = 0;                            // enemy lasers reset

        enemiesAlive = 18;                              // enemy life reset
        enemiesKilled = 0;                              
       


        Player.xPos = 56;                               // player ship starts at middle bottom
        Player.yPos = 160; 
        Player.hp = 1;                                  // player starts with 1 health
        Player.dead = 0;                                // player hasn't recently died
        Player.image = PlayerShip;                      // player starts with default ship

        for(; k < 18; k++){                                          // initializes aliens
                Enemies[k].hp = 1;                                     // enemy starting health
                Enemies[k].dead = 0;                                   // enemies hasn't recently died
                if(k < 6){                                             // first row of 6 enemies in array are 10 pt.
                        Enemies[k].xPos = (16 * k) + 4;                // start at 4, and next 2 are 16 apart
                        Enemies[k].yPos = 10 + 9 + 10;                 // 19
                        Enemies[k].image[0] = SmallEnemy30pointA;      // arms down
                        Enemies[k].image[1] = SmallEnemy30pointB;      // arms up
                        Enemies[k].points = 30;                        // enemy is worth 30 points
                }
                else if(k < 12){                                       // second row of 6 enemies in array are 20 pts.
                        Enemies[k].xPos = (16 * (k - 6)) + 4;          
                        Enemies[k].yPos = 20 + 9 + 10;                 // 29
                        Enemies[k].image[0] = SmallEnemy20pointA;      // arms down
                        Enemies[k].image[1] = SmallEnemy20pointB;      // arms up
                        Enemies[k].points = 20;                        // enemy is worth 20 points
                }
                else{                                                                                                                                                           // third row of 6 enemies in array are 30 pts.
                        Enemies[k].xPos = (16 * (k - 12) + 4);
                        Enemies[k].yPos = 30 + 9 + 10;                 // 39
                        Enemies[k].image[0] = SmallEnemy10pointA;      // arms down
                        Enemies[k].image[1] = SmallEnemy10pointB;      // arms up
                        Enemies[k].points = 10;                        // enemy is worth 10 points
                }
        }

        k = 0;
        for(; k < 3; k++){                                              // initializes bunkers
                Bunkers[k].xPos = 5 + (k * 48);                         // assigns default bunker x positions
                Bunkers[k].yPos = 130;       //CHANGE THIS                            // default bunker position
                Bunkers[k].hp = 3;                                      // assigns bunker health
                Bunkers[k].dead = 0;                                    // assigns bunker life state
                Bunkers[k].image[0] = Bunker0;                          // assigns bunker state images
                Bunkers[k].image[1] = Bunker1;
                Bunkers[k].image[2] = Bunker2;
        }

        k = 0;
        for(; k < 1; k++){                                              // initializes player lasers
                PlayerLasers[k].dead = 1;                               // no player lasers at beginning
        }

        k = 0;
        for(; k < 18; k++){                                              // initializes enemy lasers
                EnemyLasers[k].dead = 1;                                // no enemy lasers at beginning
        }
        UpdateScore(score);                                             // update score
        Draw();                                                         // draw everything that has been initialized so far
}

void PlayerXMove(void){
        ST7735_DrawBitmap(Player.xPos, Player.yPos, BlackPlayer, 12, 8); // draw black placeholder in place of ship to avoid double images
        uint32_t analogIn;                                               // holds ADC data
        analogIn = ADC_In();                                             // reads in ADC data
       
				if((analogIn < 1850) && (Player.xPos > 5)){                                     
                Player.xPos -= 1;                                        // move left if the player can
        }
        else if((analogIn < 1850) && (Player.xPos <= 5)){               
                Player.xPos = 0;                                         // minimum left position
        }
        else if((analogIn > 2250) && (Player.xPos < 117)){     
                Player.xPos += 1;                                        // move right if the player can
        } 
        else if((analogIn > 2250) && (Player.xPos >= 117)){     
                Player.xPos = 117;                                       // maximum right position
        }
}

/* EnemyMoveRight
input: none
output: none
Moves enemy aliens right along X-axis
*/
void EnemyMoveRight(void){
        int8_t k = 0;
        for(; k < 18; k++){
                Enemies[k].xPos += 1;              // move every enemy right 1 pixel
        }
}

/* EnemyMoveLeft
input: none
output: none
Moves enemy aliens left along X-axis
*/
void EnemyMoveLeft(void){
        int8_t k = 0;
        for(; k < 18; k++){
                Enemies[k].xPos -= 1;              // move every enemy left 1 pixel
        }
}

/* EnemyMoveDown
input: none
output: none
Moves enemy down along Y-axis
*/
void EnemyMoveDown(void){
        int8_t k = 0;
        for(; k < 18; k++){
                Enemies[k].yPos += 1;                               // move every enemy down 1 pixel
                if((Enemies[k].yPos + 10)==(Bunkers[0].yPos)){
                        gameOver = 1;                               // if aliens touch bunkers, game over
                        GameOver();                                 // run game over
                }
        }
}

/* EnemyMovement
input: none
output: none
Moves enemies using EnemyMoveLeft, EnemyMoveRight, EnemyMoveDown
Moves right 30 times, moves left 30 times, then down, then starts over
*/
void EnemyMovement(void){
        if(horizontalCounter < 30){
                EnemyMoveRight();                         // move enemies right 30 times
                horizontalCounter++;                      // increment counter
                frame ^= 1;                               // toggle to cue animation
        }
        else if(horizontalCounter < 59){
                EnemyMoveLeft();                          // move enemies left 30 times
                horizontalCounter++;                      // increment counter
                frame ^= 1;                               // toggle to cue animation
        }
        else if(horizontalCounter == 59){
                EnemyMoveLeft();                          // move enemy left 1 last time
                horizontalCounter = 0;                    // reset horizontal movement counter
                verticalCounter++;                        // set vertical movement flag
                frame ^= 1;                               // toggle to cue animation
        }
        if(verticalCounter == 1){
                verticalCounter = 0;                      // reset vertical movement flag
                EnemyMoveDown();                          // enemies move down 1 pixel after moving left and right across screen once
        }
}

/* PlayerLaserMove
input: none
output: none
Moves player lasers up y-axis to enemies
*/
void PlayerLaserMove(void){
        int k = 0;
        for(;k < 1; k++){               
                if(PlayerLasers[k].dead == 0){                                                                          // checks if each player laser is alive
                        if(PlayerLasers[k].yPos > 29){                                                                  // if laser has not reached top of screen
                                ST7735_DrawBitmap(PlayerLasers[k].xPos, PlayerLasers[k].yPos, BlackLaser, 2, 8);        // clear old laser sprite
                                PlayerLasers[k].yPos -= PlayerLasers[k].speed;                                          // move player laser up screen by its speed
                        }
                        else{
                                PlayerLasers[k].dead = 1;                                                               // laser is removed
                                ST7735_DrawBitmap(PlayerLasers[k].xPos, PlayerLasers[k].yPos, BlackLaser, 2, 10);       
                                playerLaserCount--;                                                                     // decrease player laser count
                        }
                }
        }
}

/* EnemyLaserMove
input: none
output: none
Moves enemy lasers down y-axis towards player
*/
void EnemyLaserMove(void){
        int k = 0;
        for(;k < 18; k++){                                                                                                                                               // checks state of every enemy laser
                if(EnemyLasers[k].dead == 0){                                                                                 // if enemy laser is alive
                        if((EnemyLasers[k].yPos < 180) && (EnemyLasers[k].yPos > 29)){                                        // checks if enemy laser has reached bottom of screen
                                ST7735_DrawBitmap(EnemyLasers[k].xPos, EnemyLasers[k].yPos, BlackLaser, 2, 8);                // draw black space at original position of laser
                                EnemyLasers[k].yPos += EnemyLasers[k].speed;                                                  // moves enemy laser down screen
                        }
                        else{
                                EnemyLasers[k].dead = 1;                                                                      // removes enemy laser
                                ST7735_DrawBitmap(EnemyLasers[k].xPos, EnemyLasers[k].yPos, BlackLaser, 2, 10);
                                enemyLaserCount--;                                                                            // decrease enemy laser count
                        }
                }
        }
}


/* Movement
input: none
output: none
Moves objects that have motion
*/
void Movement(void){
        PlayerXMove();
        PlayerLaserMove();
        EnemyLaserMove();
        //EnemyMovement(); // uses a timer or else enemies move too quickly
}

/* DrawPauseScreen
input: none
output: none
Draws a pause screen while the game is paused
*/
void DrawPauseScreen(void){
  ST7735_FillScreen(0x0000);            // set screen to black
	ST7735_SetCursor(1, 3);
  ST7735_OutString("GAME PAUSED");
  ST7735_SetCursor(1, 4);

}

/* DrawGameOverScreen
input: none
output: none
Draws game over screen


void UpdateScore(int currentScore){ 
        if (GetLanguage()){
					ST7735_SetCursor(4, 1);
					ST7735_OutString("Puntuacion:");
				} else if (GetLanguage() == 0){
					ST7735_SetCursor(7, 1);
					ST7735_OutString("Score:");
				}
        LCD_OutDec(currentScore);
}
*/
void DrawGameOverScreen(void){
	if (GetLanguage()==0){
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(6, 1);
		ST7735_OutString("Score:");
		LCD_OutDec(score);
		ST7735_SetCursor(6, 5);
		ST7735_OutString("GAME OVER");
}
	else if (GetLanguage()){
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(4, 1);
		ST7735_OutString("Puntuacion:");
		LCD_OutDec(score);
		ST7735_SetCursor(3, 5);
		ST7735_OutString("JUEGO TERMINADO");	
	}
}

/* DrawVictoryScreen
input: none
output: none
Draws victory screen
*/
void DrawVictoryScreen(void){
	if (GetLanguage()==0){
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(7, 1);
		ST7735_OutString("Score:");
		LCD_OutDec(score);
		ST7735_SetCursor(8, 5);
		ST7735_OutString("YOU");
		ST7735_SetCursor(8, 7);
		ST7735_OutString("WIN");
	
} 
	else if (GetLanguage()){
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(7, 1);
		ST7735_OutString("Puntuacion:");
		LCD_OutDec(score);
		ST7735_SetCursor(8, 5);
		ST7735_OutString("TU");
		ST7735_SetCursor(8, 7);
		ST7735_OutString("GANAS");
}
}
/* DrawPlayer
input: none
output: none
Draws player model on LCD if player is alive, draws explosion if player has recently died
*/
void DrawPlayer(void){
        if(Player.hp > 0)
                ST7735_DrawBitmap(Player.xPos, Player.yPos, Player.image, 12, 8);               // draw player spaceship
        else{
                if(Player.dead == 1){
                        ST7735_DrawBitmap(Player.xPos, Player.yPos, Explosion, 16, 10);         // draw explosion upon death
                        Player.dead = 0;                                                                                                                                                                                                        // reset recent death flag
                }
                else
                        ST7735_DrawBitmap(Player.xPos, Player.yPos, BlackEnemy, 16, 10);        // draw black space after explosion
        }
}

/* DrawEnemy
input: none
output: none
Draws enemies on LCD if enemy is alive, draws explosion if enemy has recently died
*/
void DrawEnemy(void){
        int k = 0;
        for(; k < 18; k++){
                if(Enemies[k].hp > 0)                                                                                                                                                                                                                                                                           // check if enemy is alive
                        ST7735_DrawBitmap(Enemies[k].xPos, Enemies[k].yPos, Enemies[k].image[frame], 16, 10);   // draw enemy bitmap depending on frame state
                else{
                        if(Enemies[k].dead == 1){                                                                                                                                                                                                                                                       // check recent death flag
                                ST7735_DrawBitmap(Enemies[k].xPos, Enemies[k].yPos, Explosion, 16, 10);         // draw explosion
                                Enemies[k].dead = 0;                                                                                                                                                                                                                                                            // reset recent death flag
                        }
                        else
                                ST7735_DrawBitmap(Enemies[k].xPos, Enemies[k].yPos, BlackEnemy, 16, 10);        // draw black space over enemy after death
                }
        }
}

/* DrawBunker
input: none
output: none
Draws bunkers on LCD if they are alive
*/
void DrawBunker(void){
        int k = 0;
        for(;k < 3; k++){
                if(Bunkers[k].hp > 0)                                                                                                                                                                                                                                                                                                                           // check if bunker is alive
                        ST7735_DrawBitmap(Bunkers[k].xPos, Bunkers[k].yPos, Bunkers[k].image[Bunkers[k].hp - 1], 18, 5);   // draw bunker state depending on its health value
                else
                        ST7735_DrawBitmap(Bunkers[k].xPos, Bunkers[k].yPos, BlackBunker, 18, 5);                                                                                                        // if bunker is dead, draw black space
        }
}

/* DrawPlayerLaser
input: none
output: none
Draws Player lasers on LCD
*/
void DrawPlayerLaser(void){
        int k = 0;
        for(; k < 1; k++){                                                                                                                                                                                                                                                                                                                     
                if(PlayerLasers[k].dead == 0){                                                                                                                                                                                                                                                          // check if player lasers are alive
                        ST7735_DrawBitmap(PlayerLasers[k].xPos, PlayerLasers[k].yPos, PlayerLasers[k].image, 2, 10);// if player laser is alive, draw it
                   
                }
        }
}

/* 
DrawEnemyLaser
input: none
output: none
Draws Enemy lasers on LCD
*/
void DrawEnemyLaser(void){
        int k = 0;
        for(; k < 18; k++){
                if(EnemyLasers[k].dead == 0){                                                                                                                                                                                                                                                           // check if enemy lasers are alive
                        ST7735_DrawBitmap(EnemyLasers[k].xPos, EnemyLasers[k].yPos, EnemyLasers[k].image, 2, 10);       // if enemy laser is alive, draw it
                   
                }
        }
}

/* Draw
input: none
output: none
Draws entire frame on LCD
*/
void Draw(void){
        DrawPlayer();                           // draw player model
        DrawEnemy();                            // draw enemy models
        DrawBunker();                           // draw bunker models
        DrawPlayerLaser();                      // draw player laser models
        DrawEnemyLaser();                       // draw enemy laser models
}

/* GameOver
input: none
output: none
Implements game over state
*/
void GameOver(void){
        DrawGameOverScreen();   // draw game over screen
        gameRunning = 0;
        while(1){                                                               // infinite loop
        }
}

/* Victory
input: none
output: none
Implements victory state
*/
void Victory(void){
        if(enemiesAlive == 0){          // check number of alive enemies
                gameRunning = 0;
        }
}

/* PlayerFire
input: none
output: none
Fires a laser from player model if player has not exceeded maximum lasers
*/
void PlayerFire(void){
        if(playerLaserCount < 1){                                                                                                                                               // does not exceed max # of player lasers
                PlayerLasers[playerLaserCount].xPos = (Player.xPos + 5);        // laser is 2 x 10, places laser at center of player ship
                PlayerLasers[playerLaserCount].yPos = (Player.yPos - 8);        // places laser at front of player ship
                PlayerLasers[playerLaserCount].image = PlayerLaser;             // shoots player laser
                PlayerLasers[playerLaserCount].speed = 2;                       // speed that player laser moves at
                PlayerLasers[playerLaserCount].dead = 0;                        // player laser is live
                playerLaserCount++;  
								Sound_Shoot();  // play shoot sound
                                                                                                                                                           // increment count of player lasers
        }
}

/* 
EnemyFire
input: none
output: none
Cycles through a certain delay, then fires at random from a random
living enemy if maximum enemy lasers has not been surpassed.
*/
void EnemyFire(void){
        enemyFireDelay = RandomNumber(200);              // random rate of fire for enemy
				while(enemyFireDelay > 0){                  // random delay randomizes enemy firing scheme
                enemyFireDelay--;                   // decrement delay and return
        }
    
        uint8_t k = 5;                                                          // index for aliens
        for(; k < 18; k++){
                uint8_t fire = 0;                                               // allows alien to fire
                fire = RandomNumber(2);                 // boolean result decides if enemy fires

                if((Enemies[k].hp > 0) && (enemyLaserCount < 4) && (fire == 1)){        // fire if enemy is alive, there are less than max # of enemy lasers on screen, and boolean allows fire
                        EnemyLasers[k].xPos = (Enemies[k].xPos + (16 / 2));                                                     // places laser at center of width of alien
                        EnemyLasers[k].yPos = Enemies[k].yPos + 10;                                                                                     // places laser at bottom of alien
                        EnemyLasers[k].image = EnemyLaser;                                                                                                                      // assigns sprite of alien laser
                        EnemyLasers[k].speed = 1;                                                                                                                                                               // sets speed of laser
                        EnemyLasers[k].dead = 0;                                                                                                                                                                // enemy laser is live
                        enemyLaserCount++;  // increment laser count
                }
        }
}

/* PlayerHitReg
input: none
output: none
Check if player has been hit by laser
*/
void PlayerHitReg(void){
        if(Player.hp > 0){
                int k = 0;                                                                              // index for enemy lasers
                for(;k < 18; k++){
                        if((EnemyLasers[k].dead == 0)&&         
                                 ((EnemyLasers[k].xPos >= Player.xPos) && (EnemyLasers[k].xPos < (Player.xPos + 12))) &&
                           ((EnemyLasers[k].yPos >= (Player.yPos + 18)))){ // check if enemy laser is live, if enemy laser touches player model
                                if(Player.hp > 0){                             
                                        Player.hp--;                                            // if player is alive, decrement health
                                        if(Player.hp == 0){                     
                                                Player.dead = 1;                        // if player health reaches 0, set recent death flag
                                                Sound_Explosion();              // play explosion sound
																								gameOver = 1;                                   // set gameOver flag    
																								GameOver();                                             // run game over
               
                                        }
                                }

                                ST7735_DrawBitmap(EnemyLasers[k].xPos, EnemyLasers[k].yPos, BlackLaser, 2, 10); // draw black space at laser after hit
                                enemyLaserCount--;                              // decrement enemy laser count
                                EnemyLasers[k].dead = 1;        // kill enemy laser

                        }
                }
        }
}

/* LaserHitReg
input: none
output: none
Check if enemy laser has been hit by laser
*/
void LaserHitReg(void){
        int j = 0;                                                                                                                      // enemy array index
        int k = 0;                                                                                                                      // player laser array index
        for(; j < 4; j++){
                if(EnemyLasers[j].dead == 0){                                   // checks if enemy laser is alive
                        k = 0;                                                                                                                  // reset player laser index
                        for(;k < 1; k++){                                                                               // checks all player lasers
                                if((PlayerLasers[k].dead == 0)&&
                                        ((PlayerLasers[k].xPos >= EnemyLasers[j].xPos) && (PlayerLasers[k].xPos < (EnemyLasers[j].xPos + 16))) &&
                                        ((PlayerLasers[k].yPos <= (EnemyLasers[j].yPos + 5)) || (PlayerLasers[k].yPos >= (EnemyLasers[j].yPos - 5)))){ // if player laser is live and touching enemy model
                                        if(EnemyLasers[j].dead == 0){                   
                                                EnemyLasers[j].dead = 1;                                                        // if enemy is alive, decrement enemy health
                                                if(EnemyLasers[j].dead == 1){           
                                                        EnemyLasers[j].dead = 1;                                // if enemy has just died, set recent death flag
                                                        score += 5;                                                             // add point value of enemy to score
                                                        UpdateScore(score);                                     // update score
                                                }
                                        }

                                        playerLaserCount--;                                                     // decrement player laser counter
                                        PlayerLasers[k].dead = 1;                               // player laser is dead
                                        ST7735_DrawBitmap(PlayerLasers[k].xPos, PlayerLasers[k].yPos, BlackLaser, 2, 10); // draw black space at laser after hit                                                                                                       
                                }       
                        }
                }
        }
}

/* EnemyHitReg
input: none
output: none
Check if enemy has been hit by laser
*/
void EnemyHitReg(void){
        int j = 0;                                                                                                                      // enemy array index
        int k = 0;                                                                                                                      // player laser array index
        for(; j < 18; j++){
                if(Enemies[j].hp > 0){                                                          // checks if enemy is alive
                        k = 0;                                                                                                                  // reset player laser index
                        for(;k < 1; k++){                                                                               // checks all player lasers
                                if((PlayerLasers[k].dead == 0)&&
                                        ((PlayerLasers[k].xPos >= Enemies[j].xPos) && (PlayerLasers[k].xPos < (Enemies[j].xPos + 16))) &&
                                        ((PlayerLasers[k].yPos <= (Enemies[j].yPos + 10)))){ // if player laser is live and touching enemy model
                                        if(Enemies[j].hp > 0){                 
                                                Enemies[j].hp--;                                                        // if enemy is alive, decrement enemy health
                                                if(Enemies[j].hp == 0){         
                                                        Enemies[j].dead = 1;                            // if enemy has just died, set recent death flag
                                                        score += Enemies[j].points; // add point value of enemy to score
                                                        UpdateScore(score);                                     // update score
                                                        enemiesAlive--;                                                 // decrease number of alive enemies
                                                        enemiesKilled++;                                                // increase number of dead enemies
																												Sound_Killed();
																								}
                                        }

                                        playerLaserCount--;                                                     // decrement player laser counter
                                        PlayerLasers[k].dead = 1;                               // player laser is dead
                                        ST7735_DrawBitmap(PlayerLasers[k].xPos, PlayerLasers[k].yPos, BlackLaser, 2, 10); // draw black space at laser after hit                                                                                                       
                                }       
                        }
                }
        }
}

/* BunkerPlayerHitReg
input: none
output: none
Check if bunker has been hit by player laser
*/
void BunkerPlayerHitReg(void){
        int j = 0;                                                                                              // bunker array index
        int k = 0;                                                                                              // player laser array index
        for(; j < 3; j++){                                                              // check each bunker
                if(Bunkers[j].hp > 0){                                  // if bunker is alive
                        k = 0;                                                                                          // reset player laser index
                        for(;k < 1; k++){                                                       // check every player laser
                                if((PlayerLasers[k].dead == 0)&&
                                        ((PlayerLasers[k].xPos >= Bunkers[j].xPos) && (PlayerLasers[k].xPos < (Bunkers[j].xPos + 18)))){ // if player laser is live and touching bunker
                                        if(Bunkers[j].hp > 0){          // if bunker is alive, decrement bunker health
                                                Bunkers[j].hp--;       
                                                //Sound_Explosion();                      // play explosion sound
                                        }
                                        ST7735_DrawBitmap(PlayerLasers[k].xPos, PlayerLasers[k].yPos, BlackLaser, 2, 10); // draw black at position of laser
                                        playerLaserCount--;                             // decrement player laser count
                                        PlayerLasers[k].dead = 1;       // kill player laser
                                }
                        }
                }
        }
}

/* BunkerEnemyHitReg
input: none
output: none
Check if bunker has been hit by enemy laser
*/
void BunkerEnemyHitReg(void){
        int j = 0;                                                                                              // bunker array index
        int k = 0;                                                                                              // enemy laser array index
        for(; j < 3; j++){                                                              // check each bunker for:
                if(Bunkers[j].hp > 0){                                  // if bunker is alive
                        k = 0;                                                                                          // reset enemy laser array index
                        for(;k < 18; k++){                                                       // check each enemy laser for:
                                if((EnemyLasers[k].dead == 0)&&
                                        ((EnemyLasers[k].xPos >= Bunkers[j].xPos) && (EnemyLasers[k].xPos < (Bunkers[j].xPos + 18))) &&
                                        ((EnemyLasers[k].yPos >= (Bunkers[j].yPos - 10)))){ // if enemy laser is live, if enemy laser touches bunker
                                        if(Bunkers[j].hp > 0){          // if bunker is alive
                                                Bunkers[j].hp--;                                // decrement health
                                                //Sound_Explosion();                      // play explosion sound
                                        }

                                        ST7735_DrawBitmap(EnemyLasers[k].xPos, EnemyLasers[k].yPos, BlackLaser, 2, 10);
                                        enemyLaserCount--;                              // decrement enemy laser count
                                        EnemyLasers[k].dead = 1;        // kill enemy laser after hit
                                }
                        }
                }
        }
}

/* HitReg
input: none
output: none
Checks hit registration.
*/
void HitReg(void){
        BunkerEnemyHitReg();            // hitreg of bumpers from enemy lasers
        BunkerPlayerHitReg();           // hitreg of bumpers from player lasers
        PlayerHitReg();                                 // hitreg of player from enemy lasers
        EnemyHitReg();                                  // hitreg of enemies from player lasers
}


/* EraseLasers
input: none
output: none
Erases all offscreen lasers
*/
void EraseLasers(void){
        int k = 0;
        for(;k < 4; k++){
                if(EnemyLasers[k].yPos < Bunkers[k].yPos){
                }
        }
}

/* Pause
input: none
output: none
Pauses the game.
*/
void Pause(void){
        int pauseFlag;                                                          // flag for game pause

        pauseFlag = Button1Press();             // checks if pause button was pressed
        if(pauseFlag){
                if(gameRunning){
                        DrawPauseScreen();
                }
        }
        while(pauseFlag){
                pauseFlag = Button1Press(); // polls pause button state while paused
                if(gameRunning){       
                        if(!pauseFlag){
                                ST7735_FillScreen(0x0000);// set screen to black
                                UpdateScore(score);
                        }
                }
        }
}

/* GetGameStatus
input: none
output: int
Returns status of the game
*/
int GetGameStatus(void){
        return gameRunning;
}

/* SetGameStage
input: int
output: none
Sets stage of game
*/
void SetGameStage(int newStage){
        stage = newStage;
}

void SpaceInvader(void){
        Movement();
				EnemyFire();      
				HitReg();
        Pause();
        Draw();
        Victory();
}

