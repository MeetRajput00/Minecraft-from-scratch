#include <windows.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <math.h>
#include <stdio.h>

#define M_PI_2 1.5707963267948966  // π/2 (90 degrees)

#include "src/RenderWorld.h"
#include "src/RenderPlayer.h"


#define STB_IMAGE_IMPLEMENTATION
#include "include/stb_image.h"

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void InitOpenGL(HWND hwnd);
void Render();
GLuint LoadTexture(const char* filename);
void Resize(int width, int height);
void CleanupOpenGL();
void HandleInput();

// OpenGL rendering context
HDC hDC;
HGLRC hRC;
GLuint greenBlockTexture;
float rotateAngle=-30.0f;
float playerX = 0.0f, playerY = 0.0f, playerZ = -5.0f;
float playerYaw = 0.0f;  // Player rotation angle (left/right)
float playerPitch = 0.0f;  // Player pitch (up/down)
float cameraDistance = 2.0f;  // Distance for third-person view

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    const char CLASS_NAME[] = "OpenGLWindow";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.style = CS_OWNDC;  // Allows OpenGL to use a private device context

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "OpenGL Green Cube", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);
    InitOpenGL(hwnd);

    MSG msg = { 0 };
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            SwapBuffers(hDC);
        }
    }

    CleanupOpenGL();
    return 0;
}

void InitOpenGL(HWND hwnd)
{
    hDC = GetDC(hwnd);

    PIXELFORMATDESCRIPTOR pfd = { sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        24, 8, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

    int pixelFormat = ChoosePixelFormat(hDC, &pfd);
    SetPixelFormat(hDC, pixelFormat, &pfd);

    hRC = wglCreateContext(hDC);
    wglMakeCurrent(hDC, hRC);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    greenBlockTexture = LoadTexture("D://New folder (4)//assets//greenCubeTexture.png"); // Load the texture
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void Render() {
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
        camX, camY, camZ,  // Camera position (eye)
        playerX, playerY, playerZ,  // Look at player position
        0.0f, 1.0f, 0.0f   // Up vector
    );

    // Render the world
    RenderWorld();
    RenderPlayer();
}

GLuint LoadTexture(const char* filename) {
    GLuint textureID;
    int width, height, numChannels;
    
    // Load image file
    unsigned char* data = stbi_load(filename, &width, &height, &numChannels, 0);
    if (!data) {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    glGenTextures(1, &textureID);        // Generate texture ID
    glBindTexture(GL_TEXTURE_2D, textureID);  // Bind it

    // Determine image format
    GLenum format = (numChannels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    
    // Set texture parameters (repeat and filter)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);  // Free image memory
    return textureID;       // Return texture ID
}

void Resize(int width, int height)
{
    if (height == 0) height = 1;
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

void HandleInput() {
    float moveSpeed = 0.1f;  // Movement speed
    float strafeSpeed = 0.1f;  // Strafing speed

    // Move forward/backward along Z-axis (Q and W keys)
    if (GetAsyncKeyState('W')) { 
        // Move forward in the direction the camera is facing
        playerX += cos(playerYaw) * moveSpeed;
        playerZ += sin(playerYaw) * moveSpeed;
    }
    
    if (GetAsyncKeyState('Q')) { 
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
    
    if (GetAsyncKeyState(VK_UP)){ 
        playerY += 0.1f;
    }
    if (GetAsyncKeyState(VK_DOWN)){
        playerY -= 0.1f;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_SIZE:
            Resize(LOWORD(lParam), HIWORD(lParam));
            return 0;

        case WM_PAINT:
            Render();
            SwapBuffers(hDC);
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
