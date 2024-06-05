#pragma once
#include "GUI.hpp"

class Game {

    public:
        Game();
        ~Game();

        void GameLoop();
    private:

    public:

    private:
        GUI gui;

        bool isRunning;

};