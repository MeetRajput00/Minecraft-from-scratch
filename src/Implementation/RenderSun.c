#include <gl/GLU.h>
#include "RenderSun.h"
#include <math.h>
#include "FrustumCulling.h"
#include "Transform.h"

Sphere Sun;

void RenderSun(float sunAngle)
{

    float distance = 20.0f; // Move sun further away
    InitSphere(&Sun, cos(sunAngle) * distance, 40.0f, sin(sunAngle) * distance, 3.0f);

    if (IsInFrustumSphere(&Sun))
    {

        glPushMatrix();

        glTranslatef(Sun.center.x, Sun.center.y, Sun.center.z);

        // Ensure lighting does not affect the sun color

        glColor3f(1.0f, 0.95f, 0.5f); // Bright warm yellow

        GLUquadric *quad = gluNewQuadric();
        gluSphere(quad, 3.0f, 50, 50); // Increase size for better visibility
        gluDeleteQuadric(quad);

        glPopMatrix();
    }
}

void UpdateSun()
{
    sunRotateAngle += 0.1f; // Increase slowly for smooth movement
    if (sunRotateAngle >= 360.0f)
        sunRotateAngle = 0.0f; // Loop back
}
