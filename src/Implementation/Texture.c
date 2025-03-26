#include <windows.h>
#include <GL/gl.h>

#include "Texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"

GLuint LoadTexture(const char *filename)
{
    GLuint textureID;
    int width, height, numChannels;

    // Load image file
    unsigned char *data = stbi_load(filename, &width, &height, &numChannels, 0);
    if (!data)
    {
        printf("Failed to load texture: %s\n", filename);
        return 0;
    }

    glGenTextures(1, &textureID);            // Generate texture ID
    glBindTexture(GL_TEXTURE_2D, textureID); // Bind it

    // Determine image format
    GLenum format = (numChannels == 4) ? GL_RGBA : GL_RGB;

    // Upload texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);

    // Set texture parameters (repeat and filter)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data); // Free image memory
    return textureID;      // Return texture ID
}