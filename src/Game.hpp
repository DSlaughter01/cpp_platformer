#pragma once
#include "EntityManager.hpp"
#include "GUI.hpp"
#include "SystemManager.hpp"

class Game {

    public:
        Game();
        ~Game();

        void GameLoop();
        void CreateEntity();

    private:

    public:

    private:
        GUI gui;
        EntityManager entityManager;
        SystemManager systemManager = SystemManager(entityManager);

        bool isRunning;

};