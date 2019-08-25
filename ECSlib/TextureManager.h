#pragma once
#include <raylib.h>
#include <map>

class TextureManager
{
    std::map<std::string, std::pair<Texture2D, unsigned>> textures;
public:
    Texture2D Load(const char* path)
    {
        const std::string p(path);
        auto it = textures.find(p);
        if (it == textures.end()) {
            Texture2D texture = LoadTexture(path);
            textures.insert(std::pair<std::string, std::pair<Texture2D, unsigned>>(p, std::pair<Texture2D, unsigned>(texture, 1)));
            return texture;
        }
        else
        {
            it->second.second++;
            return it->second.first;
        }
    }

    void Unload(Texture2D texture)
    {
        auto it = textures.begin();
        for (; it != textures.end(); ++it)
        {
            if (it->second.first == texture)
                break;
        }

        if (it != textures.end()) {
            if (it->second.second <= 1)
            {
                UnloadTexture(it->second.first);
                textures.erase(it);
            }
            else
            {
                it->second.second--;
            }
        }
    }

    ~TextureManager()
    {
        for (auto tex : textures)
        {
            UnloadTexture(tex.second.first);
        }
    }
};
