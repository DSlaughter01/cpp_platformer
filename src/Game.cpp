#include "Game.hpp"
#include <iostream>

Game::Game() :
    isRunning(true) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
        std::cerr << "Problem initialising SDL: " << SDL_GetError() << std::endl;
}


Game::~Game() {
    SDL_Quit();
}


void Game::LoadPlayer() {

    Entity e = entityManager.CreateEntity();
   
    CTransform transform(64, 64, 64, 64);
    entityManager.AddComponent(e, std::make_shared<CTransform>(transform));

    CSpritesheet spritesheet(0);
    entityManager.AddComponent(e, std::make_shared<CSpritesheet>(spritesheet));

    CVelocity velocity(1, 0);
    entityManager.AddComponent(e, std::make_shared<CVelocity>(velocity));  
}


void Game::LoadTilemap() {

    std::fstream tilemapFile;
    std::string currentLine;

    tilemapFile.open("src/level1.txt");
    int line = 0;

    if (tilemapFile.is_open()) {
        
        while (tilemapFile) {

            std::getline(tilemapFile, currentLine);

            for (int i = 0; i < currentLine.size(); i++) {

                if (currentLine[i] != '.') {

                    int x = i * 64;
                    int y = line * 64;

                    Entity e = entityManager.CreateEntity();

                    CTransform transform(x, y, 64, 64);
                    entityManager.AddComponent(e, std::make_shared<CTransform>(transform));

                    CSpritesheet spritesheet(currentLine[i] - '0');
                    entityManager.AddComponent(e, std::make_shared<CSpritesheet>(spritesheet));
                }
            }
            line++;
        }
    }
    tilemapFile.close();

}

void Game::GameLoop() {

    SDL_Event event;
    uint64_t frameStart, frameEnd;
    LoadTilemap();
    LoadPlayer();

    while (isRunning) {

        frameStart = SDL_GetTicks64();

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                isRunning = false;
                break;
            }
        }

        systemManager.Update();
        gui.RenderScreen(systemManager);

        // Control frame rate
        frameEnd = SDL_GetTicks64();
        if (frameEnd - frameStart < desiredFrameTicks)
            SDL_Delay(desiredFrameTicks - (frameEnd - frameStart));
    }
}