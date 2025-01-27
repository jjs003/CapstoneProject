/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/


#ifndef RESOURCE_MANAGER_H
#define RESOURCE_MANAGER_H

#include <map>
#include <string>

#include <glad/glad.h>

#include "texture.h"
#include "shader.h"


// The ResourceManager class is a singleton that manages the loading and retrieval
// of resources such as shaders and textures. It provides static functions to 
// load shaders and textures from files, store them for future access, and clean up resources.
class ResourceManager
{
public:
    // Storage for loaded shaders and textures, using string handles for easy access
    static std::map<std::string, Shader>    Shaders;   // Maps shader names to Shader objects
    static std::map<std::string, Texture2D> Textures;  // Maps texture names to Texture2D objects

    // Loads and generates a shader program from vertex, fragment, and optional geometry shader files.
    // If gShaderFile is nullptr, no geometry shader is loaded.
    // Parameters:
    //   - vShaderFile: Path to the vertex shader file
    //   - fShaderFile: Path to the fragment shader file
    //   - gShaderFile: Path to the optional geometry shader file (default: nullptr)
    //   - name: A string name to reference the shader in the resource map
    // Returns: The generated Shader object
    static Shader    LoadShader(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile, std::string name);

    // Retrieves a stored shader by its name.
    // Parameters:
    //   - name: The name of the shader to retrieve
    // Returns: A reference to the Shader object
    static Shader&    GetShader(std::string name);

    // Loads and generates a texture from a file.
    // Parameters:
    //   - file: Path to the texture image file
    //   - alpha: Whether the texture has an alpha channel (transparency)
    //   - name: A string name to reference the texture in the resource map
    // Returns: The generated Texture2D object
    static Texture2D LoadTexture(const char* file, bool alpha, std::string name);

    // Retrieves a stored texture by its name.
    // Parameters:
    //   - name: The name of the texture to retrieve
    // Returns: A reference to the Texture2D object
    static Texture2D& GetTexture(std::string name);

    // Properly de-allocates all loaded resources, clearing the resource maps.
    static void      Clear();

private:
    // Private constructor to prevent instantiation of the ResourceManager class
    ResourceManager() { }
    // Loads and generates a shader from the specified files.
    // Parameters:
    //   - vShaderFile: Path to the vertex shader file
    //   - fShaderFile: Path to the fragment shader file
    //   - gShaderFile: Path to the optional geometry shader file (default: nullptr)
    // Returns: The generated Shader object
    static Shader    loadShaderFromFile(const char* vShaderFile, const char* fShaderFile, const char* gShaderFile = nullptr);

    // Loads a single texture from the specified file.
    // Parameters:
    //   - file: Path to the texture image file
    //   - alpha: Whether the texture has an alpha channel (transparency)
    // Returns: The generated Texture2D object
    static Texture2D loadTextureFromFile(const char* file, bool alpha);
};

#endif

