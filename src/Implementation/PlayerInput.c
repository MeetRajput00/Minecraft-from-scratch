#include <Windows.h>
#include <math.h>
#include "PlayerInput.h"


#define M_PI_2 1.5707963267948966  // π/2 (90 degrees)

void HandleInput() {
    float moveSpeed = 0.1f;  // Movement speed
    float strafeSpeed = 0.1f;  // Strafing speed
    float rotateSpeed=0.3f;
    // Move forward/backward along Z-axis (Q and W keys)
    if (GetAsyncKeyState(VK_UP)) { 
        // Move forward in the direction the camera is facing
        playerX += cos(playerYaw) * moveSpeed;
        playerZ += sin(playerYaw) * moveSpeed;
    }
    
    if (GetAsyncKeyState(VK_DOWN)) { 
        // Move backward in the opposite direction
        playerX -= cos(playerYaw) * moveSpeed;
        playerZ -= sin(playerYaw) * moveSpeed;
    }

    // Move left/right relative to player's yaw (A and D keys)
    if (GetAsyncKeyState(VK_LEFT)) {  // Move left
        playerX -= cos(playerYaw + M_PI_2) * strafeSpeed;
        playerZ -= sin(playerYaw + M_PI_2) * strafeSpeed;
    }
    if (GetAsyncKeyState(VK_RIGHT)) { // Move right
        playerX += cos(playerYaw + M_PI_2) * strafeSpeed;
        playerZ += sin(playerYaw + M_PI_2) * strafeSpeed;
    }
    
    // if (GetAsyncKeyState(VK_UP)){ 
    //     playerY += 0.1f;
    // }
    // if (GetAsyncKeyState(VK_DOWN)){
    //     playerY -= 0.1f;
    // }

     // Rotate player left/right (E and R keys)
     if (GetAsyncKeyState('R')) { // Turn right (clockwise)
        playerYaw += rotateSpeed;
    }
    if (GetAsyncKeyState('E')) { // Turn left (counterclockwise)
        playerYaw -= rotateSpeed;
    }
}