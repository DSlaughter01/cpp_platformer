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
        RenderSystem(EntityManager &em) : entityManager(em) {}
        
        void Update() override {}
        void Update(SDL_Renderer* ren, std::bitset<World::maxEntities> &renderEntities,
                    std::vector<SDL_Texture*> &textureVec);
        // void AddTexture(SDL_Texture* tex);
    
    private:
        EntityManager &entityManager;
        std::array<SDL_Texture*, World::maxTextures> textureArray;
};


class MovementSystem : public System {
    
    public:
        MovementSystem(EntityManager &em) : entityManager(em) {}
        void Update() override {};
        void Update(std::bitset<World::maxEntities> &moveEntities);

    private:
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
        anyHorCollisions(false), anyVertCollisions(false) {}
        void Update() override;

    private:

        bool anyHorCollisions, anyVertCollisions;

        std::array<bool, World::maxEntities> entities {};
        std::array<std::shared_ptr<CTransform>, World::maxEntities> transforms {};
        std::array<std::shared_ptr<CCollisionState>, World::maxEntities> collStates {};
        std::array<std::shared_ptr<CVelocity>, World::maxEntities> velocities {};
        std::array<std::shared_ptr<CLanded>, World::maxEntities> landComps {};

        EntityManager &entityManager;

    private:
        void GetEntityData();
        void ResetCollisionVariables();
        void CheckVerticalCollisions();
        void CheckHorizontalCollisions();
        void ResolveHorizontalCollisions();
        void ResolveVerticalCollisions();
};