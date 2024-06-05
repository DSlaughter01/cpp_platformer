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

    LoadTextures();

    backgroundDest = {0, 0, windowWidth, windowHeight};
}


GUI::~GUI() {

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
    IMG_Quit();
}


void GUI::LoadTexture(std::string filename) {

    if (filenameToTextureMap[filename]) {
        std::cout << filename << " already loaded" << std::endl;
        return;
    }

    SDL_Surface* imgSurf = IMG_Load(filename.c_str());
    SDL_Texture* imgTex = SDL_CreateTextureFromSurface(renderer, imgSurf);
    SDL_FreeSurface(imgSurf);
    
    filenameToTextureMap[filename] = imgTex;
}


void GUI::LoadTextures() {

    LoadTexture("assets/back.png");
    LoadTexture("assets/middle.png");
}


void GUI::RenderEntities() {

    SDL_RenderCopy(renderer, filenameToTextureMap["assets/back.png"], NULL, &backgroundDest);
    SDL_RenderCopy(renderer, filenameToTextureMap["assets/middle.png"], NULL, &backgroundDest);
}


void GUI::RenderScreen() {

    SDL_RenderClear(renderer);
    RenderEntities();
    SDL_RenderPresent(renderer);
}