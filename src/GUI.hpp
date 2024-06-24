#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <map>
#include <string>
#include <vector> 
#include "SystemManager.hpp"

class GUI {

    public:
        GUI();
        ~GUI();
        void PopulateIDToFilenameMap();
        void LoadTexture(const std::string &filename);
        void LoadTextures();
        void RenderScreen(SystemManager &sm);

    private:

        int windowWidth, windowHeight;

        SDL_Window* window;
        SDL_Renderer* renderer;

        std::map<short int, std::string> IDToFilenameMap;
        std::vector<SDL_Texture*> textureVector;

        SDL_Rect backgroundDest;
};