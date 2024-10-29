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
        void Update(const Uint8* currentKeyboardState) {

            inputSystem.Update(currentKeyboardState);
            movementSystem.Update();
            collisionSystem.Update();     
        }

        void Draw(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVec) {

            renderSystem.Update(ren, textureVec);
        }
};