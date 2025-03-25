#ifndef PLAYER_INPUT_H
#define PLAYER_INPUT_H

#include <Windows.h>

extern float playerX, playerY, playerZ, playerYaw, playerPitch;
extern HWND hwnd;

void FollowCursor();
void HandleInput();

#endif