#pragma once
#include "Systems.hpp"

class SystemManager {
    public:
        SystemManager(EntityManager &em) : entityManager(em) {}

    private:
        EntityManager &entityManager;
        EventSystem eventSystem = EventSystem(entityManager);
        MovementSystem movementSystem = MovementSystem(entityManager);
        RenderSystem renderSystem = RenderSystem(entityManager);

    public:
        void Update(const Uint8* currentKeyboardState) {
            eventSystem.Update(currentKeyboardState);
            movementSystem.Update(entityManager.moveEntities);
        }

        void Draw(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVector) {
            renderSystem.Update(ren, entityManager.renderEntities, textureVector);
        }
};