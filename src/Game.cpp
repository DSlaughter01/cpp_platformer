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


void Game::CreateEntity() {

    int x = 0;
    int y = 0;

    for (int i = 0; i < MAX_ENTITIES; i++) {

        Entity e = entityManager.CreateEntity();

        CTransform transform(x, y, 64, 64);
        entityManager.AddComponent(e, std::make_shared<CTransform>(transform));

        CSpritesheet spritesheet(0);
        entityManager.AddComponent(e, std::make_shared<CSpritesheet>(spritesheet));

        CTag tag;
        entityManager.AddComponent(e, std::make_shared<CTag>(tag));

        CDimensions dimensions(64, 64);
        entityManager.AddComponent(e, std::make_shared<CDimensions>(dimensions));

        CVelocity velocity(1, 1);
        entityManager.AddComponent(e, std::make_shared<CVelocity>(velocity));

        x += 10;
        if (x > 900) {
            y += 10;
            x = 0;
        }
        
    }


}

void Game::GameLoop() {

    SDL_Event event;

    CreateEntity();

    while (isRunning) {

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {
                isRunning = false;
                break;
            }
        }

        systemManager.Update();
        gui.RenderScreen(systemManager);
    }
}