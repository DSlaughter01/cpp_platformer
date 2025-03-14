#pragma once
#include "Systems.hpp"

class SystemManager {

    public:
        SystemManager(EntityManager &em) : 
            entityManager(em),
            inputSystem(em),
            movementSystem(em),
            renderSystem(em),
            collisionSystem(em) {}

    private:
        EntityManager &entityManager;
        InputSystem inputSystem;
        MovementSystem movementSystem;
        RenderSystem renderSystem;
        CollisionSystem collisionSystem;

    public:

        void InitQuadTree(int rootWidth, int rootHeight, int maxEntsPerNode) {
            collisionSystem.InitQuadTree(rootWidth, rootHeight, maxEntsPerNode);
        }

        void Update(const Uint8* currentKeyboardState) {

            inputSystem.Update(currentKeyboardState);
            movementSystem.Update();
            collisionSystem.Update(); 
        }

        void Draw(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVec, int &xOffset) {
            renderSystem.Update(ren, textureVec, xOffset);
        }
};