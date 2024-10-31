#include "GUI.hpp"
#include <iostream>

GUI::GUI(std::vector<std::string> &textureFilenames) :
    windowWidth(World::WindowWidth), windowHeight(World::WindowHeight) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Problem initialising SDL: " << SDL_GetError() << std::endl;
        return;
    }

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        std::cerr << "Problem initialising SDL_Image: " << IMG_GetError() << std::endl;
        return;
    }

    window = SDL_CreateWindow("Platformer", 
                    SDL_WINDOWPOS_CENTERED, 
                    SDL_WINDOWPOS_CENTERED,
                    windowWidth,
                    windowHeight,
                    0);

    renderer = SDL_CreateRenderer(window, -1, 0); 

    if (!window) {
        std::cerr << "Problem creating window: " << SDL_GetError() << std::endl;
        return;
    }

    if (!renderer) {
        std::cerr << "Problem creating renderer: " << SDL_GetError() << std::endl;
        return;
    }

    m_textureFilenames = textureFilenames;

    LoadTextures();
    backgroundDest = {0, 0, windowWidth, windowHeight};
}


GUI::~GUI() {

    for (auto &i: textureVector) {
        SDL_DestroyTexture(i);
        i = nullptr;
    }

    SDL_DestroyTexture(backgroundTex);
    backgroundTex = nullptr;

    SDL_DestroyTexture(middleTex);
    middleTex = nullptr;

    SDL_DestroyWindow(window);
    window = nullptr;

    SDL_DestroyRenderer(renderer);
    renderer = nullptr;

    IMG_Quit();
    SDL_Quit();
}


void GUI::LoadTextures() {

    textureVector.reserve(m_textureFilenames.size());

    for (int i = 0; i < m_textureFilenames.size(); i++) {            
        
        SDL_Surface* imgSurf = IMG_Load(m_textureFilenames[i].c_str());
        SDL_Texture* imgTex = SDL_CreateTextureFromSurface(renderer, imgSurf);
        SDL_FreeSurface(imgSurf);

        textureVector[i] = imgTex;
    }

    backgroundTex = textureVector[backImgIdx];
    middleTex = textureVector[middleImgIdx];
}


void GUI::RenderScreen(SystemManager &systemManager) {

    SDL_RenderClear(renderer);
    
    SDL_RenderCopy(renderer, backgroundTex, NULL, &backgroundDest);
    SDL_RenderCopy(renderer, middleTex, NULL, &backgroundDest);
    systemManager.Draw(renderer, textureVector);
    SDL_RenderPresent(renderer);
}