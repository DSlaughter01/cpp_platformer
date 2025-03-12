#pragma once
#include "EntityManager.hpp"
#include <map>
#include "Variables.hpp"
#include "SDL.h"
#include "QuadTree.hpp"

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


// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/physicstutorials/6accelerationstructures/Physics%20-%20Spatial%20Acceleration%20Structures.pdf

class CollisionSystem : public System {

    public:
        CollisionSystem(EntityManager &em) : 
            entityManager(em), quadTree(em, 10),
            collisionEntities({}), velocities({}), collidingPairs({}) {}

        void Update() override;
        void InitQuadTree(int rootWidth, int rootHeight, int maxEntsPerNode) {quadTree.Init(rootWidth, rootHeight, maxEntsPerNode);}

    private:
        struct EntityPair {Entity e1, e2;};
        EntityManager &entityManager;
        std::vector<Entity> collisionEntities;
        std::array<std::shared_ptr<CVelocity>, World::MaxEntities> velocities;

        QuadTree quadTree;
        std::vector<std::shared_ptr<QuadTreeNode>> leafNodes;
        std::vector<EntityPair> collidingPairs;

        const int collIdx = 0;
        const int tranIdx = 1;

    private:
        std::vector<EntityPair> BroadPass();
        std::vector<EntityPair> CheckVerticalCollisions(std::vector<EntityPair>& collidingPairs);
        std::vector<EntityPair> CheckHorizontalCollisions(std::vector<EntityPair>& collidingPairs);
        void ResolveHorizontalCollisions(std::vector<EntityPair>& collidingPairs);
        void ResolveVerticalCollisions(std::vector<EntityPair>& collidingPairs);
};