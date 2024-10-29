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

    // Create a player with simple components
    CTransform transform(64 * 2, 64 + 10, 64, 64);
    CCollisionState collision;
    CSpritesheet spritesheet(filenameIdx::spriteStill, 1, 24, 24, Direction::Right, 2);
    CVelocity velocity(true, -3, 0);
    CLanded landed(false);

    entityManager.AddComponent(e, transform);
    entityManager.AddComponent(e, collision);
    entityManager.AddComponent(e, spritesheet);
    entityManager.AddComponent(e, velocity);
    entityManager.AddComponent(e, landed);

    entityManager.SetPlayerEntity(e);
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

                    // Create the tile with Transform, Spritesheet, and Collision components
                    CTransform transform(x, y, World::tileDim, World::tileDim);
                    CCollisionState collision;
                    CSpritesheet spritesheet(currentLine[i] - '0' + 2, 1, 18, 18);
                    CVelocity velocity(false, 0, 0);

                    entityManager.AddComponent<CTransform>(e, transform);
                    entityManager.AddComponent<CCollisionState>(e, collision);
                    entityManager.AddComponent<CSpritesheet>(e, spritesheet);
                    entityManager.AddComponent<CVelocity>(e, velocity);
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

    const Uint8 *currentKeyboardState = SDL_GetKeyboardState(NULL);

    while (isRunning) {

        frameStart = SDL_GetTicks64();

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                isRunning = false;
                break;
            }
        }

        systemManager.Update(currentKeyboardState);
        gui.RenderScreen(systemManager);

        // Control frame rate
        frameEnd = SDL_GetTicks64();
        if (frameEnd - frameStart < World::desiredFrameTicks)
            SDL_Delay(World::desiredFrameTicks - (frameEnd - frameStart));
    }
}