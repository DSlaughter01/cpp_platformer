#pragma once
#include "Systems.hpp"

class SystemManager {
    public:
        SystemManager(EntityManager &em) : entityManager(em) {}

    private:
        EntityManager &entityManager;
        InputSystem inputSystem = InputSystem(entityManager);
        MovementSystem movementSystem = MovementSystem(entityManager);
        RenderSystem renderSystem = RenderSystem(entityManager);
        CollisionSystem collisionSystem = CollisionSystem(entityManager);

    public:
        void Update(const Uint8* currentKeyboardState) {

            std::vector<EntityPair> col = collisionSystem.CheckIsCollidingRect(entityManager.collideEntities);
            std::vector<EntityPair> vert = collisionSystem.CheckVerticalCollision(col);
            movementSystem.HandleVerticalCollision(vert);

            inputSystem.Update(currentKeyboardState);

            movementSystem.Update(entityManager.moveEntities);
        }

        void Draw(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVector) {

            renderSystem.Update(ren, entityManager.renderEntities, textureVector);
        }
};