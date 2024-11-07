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
    renderEntities.reset();
    moveEntities.reset();
    collisionEntities.reset();
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

        if (renderEntities.test(e))
            renderEntities.set(e, false);
        if (moveEntities.test(e))
            moveEntities.set(e, false);
        if (collisionEntities.test(e))
            collisionEntities.set(e, false);

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


void EntityManager::CheckRemoveSystemComponents(Entity e, CompID componentID) {

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cCollisionState)
        collisionEntities.set(e, false);

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cVelocity)
        moveEntities.set(e, false);

    if (componentID == ComponentID::cTransform || componentID == ComponentID::cSpritesheet)
        renderEntities.set(e, false);
}


bool EntityManager::HasComponent(Entity e, CompID componentID) {

    if (e != World::InvalidEntity && m_entities[e]) {
        if (m_entityComponentBitset[e].test(componentID))
            return true;
    }  
    return false;
}