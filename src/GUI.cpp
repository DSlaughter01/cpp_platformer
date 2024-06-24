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

    LoadBackAndMiddle();
    PopulateIDToFilenameMap();
    LoadEntityTextures();

    backgroundDest = {0, 0, windowWidth, windowHeight};
}


GUI::~GUI() {

    for (auto &i: entityTextureVector) {
        SDL_DestroyTexture(i);
    }

    SDL_DestroyTexture(backgroundTex);
    SDL_DestroyTexture(middleTex);

    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);

    SDL_Quit();
    IMG_Quit();
}


void GUI::LoadBackAndMiddle() {

    SDL_Surface* backSurf = IMG_Load("assets/back.png");
    backgroundTex = SDL_CreateTextureFromSurface(renderer, backSurf);
    SDL_FreeSurface(backSurf);

    SDL_Surface* midSurf = IMG_Load("assets/middle.png");
    middleTex = SDL_CreateTextureFromSurface(renderer, midSurf);
    SDL_FreeSurface(midSurf);
}

void GUI::PopulateIDToFilenameMap() {

    IDToFilenameMap.clear();

    std::vector<std::string> textureFilenames = {
        "assets/spritesheets/sprite_still.png",
        "assets/tiles/alone_floor.png",
        "assets/tiles/tiles.png"
    };

    for (int i = 0; i < textureFilenames.size(); i++)
        IDToFilenameMap[i] = textureFilenames[i]; 
}


void GUI::LoadEntityTextures() {

    for (auto &file : IDToFilenameMap) {
        
        SDL_Surface* imgSurf = IMG_Load(file.second.c_str());
        SDL_Texture* imgTex = SDL_CreateTextureFromSurface(renderer, imgSurf);
        SDL_FreeSurface(imgSurf);

        entityTextureVector.emplace_back(imgTex);
    }
}


void GUI::RenderScreen(SystemManager &systemManager) {

    SDL_RenderClear(renderer);
    
    SDL_RenderCopy(renderer, backgroundTex, NULL, &backgroundDest);
    SDL_RenderCopy(renderer, middleTex, NULL, &backgroundDest);
    systemManager.Draw(renderer, entityTextureVector);
    SDL_RenderPresent(renderer);
}