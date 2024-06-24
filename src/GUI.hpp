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
        void LoadBackAndMiddle();
        void PopulateIDToFilenameMap();

        void LoadEntityTextures();
        void RenderScreen(SystemManager &systemManager);

    private:

        int windowWidth, windowHeight;

        SDL_Window* window;
        SDL_Renderer* renderer;

        SDL_Texture* backgroundTex;
        SDL_Texture* middleTex;

        SDL_Rect backgroundDest;

        std::map<short int, std::string> IDToFilenameMap;
        std::vector<SDL_Texture*> entityTextureVector;
};