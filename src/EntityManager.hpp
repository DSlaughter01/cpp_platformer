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
        std::array<bool, World::maxEntities> m_entities;

        // Maps an entity to the array of its components
        std::unordered_map<Entity, std::array<std::shared_ptr<Component>, World::maxComponents>> entityComponentMap;

        // The bitsets signifying the presence (or not) of each component
        std::array<std::bitset<World::maxComponents>, World::maxEntities> m_entityComponentBitset;

    public:

        EntityManager();

        void SetPlayerEntity(Entity e);
        Entity GetPlayerEntity() {return playerEntity;}
        std::array<bool, World::maxEntities> GetEntities() {return m_entities;}

        // ENTITY FUNCTIONS
        Entity CreateEntity();
    
        void RemoveEntity(Entity e);

        void ClearEntities();


        // COMPONENT FUNCTIONS
        template<typename T>
        void AddComponent(Entity e, T& component) {

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
        }

        // Checks whether an entity has that component
        bool HasComponent(Entity e, int componentID);

        // Returns a pointer to a component, so that it can then be used
        template<typename T>
        std::shared_ptr<T> GetComponent(Entity e, int componentID) {

            if (HasComponent(e, componentID))
                return std::dynamic_pointer_cast<T>(entityComponentMap[e][componentID]);
            else
                return nullptr;
        }


        template<typename T>
        void RemoveComponent(Entity e, int componentID) {

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
        }
};