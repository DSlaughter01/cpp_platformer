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
            std::bitset<World::maxEntities> moveEntities = entityManager.GetMoveEntities();
            std::bitset<World::maxEntities> collisionEntities = entityManager.GetCollisionEntities();

            inputSystem.Update(currentKeyboardState);
            movementSystem.Update(moveEntities);
            collisionSystem.Update();     
        }

        void Draw(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVec) {
            std::bitset<World::maxEntities> renderEntities = entityManager.GetRenderEntities();
            renderSystem.Update(ren, renderEntities, textureVec);
        }
};