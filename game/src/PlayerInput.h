#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include <Windows.h>
#include "Transform.h"

#define GRID_SIZE 100


extern float playerX, playerY, playerZ, playerYaw, playerPitch, playerVelocityY, jumpStrength;
extern HWND hwnd;
extern int isOnGround;
extern Rectangle3D **worldMap;


void FollowCursor();
void HandleInput();

#endif