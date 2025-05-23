#include <windows.h>
#include <stdlib.h> // For malloc, free
#include <stdio.h>  // For printf (debugging)
#include <math.h>   // For sin, cos, floor
#include <stdlib.h> // For rand()
#include <time.h>   // For time()
#include <gl/GL.h>

#include "RenderWorld.h"
#include "Transform.h"
#include "FrustumCulling.h"

// Define grid size
#define GRID_SIZE 100

// Create height map as a dynamically allocated 2D array
float **heightMap;

// Function to allocate memory for height map
void AllocateHeightMap()
{
    int size = GRID_SIZE * 2 + 1; // Total grid dimensions

    // Allocate array of pointers (rows)
    heightMap = (float **)malloc(size * sizeof(float *));

    // Allocate each row
    for (int i = 0; i < size; i++)
    {
        heightMap[i] = (float *)malloc(size * sizeof(float));
    }
}

// Function to generate the terrain heights using Perlin Noise
void GenerateHeightMap()
{
    float noiseScale = 0.2f; // Controls terrain roughness
    float maxHeight = 3.0f;  // Maximum height variation

    for (int x = -GRID_SIZE; x <= GRID_SIZE; x++)
    {
        for (int z = -GRID_SIZE; z <= GRID_SIZE; z++)
        {
            heightMap[x + GRID_SIZE][z + GRID_SIZE] = PerlinNoise(x * noiseScale, z * noiseScale) * maxHeight;
        }
    }
}

// Free allocated memory
void FreeHeightMap()
{
    int size = GRID_SIZE * 2 + 1;

    for (int i = 0; i < size; i++)
    {
        free(heightMap[i]); // Free each row
    }
    free(heightMap); // Free main array
}
void RenderSky()
{
    glDisable(GL_DEPTH_TEST); // Disable depth test to ensure the skybox is always drawn in the background
    glDisable(GL_LIGHTING);   // No lighting effects on the skybox

    float skySize = 50.0f; // Size of the skybox

    glBegin(GL_QUADS);

    // Sky color
    glColor3f(0.5f, 0.7f, 1.0f);

    // Front
    glVertex3f(-skySize, -skySize, -skySize);
    glVertex3f(skySize, -skySize, -skySize);
    glVertex3f(skySize, skySize, -skySize);
    glVertex3f(-skySize, skySize, -skySize);

    // Back
    glVertex3f(-skySize, -skySize, skySize);
    glVertex3f(skySize, -skySize, skySize);
    glVertex3f(skySize, skySize, skySize);
    glVertex3f(-skySize, skySize, skySize);

    // Left
    glVertex3f(-skySize, -skySize, -skySize);
    glVertex3f(-skySize, -skySize, skySize);
    glVertex3f(-skySize, skySize, skySize);
    glVertex3f(-skySize, skySize, -skySize);

    // Right
    glVertex3f(skySize, -skySize, -skySize);
    glVertex3f(skySize, -skySize, skySize);
    glVertex3f(skySize, skySize, skySize);
    glVertex3f(skySize, skySize, -skySize);

    // Top
    glVertex3f(-skySize, skySize, -skySize);
    glVertex3f(skySize, skySize, -skySize);
    glVertex3f(skySize, skySize, skySize);
    glVertex3f(-skySize, skySize, skySize);

    glEnd();

    glEnable(GL_LIGHTING);   // Re-enable lighting
    glEnable(GL_DEPTH_TEST); // Re-enable depth test
}

void RenderWorld()
{
    glTranslatef(0.0f, 0.0f, -10.0f); // Move the world back
    glBindTexture(GL_TEXTURE_2D, greenBlockTexture);
    float spacing = 2.2f; // Spacing between blocks
    Rectangle3D block = {{0.0f, 0.0f, 0.0f}, 2.0f, 3.0f, 1.5f};
    for (int x = -GRID_SIZE; x <= GRID_SIZE; x++)
    {
        for (int z = -GRID_SIZE; z <= GRID_SIZE; z++)
        {
            float height = heightMap[x + GRID_SIZE][z + GRID_SIZE]; // Use precomputed height
            // float rotationOffset = (rand() % 10) - 5; // Slight random rotation variation
            //  Compute the block's world position
            Rectangle3D worldBlock = {
                {x * spacing, height, z * spacing}, // Position in world space
                block.width,
                block.height,
                block.depth};

            // Perform frustum culling
            if (!IsInFrustumRectangle(&worldBlock))
            {
                continue; // Skip rendering if block is outside the view frustum
            }
            glPushMatrix();
            glTranslatef(x * spacing, height, z * spacing); // Position each block with varied height
            glRotatef(rotateAngle, 1.0f, 1.0f, 0.0f);

            glBegin(GL_QUADS);
            RenderRectangle3D(&block);
            glEnd();

            glPopMatrix();
        }
    }
}
// Simple Perlin Noise function (based on gradient noise)
float PerlinNoise(float x, float z)
{
    int n = (int)x + (int)z * 57;
    n = (n << 13) ^ n;
    float noise = (1.0f - ((n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f);
    return noise;
}

void RenderRectangle3D(const Rectangle3D *rect)
{
    float x = rect->pos.x;
    float y = rect->pos.y;
    float z = rect->pos.z;
    float w = rect->width;
    float h = rect->height;
    float d = rect->depth;

    // Front Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z + d);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + w, y, z + d);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + w, y + h, z + d);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y + h, z + d);

    // Back Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + w, y, z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + w, y + h, z);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y + h, z);

    // Left Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x, y, z + d);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x, y + h, z + d);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y + h, z);

    // Right Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x + w, y, z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + w, y, z + d);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + w, y + h, z + d);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x + w, y + h, z);

    // Top Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y + h, z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + w, y + h, z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + w, y + h, z + d);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y + h, z + d);

    // Bottom Face
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(x, y, z);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f(x + w, y, z);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f(x + w, y, z + d);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(x, y, z + d);
}
