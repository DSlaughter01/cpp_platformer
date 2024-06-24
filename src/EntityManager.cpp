#include "EntityManager.hpp"


EntityManager::EntityManager() {

    // Initialise availableEntityIDs
    availableEntityIDs = {};
    for (Entity e = 0; e < MAX_ENTITIES; e++) 
        availableEntityIDs.push(e);

    // Initialise m_entities
    m_entities.fill(false);

    // Reset entityComponentMap
    entityComponentMap.clear();

    // Initialise m_entityComponentBitset
    for (auto &bitset : m_entityComponentBitset) 
        bitset.reset(); 
}


Entity EntityManager::CreateEntity() {

    if (availableEntityIDs.empty()) {
        std::cerr << "No more entities left!!!" << std::endl;
        return MAX_ENTITIES - 1;
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

void EntityManager::RemoveEntity(Entity e) {

    // Add the entity back onto the available entities queue
    availableEntityIDs.push(e);

    // Reset its bitset
    m_entityComponentBitset[e] = 0ULL;   

    // Signify removal in m_entities
    m_entities[e] = false;

    // Remove it from the map
    entityComponentMap.erase(e);
}


void EntityManager::AddComponent(Entity e, std::shared_ptr<Component> component) {

    if (!m_entities[e]) {
        std::cerr << "Trying to access an Entity that doesn't exist." << std::endl;
        return;
    }

    // See if a component of that type has already been added
    std::shared_ptr<Component> thisEntity = entityComponentMap[e][component->componentID];
    if (thisEntity) {
        std::cout << "Entity " << e << " already has component " << component->componentID << "." << std::endl;
        return;
    }

    // Update the bitset in m_entities
    m_entityComponentBitset[e] |= (1ULL << component->componentID);

    // Add component 
    entityComponentMap[e][component->componentID] = component;  
}


bool EntityManager::HasComponent(Entity e, short int componentID) { 

    return ((m_entityComponentBitset[e].test(componentID)));
}


std::shared_ptr<Component> EntityManager::GetComponentAtIndex(Entity e, short int componentID) {

    if (!HasComponent(e, componentID)) {
        std::cerr << "Entity " << e << " does not have component " << componentID << "." << std::endl;
        return nullptr;
    }          

    else {
        // Get the correct component
        return entityComponentMap[e][componentID];
    }
}