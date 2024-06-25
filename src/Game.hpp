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

    private:
        GUI gui;
        EntityManager entityManager;
        SystemManager systemManager = SystemManager(entityManager);

        bool isRunning;
};