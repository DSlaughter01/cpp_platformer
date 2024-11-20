#include "Game.hpp"
#include <iostream>

Game::Game() :
    isRunning(true), gui(textureFilenames) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) 
        std::cerr << "Problem initialising SDL: " << SDL_GetError() << std::endl;
}


Game::~Game() {
    SDL_Quit();
}


void Game::LoadPlayer() {

    Entity e = entityManager.CreateEntity();
    entityManager.SetPlayerEntity(e);

    CTransform transform(Player::width * 2, Player::height , Player::width, Player::height);
    CCollisionState collision(true);
    CSpritesheet spritesheet(filenameIdx::beetle, 4, 144, 39, Direction::Right, Direction::Left, 5);
    CVelocity velocity(true, 0, 0);
    CLanded landed(false);

    entityManager.AddComponent(e, transform);
    entityManager.AddComponent(e, collision);
    entityManager.AddComponent(e, spritesheet);
    entityManager.AddComponent(e, velocity);
    entityManager.AddComponent(e, landed);
}


void Game::LoadTilemap() {

    std::fstream tilemapFile;
    std::string currentLine;

    tilemapFile.open("src/level1.txt");
    int line = 0;
    int levelWidth = 0;
    int levelHeight = 0;

    if (tilemapFile.is_open()) {
        
        while (tilemapFile) {

            std::getline(tilemapFile, currentLine);

            for (int i = 0; i < currentLine.size(); i++) {

                if (currentLine[i] != '.') {

                    int x = i * World::TileDim;
                    int y = line * World::TileDim;

                    if (x > levelWidth)
                        levelWidth = x;

                    if (y > levelHeight)
                        levelHeight = y;

                    Entity e = entityManager.CreateEntity();

                    // Create the tile with Transform, Spritesheet, and Collision components
                    CTransform transform(x, y, World::TileDim, World::TileDim);
                    CCollisionState collision(false);

                    int spritesheetID = currentLine[i] - '0' + 2;
                    
                    // TODO: Add a tag

                    CSpritesheet spritesheet(currentLine[i] - '0' + 2, 1, 18, 18);

                    entityManager.AddComponent<CTransform>(e, transform);
                    entityManager.AddComponent<CCollisionState>(e, collision);
                    entityManager.AddComponent<CSpritesheet>(e, spritesheet);
                }
            }
            line++;
        }
    }
    World::levelWidth = levelWidth;
    gui.SetBackMidRenderTimes(levelWidth);
    tilemapFile.close();
}


void Game::GameLoop() {

    SDL_Event event;
    uint64_t frameStart, frameEnd;

    LoadTilemap();
    LoadPlayer();

    int xOffset = 0;

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
        gui.RenderScreen(systemManager, xOffset);
 
        // Control frame rate
        frameEnd = SDL_GetTicks64();

        // int percentage = 100 * (frameEnd - frameStart) / World::DesiredFrameTicks;
        // std::cout << "This frame took " << percentage << "% of the desired number of ticks" << std::endl;

        if (frameEnd - frameStart < World::DesiredFrameTicks) {
            SDL_Delay(World::DesiredFrameTicks - (frameEnd - frameStart));
        }
    }
}