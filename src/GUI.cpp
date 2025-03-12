#include "GUI.hpp"
#include <iostream>

GUI::GUI() :
    windowWidth(World::WindowWidth), windowHeight(World::WindowHeight),
    backRenderTimes(0), midRenderTimes(0) {

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

    backgroundDest = {0, 0, windowWidth, windowHeight};
    middleDest = {0, 0, windowWidth, windowHeight};
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


void GUI::LoadLevel(const std::vector<std::string> &textureFilenames, int levelWidth) {

    m_textureFilenames = textureFilenames;

    backRenderTimes = std::ceil((float) levelWidth / (float) backgroundDest.w);
    midRenderTimes = std::ceil((float) levelWidth / (float) middleDest.w);

    textureVector.clear();
    textureVector.reserve(m_textureFilenames.size());

    for (int i = 0; i < m_textureFilenames.size(); i++) {            
        
        SDL_Surface* imgSurf = IMG_Load(m_textureFilenames.at(i).c_str());
        SDL_Texture* imgTex = SDL_CreateTextureFromSurface(renderer, imgSurf);

        SDL_FreeSurface(imgSurf);

        textureVector[i] = imgTex;
    }

    backgroundTex = textureVector[backImgIdx];
    middleTex = textureVector[middleImgIdx];
}


void GUI::RenderBackground(int &xOffset) {

    for (int i = 0; i < backRenderTimes; i++) {    
        backgroundDest.x - xOffset + i * backgroundDest.w;
        SDL_RenderCopy(renderer, backgroundTex, NULL, &backgroundDest);
    }
    
    for (int i = 0; i < midRenderTimes; i++) {
        middleDest.x - xOffset + i * middleDest.w;
        SDL_RenderCopy(renderer, middleTex, NULL, &middleDest);
    }
}


void GUI::RenderScreen(SystemManager &systemManager, int &xOffset) {

    SDL_RenderClear(renderer);
    RenderBackground(xOffset);
    systemManager.Draw(renderer, textureVector, xOffset);
    SDL_RenderPresent(renderer);
}