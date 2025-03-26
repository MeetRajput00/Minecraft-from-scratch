#include "Lightning.h" // Ensure this file exists in the same directory or provide the correct path
#include <gl/GL.h>

void SetupLighting()
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); // Main light source

    // Ambient light (soft, global light to brighten the scene)
    float ambientLight[] = {0.3f, 0.3f, 0.3f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

    // Sunlight settings (diffuse and specular)
    float lightColor[] = {1.0f, 0.9f, 0.6f, 1.0f};
    float lightPos[] = {0.0f, 30.0f, 0.0f, 1.0f}; // High above

    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightColor);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glEnable(GL_COLOR_MATERIAL); // Allows glColor to affect objects
}
void SetupSunLighting()
{
    glEnable(GL_LIGHT1);

    float sunlightDiffuse[] = {1.2f, 1.1f, 0.9f, 1.0f}; // Increase brightness
    glLightfv(GL_LIGHT1, GL_DIFFUSE, sunlightDiffuse);

    float sunlightSpecular[] = {1.2f, 1.1f, 0.9f, 1.0f}; // More reflection
    glLightfv(GL_LIGHT1, GL_SPECULAR, sunlightSpecular);

    float sunlightDirection[] = {0.0f, -1.0f, -0.5f, 0.0f}; // Directional light
    glLightfv(GL_LIGHT1, GL_POSITION, sunlightDirection);
}

void SetupMaterial()
{
    float materialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};  // Reflects light well
    float materialSpecular[] = {0.9f, 0.9f, 0.9f, 1.0f}; // Enhances brightness
    float materialShininess = 50.0f;                     // Higher value = shinier surfaces

    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialShininess);
}