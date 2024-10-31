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
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "Components.hpp"
#include "Variables.hpp"
#include <set>
#include <algorithm>
#include <utility>

// https://medium.com/@savas/nomad-game-engine-part-3-the-big-picture-743cec145685

using Entity = short int;

class EntityManager {

    private:
        Entity playerEntity;

        // A queue of entities that are not already in the game
        std::queue<Entity> availableEntityIDs;

        // True if an entity is present, false if not
        std::array<bool, World::MaxEntities> m_entities;

        // Maps an entity to a vector of its components
        std::unordered_map<Entity, std::array<std::shared_ptr<Component>, World::MaxComponents>> entityComponentMap;

        // The bitsets signifying the presence (or not) of each component
        std::array<std::bitset<World::MaxComponents>, World::MaxEntities> m_entityComponentBitset;

        // Entities that get passed to a system
        std::bitset<World::MaxEntities> renderEntities;
        std::bitset<World::MaxEntities> collisionEntities;
        std::bitset<World::MaxEntities> moveEntities;

    public:

        EntityManager();

        // Entity getters and setters 
        void SetPlayerEntity(Entity e);
        Entity GetPlayerEntity() {return playerEntity;}
        std::array<bool, World::MaxEntities> GetEntities() {return m_entities;}
        std::bitset<World::MaxEntities> GetRenderEntities() {return renderEntities;}
        std::bitset<World::MaxEntities> GetCollisionEntities() {return collisionEntities;}
        std::bitset<World::MaxEntities> GetMoveEntities() {return moveEntities;}

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
        void RemoveComponent(Entity e, int componentID);

        // Checks whether an entity has that component
        bool HasComponent(Entity e, int componentID);

        // Returns a pointer to a component, so that it can then be used
        template<typename T>
        std::shared_ptr<T> GetComponent(Entity e, int componentID);

        // Checks whether entities should be added to vectors to be passed to individual systems
        void CheckAddSystemComponents(Entity e, int componentID);

        // Checks whether removing a component makes them ineligible to be passed to a system
        void CheckRemoveSystemComponents(Entity e, int componentID);
};


/*
IMPLEMENATION OF TEMPLATE FUNCTIONS
*/ 
template<typename T>
void EntityManager::AddComponent(Entity e, T& component) {

    std::shared_ptr<T> comp = std::make_shared<T>(component);

    if (!m_entities[e]) {
        std::cerr << "Trying to access an Entity that doesn't exist." << std::endl;
        return;
    }

    // See if a component of that type has already been added
    if (HasComponent(e, comp->componentID)) {
        std::cout << "Entity " << e << " already has component " << comp->componentID << "." << std::endl;
        return;
    }

    m_entityComponentBitset[e].set(comp->componentID);
    entityComponentMap[e][comp->componentID] = comp;  

    CheckAddSystemComponents(e, comp->componentID);
}


template<typename T>
std::shared_ptr<T> EntityManager::GetComponent(Entity e, int componentID) {

    if (HasComponent(e, componentID))
        return std::dynamic_pointer_cast<T>(entityComponentMap[e][componentID]);
    else
        return nullptr;
}


template<typename T>
void EntityManager::RemoveComponent(Entity e, int componentID) {

    if (!m_entities[e]) {
        std::cerr << "Trying to access an Entity that doesn't exist." << std::endl;
        return;
    }

    if (!entityComponentMap[e][componentID]) {
        std::cout << "Entity " << e << " doesn't have component " << componentID << "." << std::endl;
        return;
    }

    m_entityComponentBitset[e].reset(componentID);
    entityComponentMap[e][componentID] = nullptr;

    CheckRemoveSystemComponents(e, componentID);
}