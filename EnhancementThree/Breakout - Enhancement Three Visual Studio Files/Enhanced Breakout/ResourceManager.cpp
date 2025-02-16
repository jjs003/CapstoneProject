/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/
#include "resource_manager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include "stb_image.h"

// Instantiate static variables for storing shaders and textures
std::map<std::string, Texture2D>    ResourceManager::Textures;   // Map to store textures by name
std::map<std::string, Shader>       ResourceManager::Shaders;    // Map to store shaders by name

// Loads and generates a shader program from vertex, fragment, and optional geometry shader files.
// Stores the generated shader in the Shaders map for future access.
Shader ResourceManager::LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name)
{
    // Load and compile the shader, then store it in the map with the given nam
    Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
    return Shaders[name];
}

// Retrieves a stored shader by its name.
Shader& ResourceManager::GetShader(std::string name)
{
    return Shaders[name];
}


// Loads and generates a texture from a file. Stores the generated texture in the Textures map.
Texture2D ResourceManager::LoadTexture(const char* file, bool alpha, std::string name)
{
    // Load the texture and store it in the map with the given name
    Textures[name] = loadTextureFromFile(file, alpha);
    return Textures[name];
}

// Retrieves a stored texture by its name.
Texture2D& ResourceManager::GetTexture(std::string name)
{
    return Textures[name];
}

// Deallocates all loaded resources, including shaders and textures.
void ResourceManager::Clear()
{
    // Properly delete all shaders
    for (auto iter : Shaders)
        glDeleteProgram(iter.second.ID);
    // Properly delete all textures
    for (auto iter : Textures)
        glDeleteTextures(1, &iter.second.ID);
}

// Helper function to load and compile a shader from file. Optionally loads a geometry shader.
Shader ResourceManager::loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile)
{
    // Retrieve the vertex and fragment shader source code from the specified files
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;
    try
    {
        // Open vertex and fragment shader files
        std::ifstream vertexShaderFile(vShaderFile);
        std::ifstream fragmentShaderFile(fShaderFile);
        std::stringstream vShaderStream, fShaderStream;

        // Read file contents into streams
        vShaderStream << vertexShaderFile.rdbuf();
        fShaderStream << fragmentShaderFile.rdbuf();

        // Close file handlers
        vertexShaderFile.close();
        fragmentShaderFile.close();

        // Convert streams into strings
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

        // If a geometry shader file is provided, load it as well
        if (gShaderFile != nullptr)
        {
            std::ifstream geometryShaderFile(gShaderFile);
            std::stringstream gShaderStream;
            gShaderStream << geometryShaderFile.rdbuf();
            geometryShaderFile.close();
            geometryCode = gShaderStream.str();
        }
    }
    catch (std::exception e)
    {
        std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
    }

    // Convert string code into const char pointers for OpenGL
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    const char* gShaderCode = geometryCode.c_str();

    // Create the shader object and compile it
    Shader shader;
    shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
    return shader;
}

// Helper function to load a texture from a file and generate it.
Texture2D ResourceManager::loadTextureFromFile(const char* file, bool alpha)
{
    // Create a texture object
    Texture2D texture;

    // If the texture has an alpha channel (transparency), set the format accordingly
    if (alpha)
    {
        texture.Internal_Format = GL_RGBA;
        texture.Image_Format = GL_RGBA;
    }

    // Load the image data using stb_image
    int width, height, nrChannels;
    unsigned char* data = stbi_load(file, &width, &height, &nrChannels, 0);

    // Generate the texture using the loaded image data
    texture.Generate(width, height, data);

    // Free the image data after the texture is generated
    stbi_image_free(data);

    return texture;
}

