#include "QuadTreeNode.hpp"

bool QuadTreeNode::InsertEntity(Entity e) {

    // Conditions on which the entity cannot be inserted
    if (entities.size() >= maxEntities && depth < maxDepth) {
        return false;
    }
    
    // Check if the entity already exists in entities
    if (std::find(entities.begin(), entities.end(), e) == entities.end()) {

        // Add the entity
        entities.emplace_back(e);
        return true;
    }

    else {
        return false;
    }
}


bool QuadTreeNode::RemoveEntity(Entity e) {
    
    auto it = std::find(entities.begin(), entities.end(), e);

    if (it != entities.end()) {
        entities.erase(it); 
        return true;
    }
    else
        return false;
}      


bool QuadTreeNode::CheckIsLeaf() const {
        
    for (auto &i : childNodes) {
        if (i) {
            return false;
        }

    } 
    return true;
}


bool QuadTreeNode::CheckEntityPresence(Entity e) const {
    auto it = std::find(entities.begin(), entities.end(), e);
    return (it != entities.end()) ? true : false;
}

bool QuadTreeNode::NeedsSubdivision() {  
    return (entities.size() >= 4 && depth < maxDepth && CheckIsLeaf()) ? true : false;
}