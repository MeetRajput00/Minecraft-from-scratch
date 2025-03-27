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

float GetTerrainHeight(float x, float z) {
    int mapX = (int)(x + GRID_SIZE);
    int mapZ = (int)(z + GRID_SIZE);
    
    if (mapX >= 0 && mapX < GRID_SIZE * 2 && mapZ >= 0 && mapZ < GRID_SIZE * 2) {
        return worldMap[mapZ][mapX].height-0.3f;
    }
    return 0.0f;
}

void HandleInput()
{
    float moveSpeed = 0.1f;
    float strafeSpeed = 0.1f;
    float rotateSpeed = 0.1f;
    float newX, newY, newZ;

    if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
    {
        newX = playerX + cos(playerYaw) * moveSpeed;
        newZ = playerZ + sin(playerYaw) * moveSpeed;
        float newTerrainHeight = GetTerrainHeight(newX, newZ);
        
        float heightDiff = playerY - newTerrainHeight;
        if (heightDiff <= 1.0f) {
            playerX = newX;
            playerY = fmax(playerY - 0.1f, newTerrainHeight);
            playerZ = newZ;
        }
        else if (!IsBlockAt((int)newX, (int)playerY, (int)newZ)) {
            playerX = newX;
            playerZ = newZ;
        }
    }

    if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
    {
        newX = playerX - cos(playerYaw) * moveSpeed;
        newZ = playerZ - sin(playerYaw) * moveSpeed;
        float newTerrainHeight = GetTerrainHeight(newX, newZ);
        
        float heightDiff = playerY - newTerrainHeight;
        if (heightDiff <= 1.5f) {
            playerX = newX;
            playerY = fmax(playerY - 0.1f, newTerrainHeight);
            playerZ = newZ;
        }
        else if (!IsBlockAt((int)newX, (int)playerY, (int)newZ)) {
            playerX = newX;
            playerZ = newZ;
        }
    }

    if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
    {
        newX = playerX - cos(playerYaw + M_PI_2) * strafeSpeed;
        newZ = playerZ - sin(playerYaw + M_PI_2) * strafeSpeed;
        float newTerrainHeight = GetTerrainHeight(newX, newZ);
        
        float heightDiff = playerY - newTerrainHeight;
        if (heightDiff <= 1.5f) {
            playerX = newX;
            playerY = fmax(playerY - 0.1f, newTerrainHeight);
            playerZ = newZ;
        }
        else if (!IsBlockAt((int)newX, (int)playerY, (int)newZ)) {
            playerX = newX;
            playerZ = newZ;
        }
    }

    if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
    {
        newX = playerX + cos(playerYaw + M_PI_2) * strafeSpeed;
        newZ = playerZ + sin(playerYaw + M_PI_2) * strafeSpeed;
        float newTerrainHeight = GetTerrainHeight(newX, newZ);
        
        float heightDiff = playerY - newTerrainHeight;
        if (heightDiff <= 1.5f) {
            playerX = newX;
            playerY = fmax(playerY - 0.1f, newTerrainHeight);
            playerZ = newZ;
        }
        else if (!IsBlockAt((int)newX, (int)playerY, (int)newZ)) {
            playerX = newX;
            playerZ = newZ;
        }
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