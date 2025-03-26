#ifndef RENDER_WORLD_H
#define RENDER_WORLD_H

#include <windows.h>
#include <gl/GL.h>
#include "Transform.h"

extern GLuint greenBlockTexture;
extern float rotateAngle;

void RenderWorld();
void RenderSky();
float PerlinNoise(float x, float z);
void FreeHeightMap();
void GenerateHeightMap();
void AllocateHeightMap();
void RenderRectangle3D(const Rectangle3D *rect);
#endif