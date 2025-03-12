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
    entityManager.SetPlayerEntity(e);

    CTransform transform(Player::width * 2, Player::height , Player::width, Player::height);
    CCollisionState collision(true, true);
    CSpritesheet spritesheet(filenameIdx::beetle, 4, 144, 39, Direction::Right, Direction::Left, 5);
    CVelocity velocity(0, 0);
    CLanded landed(false);

    entityManager.AddComponent(e, transform);
    entityManager.AddComponent(e, collision);
    entityManager.AddComponent(e, spritesheet);
    entityManager.AddComponent(e, velocity);
    entityManager.AddComponent(e, landed);
}


void Game::LoadTilemap() {

    std::string levelFilePath =  std::string(PROJECT_ROOT) + "src/level2.txt";
    std::fstream tilemapFile;
    std::string currentLine;

    tilemapFile.open("../src/level1.txt");
    int line = 0;
    int levelWidth = 0;
    int x = 0;
    int y = 0;

    if (tilemapFile.is_open()) {
        
        while (std::getline(tilemapFile, currentLine)) {

            for (int i = 0; i < currentLine.size(); i++) {

                if (currentLine[i] != '.') {

                    x = i * World::TileDim;
                    levelWidth = (x > levelWidth) ? x : levelWidth;

                    Entity e = entityManager.CreateEntity();

                    // Create the tile with Transform, Spritesheet, and Collision components
                    CTransform transform(x, y, World::TileDim, World::TileDim);
                    CCollisionState collision(false, true);

                    int spritesheetID = currentLine[i] - '0' + 2;
                    
                    CSpritesheet spritesheet(currentLine[i] - '0' + 2, 1, 18, 18);

                    entityManager.AddComponent(e, transform);
                    entityManager.AddComponent(e, collision);
                    entityManager.AddComponent(e, spritesheet);
                }
            }
            y += World::TileDim;
            line++;
        }
    }
    
    else {
        std::cerr << "Error in Game::LoadTilemap(): Could not open file. Please check filename." << std::endl;
        return;
    }

    World::levelWidth = x + levelWidth;
    World::levelHeight = y + World::TileDim;

    tilemapFile.close();
}


void Game::LoadLevel() {

    LoadTilemap();
    LoadPlayer();

    systemManager.InitQuadTree(World::levelWidth, World::levelHeight, 4);
    gui.LoadLevel(textureFilenames, World::levelWidth);
}


void Game::RunGame() {

    SDL_Event event;
    uint64_t frameStart, frameEnd;

    LoadLevel();

    int tranX = 0;
    int tranY = 0;

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