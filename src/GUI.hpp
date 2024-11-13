#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include <map>
#include <string>
#include <vector> 
#include "SystemManager.hpp"
#include "Variables.hpp"

class GUI {

    public:

        GUI() = default;
        GUI(std::vector<std::string> &textureFilenames);
        ~GUI();
        void LoadTextures();
        void RenderScreen(SystemManager &systemManager, int &xOffset);
        void RenderBackground(int &xOffset);
        int GetWindowWidth() {return windowWidth;}
        void SetBackMidRenderTimes(int levelWidth);

    private:


    private:

        const int backImgIdx = 0;
        const int middleImgIdx = 1;

        int windowWidth;
        int windowHeight;

        SDL_Window* window;
        SDL_Renderer* renderer;

        SDL_Texture* backgroundTex;
        SDL_Texture* middleTex;

        SDL_Rect backgroundDest;
        SDL_Rect middleDest;

        // Number of times the background needs to be rendered in order to cover the level
        int backRenderTimes;
        int midRenderTimes;

        std::vector<std::string> m_textureFilenames;
        std::vector<SDL_Texture*> textureVector;
};