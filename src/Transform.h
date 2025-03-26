#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <math.h>

// Position struct for 3D coordinates
typedef struct
{
    float x, y, z;
} Position;

// Transform struct for generic transformations
typedef struct
{
    Position pos; // Position in 3D space
    float scale;  // Uniform scale factor
} Transform;

// Sphere struct
typedef struct
{
    Position center; // Center position
    float radius;    // Radius of the sphere
} Sphere;

// Rectangle (Cuboid) struct
typedef struct
{
    Position pos; // Bottom-left corner position
    float width;  // Width along the X-axis
    float height; // Height along the Y-axis
    float depth;  // Depth along the Z-axis
} Rectangle3D;

// Function prototypes
void InitSphere(Sphere *sphere, float x, float y, float z, float radius);
void InitRectangle3D(Rectangle3D *rect, float x, float y, float z, float height, float width, float depth);
void MovePosition(Position *pos, float dx, float dy, float dz);
void ScaleTransform(Transform *transform, float scaleFactor);
void RotatePosition(Position *pos, float angleX, float angleY, float angleZ);
int IsPointInsideSphere(const Sphere *sphere, const Position *point);
int IsPointInsideRectangle3D(const Rectangle3D *rect, const Position *point);

#endif