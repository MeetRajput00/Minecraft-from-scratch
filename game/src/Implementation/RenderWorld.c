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

GLuint blockDisplayList = 0;

// Function to allocate memory for height map
void AllocateHeightMap()
{
    int size = GRID_SIZE * 2 + 1; // Total grid dimensions

    // Allocate array of pointers (rows)
    worldMap = (Rectangle3D **)malloc(size * sizeof(Rectangle3D *));

    // Allocate each row
    for (int i = 0; i < size; i++)
    {
        worldMap[i] = (Rectangle3D *)malloc(size * sizeof(Rectangle3D));
    }
}

// Add these helper functions for improved Perlin noise
float Fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float Lerp(float a, float b, float t) {
    return a + t * (b - a);
}

float Grad(int hash, float x, float y) {
    int h = hash & 15;
    float grad = 1.0f + (h & 7);
    if (h & 8) grad = -grad;
    return (h & 1 ? x : y) * grad;
}

// Replace the existing PerlinNoise function with this improved version
float PerlinNoise(float x, float y) {
    const int p[512] = { 
        // ... your permutation table here (256 values repeated twice)
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
        247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
        // ... repeat the above values ...
        151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,190,6,148,
        247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,
    };

    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;
    x -= floor(x);
    y -= floor(y);
    
    float u = Fade(x);
    float v = Fade(y);
    
    int A = p[X] + Y;
    int B = p[X + 1] + Y;
    
    return Lerp(
        Lerp(Grad(p[A], x, y), Grad(p[B], x - 1, y), u),
        Lerp(Grad(p[A + 1], x, y - 1), Grad(p[B + 1], x - 1, y - 1), u),
        v
    ) * 0.5f + 0.5f;
}

// Update the GenerateHeightMap function
void GenerateHeightMap() {
    float noiseScale = 0.03f;    // Make this smaller for larger features
    float heightScale = 5.0f;   // Increase for more dramatic height differences
    float persistence = 0.5f;    // Controls how much each octave contributes
    int octaves = 4;            // Number of layers of noise
    
    for (int x = -GRID_SIZE; x <= GRID_SIZE; x++) {
        for (int z = -GRID_SIZE; z <= GRID_SIZE; z++) {
            float amplitude = 1.0f;
            float frequency = 1.0f;
            float height = 0.0f;
            float maxValue = 0.0f;
            
            // Combine multiple octaves of noise
            for (int o = 0; o < octaves; o++) {
                float sampleX = x * noiseScale * frequency;
                float sampleZ = z * noiseScale * frequency;
                
                height += PerlinNoise(sampleX, sampleZ) * amplitude;
                maxValue += amplitude;
                
                amplitude *= persistence;
                frequency *= 2.0f;
            }
            
            // Normalize and scale the height
            height = (height / maxValue) * heightScale;
            
            // Create the block with the calculated height
            Rectangle3D worldBlock = {
                {x * 1.0f, height, z * 1.0f}, // Position
                5.0f,   // width
                height,   // height (will be scaled in render)
                5.0f    // depth
            };
            
            worldMap[x + GRID_SIZE][z + GRID_SIZE] = worldBlock;
        }
    }
}

// Free allocated memory
void FreeHeightMap()
{
    int size = GRID_SIZE * 2 + 1;

    for (int i = 0; i < size; i++)
    {
        free(worldMap[i]); // Free each row
    }
    free(worldMap); // Free main array
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
void InitBlockDisplayList() {
    blockDisplayList = glGenLists(1);
    glNewList(blockDisplayList, GL_COMPILE);
    
    glBegin(GL_QUADS);
    // Front
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
    // Back
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    // Top
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    // Bottom
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    // Right
    glTexCoord2f(0.0f, 0.0f); glVertex3f( 0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 0.5f,  0.5f, -0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f( 0.5f, -0.5f,  0.5f);
    // Left
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, -0.5f);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-0.5f, -0.5f,  0.5f);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-0.5f,  0.5f,  0.5f);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f,  0.5f, -0.5f);
    glEnd();
    
    glEndList();
}

void RenderWorld() {
    if (blockDisplayList == 0) {
        InitBlockDisplayList();
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, greenBlockTexture);

    const float spacing = 1.0f;
    const int renderDistance = 40; // Reduce render distance for better performance

    // Get player chunk position
    int playerChunkX = (int)playerX;
    int playerChunkZ = (int)playerZ;

    for (int x = -renderDistance; x <= renderDistance; x++) {
        for (int z = -renderDistance; z <= renderDistance; z++) {
            int worldX = x + playerChunkX + GRID_SIZE;
            int worldZ = z + playerChunkZ + GRID_SIZE;

            if (worldX < 0 || worldX >= GRID_SIZE * 2 || worldZ < 0 || worldZ >= GRID_SIZE * 2) {
                continue;
            }

            Rectangle3D* block = &worldMap[worldX][worldZ];
            
            // Simple distance-based culling
            float dx = block->pos.x - playerX;
            float dz = block->pos.z - playerZ;
            if (dx * dx + dz * dz > renderDistance * renderDistance) {
                continue;
            }

            glPushMatrix();
            glTranslatef(block->pos.x, 0, block->pos.z);
            // Scale the height based on the terrain height
            glScalef(block->width, block->height, block->depth);
            glCallList(blockDisplayList);
            glPopMatrix();
        }
    }

    glDisable(GL_TEXTURE_2D);
}
