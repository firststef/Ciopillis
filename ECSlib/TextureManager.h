#pragma once
#include <raylib.h>
#include <vector>

class TextureManager
{
    std::vector<Texture2D> textures;
public:
    Texture2D Load(const char* path)
    {
        Texture2D texture = LoadTexture(path);
        textures.push_back(texture);
        return texture;
    }

    ~TextureManager()
    {
        for (auto tex : textures)
        {
            UnloadTexture(tex);
        }
    }
};
