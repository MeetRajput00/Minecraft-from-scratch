#include "Transform.h"

void InitSphere(Sphere *sphere, float x, float y, float z, float radius)
{
    sphere->center.x = x;
    sphere->center.y = y;
    sphere->center.z = z;
    sphere->radius = radius;
}

void InitRectangle3D(Rectangle3D *rect, float x, float y, float z, float height, float width, float depth)
{
    rect->pos.x = x;
    rect->pos.y = y;
    rect->pos.z = z;
    rect->height = height;
    rect->width = width;
    rect->depth = depth;
}

// Move position by a delta
void MovePosition(Position *pos, float dx, float dy, float dz)
{
    pos->x += dx;
    pos->y += dy;
    pos->z += dz;
}

// Scale the transform uniformly
void ScaleTransform(Transform *transform, float scaleFactor)
{
    transform->scale *= scaleFactor;
}

// Rotate a position around the origin (basic rotation)
void RotatePosition(Position *pos, float angleX, float angleY, float angleZ)
{
    float cosX = cos(angleX), sinX = sin(angleX);
    float cosY = cos(angleY), sinY = sin(angleY);
    float cosZ = cos(angleZ), sinZ = sin(angleZ);

    // Rotate around X-axis
    float y = pos->y * cosX - pos->z * sinX;
    float z = pos->y * sinX + pos->z * cosX;
    pos->y = y;
    pos->z = z;

    // Rotate around Y-axis
    float x = pos->x * cosY + pos->z * sinY;
    z = -pos->x * sinY + pos->z * cosY;
    pos->x = x;
    pos->z = z;

    // Rotate around Z-axis
    x = pos->x * cosZ - pos->y * sinZ;
    y = pos->x * sinZ + pos->y * cosZ;
    pos->x = x;
    pos->y = y;
}

// Check if a point is inside a sphere
int IsPointInsideSphere(const Sphere *sphere, const Position *point)
{
    float dx = point->x - sphere->center.x;
    float dy = point->y - sphere->center.y;
    float dz = point->z - sphere->center.z;
    return (dx * dx + dy * dy + dz * dz) <= (sphere->radius * sphere->radius);
}

// Check if a point is inside a 3D rectangle (cuboid)
int IsPointInsideRectangle3D(const Rectangle3D *rect, const Position *point)
{
    return (point->x >= rect->pos.x && point->x <= rect->pos.x + rect->width) &&
           (point->y >= rect->pos.y && point->y <= rect->pos.y + rect->height) &&
           (point->z >= rect->pos.z && point->z <= rect->pos.z + rect->depth);
}