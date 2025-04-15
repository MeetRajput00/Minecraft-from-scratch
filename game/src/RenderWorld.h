#ifndef RENDER_WORLD_H
#define RENDER_WORLD_H

#include <windows.h>
#include <gl/GL.h>
#include "Transform.h"

extern GLuint greenBlockTexture;
extern float rotateAngle;

// Define grid size
#define GRID_SIZE 100

// Create height map as a dynamically allocated 2D array
extern Rectangle3D **worldMap;
extern float playerX,playerZ;

void RenderWorld();
void RenderSky();
float PerlinNoise(float x, float z);
void FreeHeightMap();
void GenerateHeightMap();
void AllocateHeightMap();
void RenderRectangle3D(const Rectangle3D *rect);
#endif