#include "GUI.hpp"
#include <iostream>

GUI::GUI() :
    windowWidth(800), windowHeight(600) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Problem initialising SDL: " << SDL_GetError() << std::endl;
    }

    if (IMG_Init(IMG_INIT_JPG) < 0) {
        std::cerr << "Problem initialising SDL_Image: " << IMG_GetError() << std::endl;
    }

    window = SDL_CreateWindow("Platformer", 
                    SDL_WINDOWPOS_CENTERED, 
                    SDL_WINDOWPOS_CENTERED,
                    windowWidth,
                    windowHeight,
                    0);

    renderer = SDL_CreateRenderer(window, -1, 0); 
}


GUI::~GUI() {

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
    IMG_Quit();
}


void GUI::RenderScreen() {

    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
}