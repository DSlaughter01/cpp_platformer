#include "QuadTree.hpp"

QuadTree::QuadTree(EntityManager &em, int maxD) :
    entityManager(em), leafNodeCount(1), maxDepth(maxD) {}


void QuadTree::Init(int rootWidth, int rootHeight, int maxEntsPerNode) {

    // Creates the root node as the whole level
    maxEntitiesPerNode = maxEntsPerNode;
    SDL_Rect boundingBox = {0, 0, rootWidth, rootHeight};
    root = std::make_shared<QuadTreeNode>(boundingBox, 0, maxDepth, maxEntsPerNode);

    // Creates the initial tree
    Update(entityManager.GetCollisionEntities(), {});
}


std::vector<std::shared_ptr<QuadTreeNode>> QuadTree::GetLeafNodes() {
    
    std::vector<std::shared_ptr<QuadTreeNode>> leafNodes {};
    leafNodes.reserve(leafNodeCount); // Preallocate memory, leafNodeCount should already be known
    CollectLeafNodes(root, leafNodes);

    return leafNodes;      
}


void QuadTree::CollectLeafNodes(std::shared_ptr<QuadTreeNode> &curr, std::vector<std::shared_ptr<QuadTreeNode>> &leafNodes) {

    if (curr->CheckIsLeaf())
        leafNodes.emplace_back(curr); 
    
    else {
        for (auto &child : curr->GetChildNodes()) {

            if (child) 
                CollectLeafNodes(child, leafNodes);
        }
    }  
}


void QuadTree::Merge(std::shared_ptr<QuadTreeNode> &parent) {
    
    // Child nodes should be merged into parent nodes if the parent and child nodes combine contain 
    // 4 or fewer DIFFERENT entities (because one entity may span more than one child node). 
    int differentEntityCount {};
    std::vector<Entity> differentEntityVec {};
    differentEntityVec.reserve(4);

    // Check parent entities
    for (auto &e : parent->GetEntities()) {
        if (e != World::InvalidEntity) {
            differentEntityCount++;
            differentEntityVec.emplace_back(e);
        }
    }

    bool breaker = false; // Used to break out of the internal loop to prevent useless iterations

    // Check child entities
    for (auto &child : parent->GetChildNodes()) {

        for (auto &e : child->GetEntities()) {

            // If the entity is not already in the vector, emplace it
            auto it = std::find(differentEntityVec.begin(), differentEntityVec.end(), e);

            if (it == differentEntityVec.end()) {
                
                // Return if there are more than 4 different entities, which can therefore not be contained in a single node
                differentEntityCount++;
                if (differentEntityCount > 4) {
                    breaker = true;
                    break;
                }

                differentEntityVec.emplace_back(e);
            }
        }

        if (breaker) 
            break;
    }

    if (differentEntityCount <= 4) {

        // Set all entities to World::InvalidEntity
        parent->ClearEntities(); 

        // Set parent's entities to differentEntities
        for (int i = 0; i < differentEntityVec.size(); i++) 
            parent->InsertEntity(differentEntityVec[i]);

        // Remove children from parent
        parent->ClearChildren();
    }
}


bool QuadTree::Subdivide(std::shared_ptr<QuadTreeNode> &curr) {

    if (!curr->NeedsSubdivision()) {
        return false;
    }

    leafNodeCount += 3; // current node is no longer a leaf, but create 4 more nodes by subdividing => 3
    
    // Create a rectangle with 1/2 width and 1/2 height
    SDL_Rect boundingBox = curr->GetBoundingBox();
    int originalX = boundingBox.x;
    int originalY = boundingBox.y;
    int newWidth = boundingBox.w / 2;
    int newHeight = boundingBox.h / 2;

    // Assign 1 rectangle to each quadrant
    SDL_Rect topLeftBB = {originalX, originalY, newWidth, newHeight};
    SDL_Rect topRightBB = {originalX + newWidth, originalY, newWidth, newHeight};
    SDL_Rect bottomLeftBB = {originalX, originalY + newHeight, newWidth, newHeight};
    SDL_Rect bottomRightBB = {originalX + newWidth, originalY + newHeight, newWidth, newHeight};

    // Create new nodes
    int depth = curr->GetDepth();
    std::shared_ptr<QuadTreeNode> topLeftNode = std::make_shared<QuadTreeNode>(topLeftBB, depth, maxDepth, maxEntitiesPerNode);
    std::shared_ptr<QuadTreeNode> topRightNode = std::make_shared<QuadTreeNode>(topRightBB, depth, maxDepth, maxEntitiesPerNode);
    std::shared_ptr<QuadTreeNode> bottomLeftNode = std::make_shared<QuadTreeNode>(bottomLeftBB, depth, maxDepth, maxEntitiesPerNode);
    std::shared_ptr<QuadTreeNode> bottomRightNode = std::make_shared<QuadTreeNode>(bottomRightBB, depth, maxDepth, maxEntitiesPerNode);

    std::array<std::shared_ptr<QuadTreeNode>, 4> newChildren = {topLeftNode, topRightNode, bottomLeftNode, bottomRightNode};
    curr->SetChildNodes(newChildren);

    // Redistribute and clear the current node's entities
    for (auto& e : curr->GetEntities()) {

        if (e) {

            for (auto &child : curr->GetChildNodes()) {
                InsertEntity(child, e);
            }
            curr->ClearEntities();
        }
    }

    return true;
}


