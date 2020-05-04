//Header file for GameEngine.c
#include <stdint.h>

#ifndef GAMEENGINE_H // do not include more than once
#define GAMEENGINE_H

uint32_t RandomNumber(uint32_t number);
void UpdateScore(int currentScore);
void SpaceInvader_Init(void);
void PlayerXMove(void);
void EnemyMoveRight(void);
void EnemyMoveLeft(void);
void EnemyMoveDown(void);
void EnemyMovement(void);
void PlayerLaserMove(void);
void EnemyLaserMove(void);
void Movement(void);
void DrawPauseScreen(void);
void DrawGameOverScreen(void);
void DrawVictoryScreen(void);
void DrawPlayer(void);
void DrawEnemy(void);
void DrawBunker(void);
void DrawPlayerLaser(void);
void DrawEnemyLaser(void);
void Draw(void);
void GameOver(void);
void Victory(void);
void PlayerFire(void);
void EnemyFire(void);
void PlayerHitReg(void);
void LaserHitReg(void);
void EnemyHitReg(void);
void BunkerPlayerHitReg(void);
void BunkerEnemyHitReg(void);
void HitReg(void);
void EraseLasers(void);
void Pause(void);
int GetGameStatus(void);
void SetGameStage(int newStage);
void SpaceInvader(void);

#endif
