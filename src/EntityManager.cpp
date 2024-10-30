#include "EntityManager.hpp"
#include "Variables.hpp"

EntityManager::EntityManager() :
    playerEntity(World::noPlayer) {

    // Initialise availableEntityIDs
    availableEntityIDs = {};
    for (Entity e = 0; e < World::maxEntities; e++) 
        availableEntityIDs.push(e);

    // Initialise m_entities
    m_entities.fill(false);

    // Reset entityComponentMap
    entityComponentMap.clear();

    // Initialise m_entityComponentBitset
    for (auto &bitset : m_entityComponentBitset) 
        bitset.reset(); 

    // System entity vectors
    renderEntities.reset();
    moveEntities.reset();
    collisionEntities.reset();
}


Entity EntityManager::CreateEntity() {

    if (availableEntityIDs.empty()) {
        std::cerr << "No more entities left!!!" << std::endl;
        return World::noPlayer;
    }

    // Get an available entity ID
    Entity e = availableEntityIDs.front();
    availableEntityIDs.pop();

    // Mark presence of an entity
    m_entities[e] = true;

    // Add entity and an empty component vector to the map
    entityComponentMap[e].fill(nullptr);

    return e;
}


void EntityManager::SetPlayerEntity(Entity e) {

    playerEntity = e;
}

// TODO : Add a function which tells the user if an entity is active

void EntityManager::RemoveEntity(Entity e) {

    if (m_entities[e]) {

        if (renderEntities.test(e))
            renderEntities.set(e, false);
        if (moveEntities.test(e))
            moveEntities.set(e, false);
        if (collisionEntities.test(e))
            collisionEntities.set(e, false);

        // Add the entity back onto the available entities queue
        availableEntityIDs.push(e);

        // Reset its bitset
        m_entityComponentBitset[e].reset();   

        // Signify removal in m_entities
        m_entities[e] = false;

        // Remove it from the map
        entityComponentMap.erase(e);
    }
}


void EntityManager::ClearEntities() {

    for (short int e = 0; e < World::maxEntities; e++) {

        if (m_entities[e]) {

            m_entities[e] = false;
            availableEntityIDs.push(e);
            m_entityComponentBitset[e].reset();
            entityComponentMap.erase(e);
        }
    }

    SetPlayerEntity(World::noPlayer);
}


void EntityManager::CheckAddSystemComponents(Entity e, int componentID) {

    // ComponentID is the component that has just been added to entity e
    if (componentID == ComponentID::cTransform) {

        // Transform and Velocity needed to move
        if (HasComponent(e, ComponentID::cVelocity)) {
            moveEntities.set(e, true);
        }
        // Transform and CollisionState needed to collide
        if (HasComponent(e, ComponentID::cCollisionState)) {
            collisionEntities.set(e, true);
        }
        // Transform and Spritesheet needed to render
        if (HasComponent(e, ComponentID::cSpritesheet)) {
            renderEntities.set(e, true);
        }
    }

    else if (componentID == ComponentID::cVelocity && HasComponent(e, ComponentID::cTransform))
            moveEntities.set(e, true);

    else if (componentID == ComponentID::cCollisionState && HasComponent(e, ComponentID::cTransform))
            collisionEntities.set(e, true);

    else if (componentID == ComponentID::cSpritesheet && HasComponent(e, ComponentID::cTransform))
            renderEntities.set(e, true);
}


void EntityManager::CheckRemoveSystemComponents(Entity e, int componentID) {

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cCollisionState)
        collisionEntities.set(e, false);

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cVelocity)
        moveEntities.set(e, false);

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cSpritesheet)
        renderEntities.set(e, false);
}


bool EntityManager::HasComponent(Entity e, int componentID) {

    if (m_entityComponentBitset[e].test(componentID) && entityComponentMap[e][componentID])
        return true;
    else    
        return false;
}