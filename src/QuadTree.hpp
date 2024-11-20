#pragma once
#include "SDL2/SDL.H"

#include <array>
#include <memory>
#include <bitset>

#include "Variables.hpp"
#include "EntityManager.hpp"
#include "QuadTreeNode.hpp"

class QuadTree {

    private:
        int leafNodeCount;

        // The maximum depth of the tree, to prevent infinite recursion
        int maxDepth;
        
        EntityManager &entityManager;
        std::shared_ptr<QuadTreeNode> root;

    public:
        QuadTree(EntityManager &em, int maxD);

        std::shared_ptr<QuadTreeNode> GetRoot() {return root;}

        void Update(const std::vector<Entity> &createdEntities, const std::vector<Entity> &deletedEntities);

        // Returns a vector of all leaf nodes in the tree. To be used externally in the collision system.
        std::vector<std::shared_ptr<QuadTreeNode>> GetLeafNodes();

    private:

        // Helper function to be called recursively inside GetLeafNodes
        void CollectLeafNodes(std::shared_ptr<QuadTreeNode> &curr, std::vector<std::shared_ptr<QuadTreeNode>> &leafNodes);
    
        void Merge(std::shared_ptr<QuadTreeNode> &parent);

        // Creates 4 new child node for the specified node
        void Subdivide(std::shared_ptr<QuadTreeNode> &curr);

        void ClearNode(std::shared_ptr<QuadTreeNode> &curr);

        // Recursively traverses the tree and inserts the entity where it fits in the node's BB. Should be called with root by user.
        void InsertEntity(std::shared_ptr<QuadTreeNode> &curr, Entity e);

        // Recursively traverses the tree and removes the entity where it does not fit in the node's BB. Should be called with root by user.
        void RemoveEntity(std::shared_ptr<QuadTreeNode> &curr, Entity e);
};