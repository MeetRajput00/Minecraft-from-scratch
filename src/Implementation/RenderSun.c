#include <gl/GLU.h>
#include "RenderSun.h"
#include <math.h>

void RenderSun(float sunAngle) {
    glPushMatrix();
    
    float sunDistance = 20.0f;  // Move sun further away
    float sunHeight = 40.0f;    // Increase height
    float sunX = cos(sunAngle) * sunDistance;
    float sunZ = sin(sunAngle) * sunDistance;

    glTranslatef(sunX, sunHeight, sunZ);

    // Ensure lighting does not affect the sun color
    
    glColor3f(1.0f, 0.95f, 0.5f); // Bright warm yellow
    
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 3.0f, 50, 50); // Increase size for better visibility
    gluDeleteQuadric(quad);


    glPopMatrix();
}


void UpdateSun() {
    sunRotateAngle += 0.1f; // Increase slowly for smooth movement
    if (sunRotateAngle >= 360.0f) sunRotateAngle = 0.0f; // Loop back
}
