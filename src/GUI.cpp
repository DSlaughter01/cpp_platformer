#include "GUI.hpp"
#include <iostream>

GUI::GUI() :
    windowWidth(900), windowHeight(640) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Problem initialising SDL: " << SDL_GetError() << std::endl;
    }

    if (IMG_Init(IMG_INIT_PNG) < 0) {
        std::cerr << "Problem initialising SDL_Image: " << IMG_GetError() << std::endl;
    }

    window = SDL_CreateWindow("Platformer", 
                    SDL_WINDOWPOS_CENTERED, 
                    SDL_WINDOWPOS_CENTERED,
                    windowWidth,
                    windowHeight,
                    0);

    renderer = SDL_CreateRenderer(window, -1, 0); 

    PopulateIDToFilenameMap();
    LoadTextures();

    backgroundDest = {0, 0, windowWidth, windowHeight};
}


GUI::~GUI() {

    for (auto &i: textureVector) {
        SDL_DestroyTexture(i);
    }

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
    IMG_Quit();
}

void GUI::PopulateIDToFilenameMap() {

    IDToFilenameMap.clear();

    std::vector<std::string> textureFilenames = {
        "assets/tiles/alone_floor.png",
        "assets/spritesheets/sprite_still.png",
        "assets/back.png",
        "assets/middle.png",
        "assets/tiles/tiles.png"
    };

    for (int i = 0; i < textureFilenames.size(); i++) {
        IDToFilenameMap[i] = textureFilenames[i];
    }
}

void GUI::LoadTexture(const std::string &filename) {

    SDL_Surface* imgSurf = IMG_Load(filename.c_str());
    SDL_Texture* imgTex = SDL_CreateTextureFromSurface(renderer, imgSurf);
    SDL_FreeSurface(imgSurf);

    textureVector.emplace_back(imgTex);
}


void GUI::LoadTextures() {

    for (auto &file : IDToFilenameMap) {
        LoadTexture(file.second);
    }
}



void GUI::RenderScreen(SystemManager &sm) {

    SDL_RenderClear(renderer);
    sm.Draw(renderer, textureVector);
    SDL_RenderPresent(renderer);
}