void QuadTree::ClearNode(std::shared_ptr<QuadTreeNode> &curr) {

    // Only clear leaf nodes, otherwise will leave isolated nodes further along the branch
    if (curr->CheckIsLeaf()) {
        curr->ClearEntities();
        leafNodeCount--;
    }
}


// Finds the correct node(s) for insertion of an entity
void QuadTree::InsertEntity(std::shared_ptr<QuadTreeNode> &curr, Entity e) {

    // Check if the entity and node BBs intersect, and return if not as no child nodes can satisfy
    std::shared_ptr<CTransform> transform = entityManager.GetComponent<CTransform>(e, ComponentID::cTransform);
    
    if (!transform)
        return;
      
    SDL_Rect entityBB = transform->m_rect;
    if (!SDL_HasIntersection(&entityBB, &(curr->GetBoundingBox())))
        return;
        
    // If it does intersect, and it is a leaf ...
    if (curr->CheckIsLeaf()) {

        // ... try and insert the entity into the current node
        bool inserted = curr->InsertEntity(e);

        // If it cannot be inserted, the node needs to subdivide and redistribute entities
        if (!inserted) {

            bool divided = Subdivide(curr);

            if (divided) {

                for (auto &child : curr->GetChildNodes()) 
                    InsertEntity(child, e);
            }

            // This usually happens when the tree has reached max depth and cannot subdivide, so stores more entities than normal
            else 
                curr->InsertEntity(e);
        }
    }

    // Intersection && !leafNode => go further into tree
    else {
        for (auto &child : curr->GetChildNodes()) {
            InsertEntity(child, e);
        }
    }
}


// Finds the correct node(s) for removal of an entity
void QuadTree::RemoveEntity(std::shared_ptr<QuadTreeNode> &curr, Entity e) {

    // This algorithm needs to visit all of the leaf nodes, and check whether the entity still fits in the node
    // It could also get called if an entity is deleted from the game itself, so doesn't rely on bounding box
    if (curr->CheckIsLeaf()) 
        curr->RemoveEntity(e);

    else {
        for (auto &child : curr->GetChildNodes())
            RemoveEntity(child, e); // Confusing that RemoveEntity and curr->RemoveEntity both exist?
    }
}


void QuadTree::Update(const std::vector<Entity> &createdEntities, const std::vector<Entity> &deletedEntities) {
 
    // Created entities need to be inserted into the right part of the tree
    if (createdEntities.size() > 0) {
        for (int i = 0; i < createdEntities.size(); i++) {
            InsertEntity(root, createdEntities[i]);
        }
    }

    // Entities that are no longer active should be removed from the tree
    if (deletedEntities.size() > 0) {
        for (int i = 0; i < deletedEntities.size(); i++) 
            RemoveEntity(root, deletedEntities[i]);  
    }

    // Once the created and deleted entities have been processed, clear so they don't keep getting passed to the Update method
    entityManager.ResetCollisionEntityChanges();
    
    std::vector<Entity> collisionEntities = entityManager.GetCollisionEntities();
    std::shared_ptr<CCollisionState> coll = nullptr;
    std::shared_ptr<CTransform> tran = nullptr;
    std::shared_ptr<CVelocity> vel = nullptr;
    SDL_Rect rect;

    std::vector<std::shared_ptr<QuadTreeNode>> leafNodes = GetLeafNodes();

    for (auto &e : collisionEntities) {

        // Get the position of each entity, if the entity is dynamic
        coll = entityManager.GetComponent<CCollisionState>(e, ComponentID::cCollisionState);
        tran = entityManager.GetComponent<CTransform>(e, ComponentID::cTransform);

        if (coll && coll->dynamic && tran) {
            
            rect = tran->m_rect;

            vel = entityManager.GetComponent<CVelocity>(e, ComponentID::cVelocity);

            if (vel && (vel->dx != 0 || vel->dy != 0)) {

                // If the entity moved in the last frame, remove it from its old node and insert it into the new one.
                RemoveEntity(root, e);
                InsertEntity(root, e);
            }
        }
    } 
}