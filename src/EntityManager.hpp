#pragma once
#include <string>
#include <array>
#include <iostream>
#include <bitset>
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

// https://medium.com/@savas/nomad-game-engine-part-3-the-big-picture-743cec145685

using Entity = short int;

class EntityManager {

    public:

        EntityManager();

        Entity CreateEntity();
        
        void SetPlayerEntity(Entity e);

        void RemoveEntity(Entity e);

        void AddComponent(Entity e, std::shared_ptr<Component> component);

        // Check to see whether the adding a component makes the entity eligible for any system
        void CheckSystems(Entity e);

        // Checks whether an entity has that component
        bool HasComponent(Entity e, short int componentID);

        // Returns a reference to the component, so that it can then be modified
        std::shared_ptr<Component> GetComponentAtIndex(Entity e, short int componentID);

        Entity playerEntity;

        // Entities possessing certain qualities, relevant for efficiency of passing entities to systems
        std::set<Entity> renderEntities;
        std::set<Entity> moveEntities;

    private:
        // A queue of entities that are not already in the game
        std::queue<Entity> availableEntityIDs;

        // True if an entity is present, false if not
        std::array<bool, World::maxEntities> m_entities;

        // Maps an entity to the vector of its components
        std::unordered_map<Entity, std::array<std::shared_ptr<Component>, World::maxEntities>> entityComponentMap;

        // The bitsets signifying the presence (or not) of each components
        std::array<std::bitset<World::maxComponents>, World::maxEntities> m_entityComponentBitset;
};