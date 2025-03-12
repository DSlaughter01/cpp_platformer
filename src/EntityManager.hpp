#pragma once
#include <string>
#include <array>
#include <iostream>
#include <bitset>
#include <optional>
#include <vector>
#include <memory>
#include <queue>
#include <any>
#include <unordered_map>
#include <SDL.h>
#include <SDL_image.h>
#include "Components.hpp"
#include "Variables.hpp"
#include <set>
#include <algorithm>
#include <utility>

using Entity = int16_t;
using CompID = uint16_t;
using BinaryKey = uint32_t;

class EntityManager {

    private:
        Entity playerEntity;

        // A queue of entities that are not already in the game
        std::queue<Entity> availableEntityIDs;

        // True if an entity is present, false if not
        std::bitset<World::MaxEntities> m_entities;

        // Maps an entity to a vector of its components - the key is a pair of Entity, componentID (short int)
        std::unordered_map<BinaryKey, std::shared_ptr<Component>> entityComponentMap;

        // The bitsets signifying the presence (or not) of each component in active entities
        std::unordered_map<Entity, std::bitset<World::MaxComponents>> m_entityComponentBitset;

        // Entities that get passed to a system
        std::vector<Entity> renderEntities;
        std::vector<Entity> collisionEntities;
        std::vector<Entity> moveEntities;

        // These 2 are to be passed to the QuadTree's update function
        std::vector<Entity> newCollisionEntities;
        std::vector<Entity> removedCollisionEntities;

    public:

        EntityManager();

        inline BinaryKey ReturnBinaryKey(Entity entity, CompID componentID) {
            return (static_cast<BinaryKey>(entity) << 8) | componentID;
        }

        void ResetCollisionEntityChanges() {
            newCollisionEntities.clear();
            removedCollisionEntities.clear();
        }

        // Entity getters and setters 
        void SetPlayerEntity(Entity e);
        Entity GetPlayerEntity() {return playerEntity;}
        std::bitset<World::MaxEntities> GetEntities() {return m_entities;}
        std::vector<Entity> GetRenderEntities() {return renderEntities;}
        std::vector<Entity> GetCollisionEntities() {return collisionEntities;}
        std::vector<Entity> GetNewCollisionEntities() {return newCollisionEntities;}
        std::vector<Entity> GetRemovedCollisionEntities() {return removedCollisionEntities;}
        std::vector<Entity> GetMoveEntities() {return moveEntities;}

        // ENTITY FUNCTIONS
        Entity CreateEntity();
        void RemoveEntity(Entity e);

        // Cleans up all entities in the game
        void ClearEntities();

        // COMPONENT FUNCTIONS
        // Adds a component to the specified entity, updating other structures holding entity/component data
        template<typename T>
        void AddComponent(Entity e, T& component);

        // Removes a component from the specified entity, updating other structures holding entity/component data
        template<typename T>
        void RemoveComponent(Entity e, CompID componentID);

        // Checks whether an entity has that component
        bool HasComponent(Entity e, CompID componentID);

        // Returns a pointer to a component, so that it can then be used or modified as appropriate
        template<typename T>
        std::shared_ptr<T> GetComponent(Entity e, CompID componentID);

        // Checks whether entities should be added to vectors to be passed to individual systems
        void CheckAddSystemComponents(Entity e, CompID componentID);

        // Checks whether removing a component makes them ineligible to be passed to a system
        void CheckRemoveSystemComponents(Entity e, CompID componentID);
};


/*
IMPLEMENATION OF TEMPLATE FUNCTIONS
*/ 
template<typename T>
void EntityManager::AddComponent(Entity e, T& component) {

    if (e == World::InvalidEntity || !m_entities[e]) {
        std::cerr << "Trying to access an Entity that doesn't exist." << std::endl;
        return;
    }

    std::shared_ptr<T> comp = std::make_shared<T>(component);

    // See if a component of that type has already been added
    if (HasComponent(e, comp->componentID)) {
        std::cout << "Entity " << e << " already has component " << comp->componentID << "." << std::endl;
        return;
    }

    m_entityComponentBitset[e].set(comp->componentID);
    BinaryKey key = ReturnBinaryKey(e, comp->componentID);
    entityComponentMap.insert({key, comp});  

    CheckAddSystemComponents(e, comp->componentID);
}


template<typename T>
std::shared_ptr<T> EntityManager::GetComponent(Entity e, CompID componentID) {

    if (e == World::InvalidEntity || !HasComponent(e, componentID)) 
        return nullptr;

    else {
        BinaryKey key = ReturnBinaryKey(e, componentID);
        return std::dynamic_pointer_cast<T>(entityComponentMap[key]);
    }
}


template<typename T>
void EntityManager::RemoveComponent(Entity e, CompID componentID) {

    if (e == World::InvalidEntity || !m_entities[e]) {
        std::cerr << "Trying to access an Entity that doesn't exist." << std::endl;
        return;
    }

    if (!HasComponent(e, componentID)) {
        std::cout << "Entity " << e << " doesn't have component " << componentID << "." << std::endl;
        return;
    }

    m_entityComponentBitset[e].reset(componentID);
    BinaryKey key = ReturnBinaryKey(e, componentID);
    entityComponentMap.erase(key);

    CheckRemoveSystemComponents(e, componentID);
}