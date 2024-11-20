#include "QuadTreeNode.hpp"

bool QuadTreeNode::InsertEntity(Entity e) {

    // Check if the entities array is full, and if not get the first available index
    auto it = std::find(entities.begin(), entities.end(), World::InvalidEntity);
    Entity availableIdx = (it != entities.end()) ? std::distance(entities.begin(), it) : World::InvalidEntity;

    // Check if the entity already exists in entities
    auto it2 = std::find(entities.begin(), entities.end(), e);

    if (availableIdx != World::InvalidEntity && it2 != entities.end()) {

        // Assign the first non-assigned entity to the correct position in the tree's entities vector
        entities[availableIdx] = e;
        return true;
    }

    else 
        return false;
}


bool QuadTreeNode::RemoveEntity(Entity e) {
    
    auto it = std::find(entities.begin(), entities.end(), e);
    Entity entityIdx = (it != entities.end()) ? std::distance(entities.begin(), it) : World::InvalidEntity;

    if (entityIdx != World::InvalidEntity) {
        entities[entityIdx] = World::InvalidEntity; 
        return true;
    }
    else
        return false;
}      


bool QuadTreeNode::CheckIsLeaf() const {

    return std::none_of(childNodes.begin(), childNodes.end(), [](const std::shared_ptr<QuadTreeNode>& child) {
                        return !child; // Predicate: Node is not null
                    });
}


bool QuadTreeNode::CheckEntityPresence(Entity e) {
    auto it = std::find(entities.begin(), entities.end(), e);
    return (it != entities.end()) ? true : false;
}