#include <Windows.h>
#include <math.h>
#include "PlayerInput.h"
#include <stdio.h>
#include "Transform.h"

#define M_PI_2 1.5707963267948966 // π/2 (90 degrees)
#define GRID_SIZE 100

POINT lastMousePos;
boolean firstMouse = TRUE;

int IsBlockAt(int x, int y, int z)
{
    Position pos={x,y,z};
    return IsPointInsideRectangle3D(&worldMap[x + GRID_SIZE][z + GRID_SIZE],&pos);
}

void HandleInput()
{
    float moveSpeed = 0.1f;
    float strafeSpeed = 0.1f;
    float rotateSpeed = 0.1f;
    float newX, newY, newZ;

    if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
    {
        playerX = playerX + cos(playerYaw) * moveSpeed;
        playerZ = playerZ + sin(playerYaw) * moveSpeed;
    }

    if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
    {
        playerX = playerX - cos(playerYaw) * moveSpeed;
        playerZ = playerZ - sin(playerYaw) * moveSpeed;
    }

    if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
    {
        playerX = playerX - cos(playerYaw + M_PI_2) * strafeSpeed;
        playerZ = playerZ - sin(playerYaw + M_PI_2) * strafeSpeed;
    }

    if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
    {
        playerX = playerX + cos(playerYaw + M_PI_2) * strafeSpeed;
        playerZ = playerZ + sin(playerYaw + M_PI_2) * strafeSpeed;
    }

    if (GetAsyncKeyState('Q'))
    {
        playerYaw -= rotateSpeed;
    }
    if (GetAsyncKeyState('E'))
    {
        playerYaw += rotateSpeed;
    }
    if (GetAsyncKeyState(' ') && isOnGround)
    {
        playerVelocityY = jumpStrength;
        isOnGround = 0;
    }
}

void FollowCursor()
{
    float rotateSpeed = 0.002f;
    float pitchLimit = 0.506f;

    POINT mousePos;
    GetCursorPos(&mousePos);

    RECT rect;
    GetWindowRect(hwnd, &rect);

    if (mousePos.x < rect.left || mousePos.x > rect.right ||
        mousePos.y < rect.top || mousePos.y > rect.bottom)
    {
        firstMouse = TRUE;
        return;
    }
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    POINT center = {windowWidth / 2, windowHeight / 2};
    ClientToScreen(hwnd, &center);

    if (firstMouse)
    {
        SetCursorPos(center.x, center.y);
        lastMousePos = mousePos;
        firstMouse = FALSE;
    }

    float deltaX = (mousePos.x - lastMousePos.x) * rotateSpeed;
    float deltaY = (mousePos.y - lastMousePos.y) * rotateSpeed;

    playerYaw += deltaX;
    playerPitch -= deltaY;
    if (playerPitch > pitchLimit)
        playerPitch = pitchLimit;
    if (playerPitch < -pitchLimit)
        playerPitch = -pitchLimit;

    lastMousePos = mousePos;
}