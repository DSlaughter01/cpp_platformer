#include "EntityManager.hpp"
#include "Variables.hpp"

EntityManager::EntityManager() :
    playerEntity(World::InvalidEntity) {

    // Initialise availableEntityIDs
    availableEntityIDs = {};
    for (Entity e = 0; e < World::MaxEntities; e++) 
        availableEntityIDs.push(e);

    // Initialise m_entities
    m_entities.reset();

    // Reset entityComponentMap and entityComponentBitset
    entityComponentMap.clear();
    m_entityComponentBitset.clear();

    // System entity vectors
    renderEntities.clear();
    moveEntities.clear();
    collisionEntities.clear();
}


Entity EntityManager::CreateEntity() {

    if (availableEntityIDs.empty()) {
        std::cerr << "No more entities left!!!" << std::endl;
        return World::InvalidEntity;
    }

    // Get an available entity ID
    Entity e = availableEntityIDs.front();
    availableEntityIDs.pop();

    // Mark presence of an entity
    m_entities.set(e, true);

    // Insert an empty bitset associated with the entity to mark as yet 0 components
    m_entityComponentBitset.insert({e, World::EmptyComponentBitset});

    return e;
}


void EntityManager::SetPlayerEntity(Entity e) {

    playerEntity = e;
}


void EntityManager::RemoveEntity(Entity e) {

    if (m_entities[e]) {

        // Slightly hacky but a transform component is common to all systems, so passing this as an argument the entity from all systems easily
        CheckRemoveSystemComponents(e, ComponentID::cTransform);

        // Add the entity back onto the available entities queue
        availableEntityIDs.push(e);

        // Signify removal in m_entities
        m_entities.set(e, false);

        // Remove it from the map
        for (int i = 0; i < World::MaxComponents; i++) {
            if (m_entityComponentBitset[e].test(i)) {
                BinaryKey key = ReturnBinaryKey(e, i);
                entityComponentMap.erase(key);
            }
        }

        // Reset its bitset
        m_entityComponentBitset.erase(e);   
    }
}


void EntityManager::ClearEntities() {

    for (short int e = 0; e < World::MaxEntities; e++) {

        if (m_entities.test(e)) {
            m_entities.set(e, false);
            availableEntityIDs.push(e);
        }
    }

    entityComponentMap.clear();
    m_entityComponentBitset.clear();
    SetPlayerEntity(World::InvalidEntity);
}


void EntityManager::CheckAddSystemComponents(Entity e, CompID componentID) {

    // ComponentID is the component that has just been added to entity e
    if (componentID == ComponentID::cTransform) {

        // Transform and Velocity needed to move
        if (HasComponent(e, ComponentID::cVelocity)) {
            moveEntities.emplace_back(e);
        }
        // Transform and CollisionState needed to collide
        if (HasComponent(e, ComponentID::cCollisionState)) {
            collisionEntities.emplace_back(e);
        }
        // Transform and Spritesheet needed to render
        if (HasComponent(e, ComponentID::cSpritesheet)) {
            renderEntities.emplace_back(e);
        }
    }

    else if (componentID == ComponentID::cVelocity && HasComponent(e, ComponentID::cTransform))
            moveEntities.emplace_back(e);

    else if (componentID == ComponentID::cCollisionState && HasComponent(e, ComponentID::cTransform))
            collisionEntities.emplace_back(e);

    else if (componentID == ComponentID::cSpritesheet && HasComponent(e, ComponentID::cTransform))
            renderEntities.emplace_back(e);
}


void EntityManager::CheckRemoveSystemComponents(Entity e, CompID componentID) {

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cCollisionState) {

        auto collIt = std::find(collisionEntities.begin(), collisionEntities.end(), e);

        if (collIt != collisionEntities.end())
            collisionEntities.erase(collIt);
    }

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cVelocity) {
        
        auto moveIt = std::find(moveEntities.begin(), moveEntities.end(), e);

        if (moveIt != moveEntities.end())
            moveEntities.erase(moveIt);
    }

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cSpritesheet) {
        
        auto renderIt = std::find(renderEntities.begin(), renderEntities.end(), e);

        if (renderIt != renderEntities.end())
            renderEntities.erase(renderIt);
    }
}


bool EntityManager::HasComponent(Entity e, CompID componentID) {

    if (e != World::InvalidEntity && m_entities[e]) {
        if (m_entityComponentBitset[e].test(componentID))
            return true;
    }  
    return false;
}