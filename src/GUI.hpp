#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <map>
#include <string>


class GUI {

    public:
        GUI();
        ~GUI();
        void LoadTexture(std::string filename);
        void LoadTextures();
        void RenderEntities();
        void RenderScreen();

    private:

        int windowWidth, windowHeight;

        SDL_Window* window;
        SDL_Renderer* renderer;

        std::map<std::string, SDL_Texture*> filenameToTextureMap;

        SDL_Rect backgroundDest;
};