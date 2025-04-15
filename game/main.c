#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>

#include "src/RenderWorld.h"
#include "src/RenderPlayer.h"
#include "src/RenderSun.h"
#include "src/Lightning.h"
#include "src/Texture.h"
#include "src/PlayerInput.h"
#include "src/FrustumCulling.h"
#include "src/Transform.h"


LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitOpenGL(HWND hwnd);
void Render();
void Resize(int width, int height);
void CleanupOpenGL();
void UpdatePhysics(float deltaTime);
// OpenGL rendering context
HDC hDC;
HGLRC hRC;
HWND hwnd;
GLuint greenBlockTexture;
float rotateAngle = 0.0f;
float playerX = 0.0f, playerY = 10.0f, playerZ = -5.0f;
float playerYaw = 0.0f;   // Player rotation angle (left/right)
float playerPitch = 0.0f; // Player pitch (up/down)

float playerVelocityY = 0.0f; // Vertical velocity
float gravity = -9.8f;        // Gravity strength
float groundY = 3.50f;        // Ground height (modify for terrain)
float jumpStrength = 5.0f;    // Jumping force
int isOnGround = 0;           // 1 if player is on the ground

float cameraDistance = 2.0f; // Distance for third-person view
float sunRotateAngle = 1.0f;

// Define grid size
#define GRID_SIZE 100

// Create height map as a dynamically allocated 2D array
Rectangle3D **worldMap;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "OpenGLWindow";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_OWNDC; // Allows OpenGL to use a private device context

    RegisterClass(&wc);

    hwnd = CreateWindowEx(
        0, CLASS_NAME, "OpenGL Green Cube", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    InitOpenGL(hwnd);

    AllocateHeightMap();
    GenerateHeightMap();
    SetupLighting();
    SetupSunLighting();
    SetupMaterial();

    const float TARGET_FPS = 60.0f;
    const float FRAME_TIME = 1.0f / TARGET_FPS; // ~16.67ms per frame
    LARGE_INTEGER freq, lastTime, currentTime;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&lastTime);

    MSG msg = {0};
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        // Measure elapsed time
        QueryPerformanceCounter(&currentTime);
        float deltaTime = (float)(currentTime.QuadPart - lastTime.QuadPart) / freq.QuadPart;

        if (deltaTime >= FRAME_TIME)
        {
            lastTime = currentTime; // Reset timer

            // Update & Render
            UpdatePhysics(deltaTime);
            HandleInput();
            FollowCursor();
            Render();
            SwapBuffers(hDC);
        }
    }
    FreeHeightMap();
    CleanupOpenGL();
    return 0;
}

float CheckCollision(float x, float y, float z) {
    // Check collision with terrain using the height map
    int mapX = (int)(x + GRID_SIZE/2);
    int mapZ = (int)(z + GRID_SIZE/2);
    
    // Ensure we're within bounds
    if (mapX >= 0 && mapX < GRID_SIZE && mapZ >= 0 && mapZ < GRID_SIZE) {
        return worldMap[mapZ][mapX].height;
    }
    return groundY; // Default ground height if out of bounds
}

void UpdatePhysics(float deltaTime)
{
    // Apply gravity if not on ground
    if (!isOnGround)
    {
        playerVelocityY += gravity*deltaTime;
        float nextY = playerY + playerVelocityY * deltaTime;
        
        // Get the height of the terrain at player's position
        float terrainHeight = CheckCollision(playerX, nextY, playerZ);
        // Check for collision
        if (nextY <= terrainHeight)
        {
            playerY = terrainHeight;
            playerVelocityY = 0.0f;
            isOnGround = 1;
        }
        else
        {
            playerY = nextY;
        }
    }
    else
    {
        // Check if we're still on ground
        float terrainHeight = CheckCollision(playerX, playerY, playerZ);
        if (playerY > terrainHeight + 0.1f) // Small threshold to prevent floating point issues
        {
            isOnGround = 0;
        }
    }
}

void InitOpenGL(HWND hwnd)
{
    hDC = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = {sizeof(PIXELFORMATDESCRIPTOR), 1,
                                 PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                                 PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                 24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0};

    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    greenBlockTexture = LoadTexture("D://New folder (4)//game//assets//greenCubeTexture.png"); // Load the texture
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void Render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    // Calculate the camera direction (based on yaw & pitch)
    float lookX = cos(playerYaw) * cos(playerPitch);
    float lookY = sin(playerPitch);
    float lookZ = sin(playerYaw) * cos(playerPitch);

    // Compute the camera position for a third-person view
    float camX = playerX - lookX * cameraDistance;
    float camY = playerY - lookY * cameraDistance;
    float camZ = playerZ - lookZ * cameraDistance;

    // Attach the camera to the player
    gluLookAt(
        camX, camY, camZ,          // Camera position (eye)
        playerX, playerY, playerZ, // Look at player position
        0.0f, 1.0f, 0.0f           // Up vector
    );

    // ExtractFrustum();
    //  Render the world
    RenderSky();
    RenderSun(sunRotateAngle);
    RenderWorld();
    // RenderPlayer();
}

void Resize(int width, int height)
{
    if (height == 0)
        height = 1;
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)width / (GLfloat)height, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
}

void CleanupOpenGL()
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(GetActiveWindow(), hDC);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_SIZE:
        Resize(LOWORD(lParam), HIWORD(lParam));
        return 0;

    case WM_PAINT:
        return 0;

    case WM_KEYDOWN:
        HandleInput();
        break;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
