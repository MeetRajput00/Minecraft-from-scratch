#include <Windows.h>
#include <math.h>
#include "PlayerInput.h"
#include <stdio.h>

#define M_PI_2 1.5707963267948966 // π/2 (90 degrees)

POINT lastMousePos;
boolean firstMouse = TRUE;

void HandleInput()
{
    float moveSpeed = 0.1f;   // Movement speed
    float strafeSpeed = 0.1f; // Strafing speed
    float rotateSpeed = 0.2f;

    // Move forward/backward along Z-axis (Q and W keys)
    if (GetAsyncKeyState(VK_UP) || GetAsyncKeyState('W'))
    {
        // Move forward in the direction the camera is facing
        playerX += cos(playerYaw) * moveSpeed;
        playerZ += sin(playerYaw) * moveSpeed;
    }

    if (GetAsyncKeyState(VK_DOWN) || GetAsyncKeyState('S'))
    {
        // Move backward in the opposite direction
        playerX -= cos(playerYaw) * moveSpeed;
        playerZ -= sin(playerYaw) * moveSpeed;
    }

    // Move left/right relative to player's yaw (A and D keys)
    if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))
    { // Move left
        playerX -= cos(playerYaw + M_PI_2) * strafeSpeed;
        playerZ -= sin(playerYaw + M_PI_2) * strafeSpeed;
    }
    if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))
    { // Move right
        playerX += cos(playerYaw + M_PI_2) * strafeSpeed;
        playerZ += sin(playerYaw + M_PI_2) * strafeSpeed;
    }

    if (GetAsyncKeyState('Q'))
    { // Move right
        playerYaw -= sin(playerYaw + M_PI_2) * rotateSpeed;
    }
    if (GetAsyncKeyState('E'))
    { // Move right
        playerYaw += sin(playerYaw + M_PI_2) * rotateSpeed;
    }
    if (GetAsyncKeyState(' ') && isOnGround)
    { // Jump
        playerVelocityY = jumpStrength;
        isOnGround = 0; // Not on ground anymore
    }
}

void FollowCursor()
{
    float rotateSpeed = 0.002f; // Adjust sensitivity
    float pitchLimit = 0.506f;  // Limit vertical rotation (avoid flipping)

    // Get current mouse position
    POINT mousePos;
    GetCursorPos(&mousePos);

    // Get window center
    RECT rect;
    GetWindowRect(hwnd, &rect);

    // Check if mouse is inside the window
    if (mousePos.x < rect.left || mousePos.x > rect.right ||
        mousePos.y < rect.top || mousePos.y > rect.bottom)
    {
        firstMouse = TRUE; // Reset tracking when mouse leaves window
        return;
    }
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;
    POINT center = {windowWidth / 2, windowHeight / 2};
    ClientToScreen(hwnd, &center); // Convert to screen coordinates

    // If first frame, initialize lastMousePos
    if (firstMouse)
    {
        SetCursorPos(center.x, center.y);
        lastMousePos = mousePos;
        firstMouse = FALSE;
    }

    // Calculate mouse movement delta
    float deltaX = (mousePos.x - lastMousePos.x) * rotateSpeed;
    float deltaY = (mousePos.y - lastMousePos.y) * rotateSpeed;

    // Update rotation based on delta
    playerYaw += deltaX;   // Rotate left/right
    playerPitch -= deltaY; // Rotate up/down (invert Y for natural feel)
    // Clamp pitch to avoid flipping
    if (playerPitch > pitchLimit)
        playerPitch = pitchLimit;
    if (playerPitch < -pitchLimit)
        playerPitch = -pitchLimit;

    // Store last position
    lastMousePos = mousePos;
}