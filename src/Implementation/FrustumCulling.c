#include <math.h>
#include <windows.h>
#include <GL/gl.h>
#include "Transform.h"

struct Plane
{
    float a, b, c, d; // Plane equation: ax + by + cz + d = 0
};

// The six planes of the frustum
struct Plane frustum[6];

// Extract frustum planes from the current projection & modelview matrix
void ExtractFrustum()
{
    float clip[16];
    float proj[16];
    float modl[16];

    // Get the projection & modelview matrix
    glGetFloatv(GL_PROJECTION_MATRIX, proj);
    glGetFloatv(GL_MODELVIEW_MATRIX, modl);

    // Multiply projection & modelview matrices to get the clipping matrix
    clip[0] = modl[0] * proj[0] + modl[1] * proj[4] + modl[2] * proj[8] + modl[3] * proj[12];
    clip[1] = modl[0] * proj[1] + modl[1] * proj[5] + modl[2] * proj[9] + modl[3] * proj[13];
    clip[2] = modl[0] * proj[2] + modl[1] * proj[6] + modl[2] * proj[10] + modl[3] * proj[14];
    clip[3] = modl[0] * proj[3] + modl[1] * proj[7] + modl[2] * proj[11] + modl[3] * proj[15];

    clip[4] = modl[4] * proj[0] + modl[5] * proj[4] + modl[6] * proj[8] + modl[7] * proj[12];
    clip[5] = modl[4] * proj[1] + modl[5] * proj[5] + modl[6] * proj[9] + modl[7] * proj[13];
    clip[6] = modl[4] * proj[2] + modl[5] * proj[6] + modl[6] * proj[10] + modl[7] * proj[14];
    clip[7] = modl[4] * proj[3] + modl[5] * proj[7] + modl[6] * proj[11] + modl[7] * proj[15];

    clip[8] = modl[8] * proj[0] + modl[9] * proj[4] + modl[10] * proj[8] + modl[11] * proj[12];
    clip[9] = modl[8] * proj[1] + modl[9] * proj[5] + modl[10] * proj[9] + modl[11] * proj[13];
    clip[10] = modl[8] * proj[2] + modl[9] * proj[6] + modl[10] * proj[10] + modl[11] * proj[14];
    clip[11] = modl[8] * proj[3] + modl[9] * proj[7] + modl[10] * proj[11] + modl[11] * proj[15];

    clip[12] = modl[12] * proj[0] + modl[13] * proj[4] + modl[14] * proj[8] + modl[15] * proj[12];
    clip[13] = modl[12] * proj[1] + modl[13] * proj[5] + modl[14] * proj[9] + modl[15] * proj[13];
    clip[14] = modl[12] * proj[2] + modl[13] * proj[6] + modl[14] * proj[10] + modl[15] * proj[14];
    clip[15] = modl[12] * proj[3] + modl[13] * proj[7] + modl[14] * proj[11] + modl[15] * proj[15];

    // Extract the planes from the clip matrix
    for (int i = 0; i < 6; i++)
    {
        int j = i * 4;
        frustum[i].a = clip[3] - clip[j + 0];
        frustum[i].b = clip[7] - clip[j + 1];
        frustum[i].c = clip[11] - clip[j + 2];
        frustum[i].d = clip[15] - clip[j + 3];

        // Normalize the plane
        float magnitude = sqrt(frustum[i].a * frustum[i].a + frustum[i].b * frustum[i].b + frustum[i].c * frustum[i].c);
        frustum[i].a /= magnitude;
        frustum[i].b /= magnitude;
        frustum[i].c /= magnitude;
        frustum[i].d /= magnitude;
    }
}

boolean IsInFrustumSphere(const Sphere *sphere)
{
    for (int i = 0; i < 6; i++)
    {
        if (frustum[i].a * sphere->center.x + frustum[i].b * sphere->center.y + frustum[i].c * sphere->center.z + frustum[i].d <= -sphere->radius)
        {
            return FALSE; // Object is outside the frustum
        }
    }
    return TRUE; // Object is visible
}

boolean IsInFrustumRectangle(const Rectangle3D *rect)
{
    // Compute the 8 vertices of the rectangle (cuboid)
    Position vertices[8] = {
        {rect->pos.x, rect->pos.y, rect->pos.z},                                           // 0: Near Bottom Left
        {rect->pos.x + rect->width, rect->pos.y, rect->pos.z},                             // 1: Near Bottom Right
        {rect->pos.x, rect->pos.y + rect->height, rect->pos.z},                            // 2: Near Top Left
        {rect->pos.x + rect->width, rect->pos.y + rect->height, rect->pos.z},              // 3: Near Top Right
        {rect->pos.x, rect->pos.y, rect->pos.z + rect->depth},                             // 4: Far Bottom Left
        {rect->pos.x + rect->width, rect->pos.y, rect->pos.z + rect->depth},               // 5: Far Bottom Right
        {rect->pos.x, rect->pos.y + rect->height, rect->pos.z + rect->depth},              // 6: Far Top Left
        {rect->pos.x + rect->width, rect->pos.y + rect->height, rect->pos.z + rect->depth} // 7: Far Top Right
    };

    // Check each frustum plane
    for (int i = 0; i < 6; i++)
    {
        int outsideCount = 0;
        for (int j = 0; j < 8; j++)
        {
            if (frustum[i].a * vertices[j].x +
                    frustum[i].b * vertices[j].y +
                    frustum[i].c * vertices[j].z +
                    frustum[i].d <
                0)
            {
                outsideCount++;
            }
        }

        // If all 8 vertices are outside one frustum plane, it's not visible
        if (outsideCount == 8)
        {
            return FALSE;
        }
    }

    return TRUE; // At least one vertex is inside
}