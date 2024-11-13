#pragma once
#include "EntityManager.hpp"
#include <map>
#include "Variables.hpp"
#include "SDL2/SDL.h"

class System {

    public: 
        virtual void Update() = 0;
        virtual ~System() = default;
};


class RenderSystem : public System {

    public:
        RenderSystem(EntityManager &em) : 
            entityManager(em), renderEntities({}) {}
        
        void Update() override {}
        void Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVec, int &xOffset);
        // void AddTexture(SDL_Texture* tex);
    
    private:
        EntityManager &entityManager;
        std::vector<Entity> renderEntities;
        std::array<SDL_Texture*, World::MaxTextures> textureArray;
};


class MovementSystem : public System {
    
    public:
        MovementSystem(EntityManager &em) : 
            entityManager(em), moveEntities({}) {}
        void Update() override;

    private:
        std::vector<Entity> moveEntities;
        EntityManager &entityManager;
};


class InputSystem : public System {

    public:
        InputSystem(EntityManager &em) : entityManager(em) {}
        void Update() override {};
        void Update(const Uint8* keyboard);
    
    private:
        EntityManager &entityManager;
};


class CollisionSystem : public System {

    public:
        CollisionSystem(EntityManager &em) : entityManager(em),
        anyHorCollisions(false), anyVertCollisions(false), collisionEntities({}) {}
        void Update() override;

    private:

        bool anyHorCollisions, anyVertCollisions;
        std::vector<Entity> collisionEntities;

        std::array<std::shared_ptr<CVelocity>, World::MaxEntities> velocities {};

        const int collIdx = 0;
        const int tranIdx = 1;

        EntityManager &entityManager;

    private:
        void CheckVerticalCollisions();
        void CheckHorizontalCollisions();
        void ResolveHorizontalCollisions();
        void ResolveVerticalCollisions();
};