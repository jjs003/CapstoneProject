/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>

// Texture2D class is responsible for managing OpenGL textures.
// It stores texture data and provides functions for generating,
// binding, and configuring textures for use in rendering.
class Texture2D
{
public:
    // Texture properties
    unsigned int ID;              // OpenGL ID for the texture object
    unsigned int Width, Height;   // Dimensions of the loaded texture (in pixels)

    // Format of the texture
    unsigned int Internal_Format; // Internal format of the texture object
    unsigned int Image_Format;    // Format of the loaded texture image

    // Texture configuration
    unsigned int Wrap_S;          // Wrapping mode along the S (horizontal) axis
    unsigned int Wrap_T;          // Wrapping mode along the T (vertical) axis
    unsigned int Filter_Min;      // Filtering mode when texture is minified
    unsigned int Filter_Max;      // Filtering mode when texture is magnified

    // Constructor that sets default texture configuration
    Texture2D();

    // Generates a texture from the given image data
    // Parameters:
    // - width: The width of the texture in pixels
    // - height: The height of the texture in pixels
    // - data: Pointer to the texture's image data
    void Generate(unsigned int width, unsigned int height, unsigned char* data);

    // Binds the texture as the currently active GL_TEXTURE_2D object
    void Bind() const;
};

#endif

