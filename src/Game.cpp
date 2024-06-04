#include "Game.hpp"
#include <iostream>

Game::Game() :
    isRunning(true) {

    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        std::cerr << "Problem initialising SDL: " << SDL_GetError() << std::endl;
    }
}


Game::~Game() {
    SDL_Quit();
}

void Game::GameLoop() {

    SDL_Event event;

    while (isRunning) {

        while (SDL_WaitEvent(&event)) {

            if (event.type == SDL_QUIT) {
                isRunning = false;
                break;
            }
        }

        gui.RenderScreen();
    }
}