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

        void RunGame();

    private: 
        void LoadTilemap();
        void LoadPlayer();
        void LoadLevel();

    public:

        std::vector<std::string> textureFilenames = {
            std::string(PROJECT_ROOT) + "/assets/back.png",
            std::string(PROJECT_ROOT) + "/assets/middle.png",
            std::string(PROJECT_ROOT) + "/assets/spritesheets/sprite_still.png",
            std::string(PROJECT_ROOT) + "/assets/tiles/alone_floor.png",
            std::string(PROJECT_ROOT) + "/assets/tiles/coin.png",
            std::string(PROJECT_ROOT) + "/assets/spritesheets/beetle.png",
        };

        enum filenameIdx {back, middle, spriteStill, aloneFloor, coin, beetle};

    private:

        GUI gui;

        EntityManager entityManager;
        SystemManager systemManager = SystemManager(entityManager);

        bool isRunning;
};