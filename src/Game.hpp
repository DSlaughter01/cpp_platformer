#pragma once
#include "EntityManager.hpp"
#include "GUI.hpp"
#include "SystemManager.hpp"
#include <fstream>
#include "Variables.hpp"

class Game {

    public:
        Game();
        ~Game();

        void GameLoop();

    private: 
        void LoadTilemap();
        void LoadPlayer();

    public:

        std::vector<std::string> textureFilenames = {
            "assets/back.png",
            "assets/middle.png",
            "assets/spritesheets/sprite_still.png",
            "assets/tiles/alone_floor.png",
            "assets/tiles/coin.png",
            "assets/spritesheets/beetle.png",
        };

        enum filenameIdx {back, middle, spriteStill, aloneFloor, coin, beetle};

    private:

        GUI gui;

        EntityManager entityManager;
        SystemManager systemManager = SystemManager(entityManager);

        bool isRunning;
};