#ifndef RENDER_WORLD_H
#define RENDER_WORLD_H

#include <GL/gl.h>  // Ensure OpenGL types are recognized

extern GLuint greenBlockTexture;
extern float rotateAngle;

void RenderWorld();
void RenderSky();
void RenderBlock();
float PerlinNoise(float x, float z);
void FreeHeightMap();
void GenerateHeightMap();
void AllocateHeightMap();
#endif