#pragma once
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"

class GUI {

    public:
        GUI();
        ~GUI();
        void RenderScreen();

    private:

        int windowWidth, windowHeight;

        SDL_Window* window;
        SDL_Renderer* renderer;
};