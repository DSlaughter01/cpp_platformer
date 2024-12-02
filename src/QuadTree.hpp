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
        int maxDepth;
        int maxEntitiesPerNode;
        
        EntityManager &entityManager;
        std::shared_ptr<QuadTreeNode> root;
        std::vector<std::shared_ptr<QuadTreeNode>> leafNodes;
        std::unordered_map<Entity, std::vector<std::shared_ptr<QuadTreeNode>>> entityNodeMap;

    public:
        QuadTree(EntityManager &em, int maxD);

        void Init(int rootWidth, int rootHeight, int maxEntsPerNode); // Not dealt with in constructor because levelHeight and levelWidth aren't initialised in time

        std::shared_ptr<QuadTreeNode> GetRoot() {return root;}
        int GetMaxEntitiesPerNode() {return maxEntitiesPerNode;}
        int GetMaxDepth() {return maxDepth;}
        std::vector<std::shared_ptr<QuadTreeNode>> GetLeafNodes() {return leafNodes;}

        void Update();

    private:

        void Merge(std::shared_ptr<QuadTreeNode> &parent);

        // Creates 4 new child node for the specified node
        bool Subdivide(std::shared_ptr<QuadTreeNode> &curr);

        void ClearNode(std::shared_ptr<QuadTreeNode> &curr);

        // Recursively traverses the tree and inserts the entity where it fits in the node's BB. Should be called with root by user.
        void InsertEntity(std::shared_ptr<QuadTreeNode> &curr, Entity e);

        // Recursively traverses the tree and removes the entity where it does not fit in the node's BB. Should be called with root by user.
        void RemoveEntity(std::shared_ptr<QuadTreeNode> &curr, Entity e);
};