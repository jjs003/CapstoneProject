/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#include <iostream>
#include "texture.h"

// Constructor that initializes default values for the texture object.
Texture2D::Texture2D()
    : Width(0), Height(0), Internal_Format(GL_RGB), Image_Format(GL_RGB), Wrap_S(GL_REPEAT), Wrap_T(GL_REPEAT), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
    // Generate texture object in OpenGL
    glGenTextures(1, &this->ID);
}

// Generates a texture from image data and sets texture parameters.
void Texture2D::Generate(unsigned int width, unsigned int height, unsigned char* data)
{
    this->Width = width;
    this->Height = height;

    // Bind the texture for subsequent configuration
    glBindTexture(GL_TEXTURE_2D, this->ID);

    // Create the texture object in OpenGL and load the image data
    glTexImage2D(GL_TEXTURE_2D, 0, this->Internal_Format, width, height, 0, this->Image_Format, GL_UNSIGNED_BYTE, data);

    // Set the texture wrap modes for both axes (horizontal and vertical)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);

    // Set the texture filtering modes for minification and magnification
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);

    // Unbind the texture to prevent further modifications
    glBindTexture(GL_TEXTURE_2D, 0);
}

// Binds the texture object to the current OpenGL context for use in rendering.
void Texture2D::Bind() const
{
    // Bind the texture using its OpenGL ID
    glBindTexture(GL_TEXTURE_2D, this->ID);
}

