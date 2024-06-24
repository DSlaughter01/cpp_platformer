#pragma once
#include "Systems.hpp"

class SystemManager {
    public:
        SystemManager(EntityManager &em) : entityManager(em) {}

    private:
        EntityManager &entityManager;
        MovementSystem movementSystem = MovementSystem(entityManager);
        RenderSystem renderSystem = RenderSystem(entityManager);

    public:
        void Update() {
            movementSystem.Update();
        }

        void Draw(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVector) {
            renderSystem.Update(ren, textureVector);
        }
};