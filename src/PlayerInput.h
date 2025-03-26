#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include <Windows.h>

extern float playerX, playerY, playerZ, playerYaw, playerPitch, playerVelocityY, jumpStrength;
extern HWND hwnd;
extern int isOnGround;

void FollowCursor();
void HandleInput();

#endif