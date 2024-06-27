#pragma once
#include "EntityManager.hpp"
#include <map>
#include "Variables.hpp"
#include "SDL2/SDL.h"

struct EntityPair {
    Entity first, second;
};

class System {

    public: 
        virtual void Update() = 0;
        virtual ~System() = default;
};


class InputSystem : public System {

    public:
        InputSystem(EntityManager &em) : entityManager(em) {}

        void Update() override {}
        void Update(const Uint8* currentKeyboardState);

    private:
        EntityManager &entityManager;
};


class RenderSystem : public System {

    public:
        RenderSystem(EntityManager &em) : entityManager(em) {}
        
        void Update() override {}
        void Update(SDL_Renderer* ren, std::set<Entity> &renderSet, std::vector<SDL_Texture*> &textureVector);
    
    private:
        EntityManager &entityManager;
};


class MovementSystem : public System {

    public:
        MovementSystem(EntityManager &em) : entityManager(em) {}
        void Update() override {}
        void HandleVerticalCollision(std::vector<EntityPair> &collidingEntities);

        void Update(std::set<Entity> &moveSet);

    private:
        EntityManager &entityManager;
};


class CollisionSystem : public System {

    public:
        CollisionSystem(EntityManager &em) : entityManager(em) {}
        void Update() override {}

        // First "cheap pass", which tells us which rectangles are colliding 
        std::vector<EntityPair> CheckIsCollidingRect(std::vector<Entity> &collideSet);

        // Check more precisely the point of contact
        // Returns a vector of {topEntity, bottomEntity} pairs
        std::vector<EntityPair> CheckVerticalCollision(std::vector<EntityPair> &collidingPairs);

        // Returns a vector of {leftEntity, rightEntity} pairs - direction is whether to check left ('l') or right ('l') collisions
        std::vector<EntityPair> CheckHorizontalCollision(std::vector<EntityPair> &collidingPairs, char direction);

    private:
        EntityManager &entityManager;
};