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
    CTransform transform(Player::width * 2, Player::height , Player::width, Player::height);
    CCollisionState collision;
    CSpritesheet spritesheet(filenameIdx::beetle, 4, 144, 39, Direction::Right, 5);
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

                    int x = i * World::TileDim;
                    int y = line * World::TileDim;

                    Entity e = entityManager.CreateEntity();

                    // Create the tile with Transform, Spritesheet, and Collision components
                    CTransform transform(x, y, World::TileDim, World::TileDim);
                    CCollisionState collision;
                    CSpritesheet spritesheet(currentLine[i] - '0' + 2, 1, 18, 18);

                    entityManager.AddComponent<CTransform>(e, transform);
                    entityManager.AddComponent<CCollisionState>(e, collision);
                    entityManager.AddComponent<CSpritesheet>(e, spritesheet);
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
        if (frameEnd - frameStart < World::DesiredFrameTicks)
            SDL_Delay(World::DesiredFrameTicks - (frameEnd - frameStart));
    }
}