#include "SDL2/SDL.h"
#include "Variables.hpp"
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <iostream>
// Represents a node on the QuadTree, containing a limited number of entities before subdividing, and having 0 or 4 child nodes
class QuadTreeNode {

    private:

        // Max amount of entities the node can hold, except for leaf nodes which have an overflow
        const uint16_t maxEntities;

        // Node depth in the tree, starting from 1 at the root (argument = 0 when created by the QuadTree)
        uint16_t depth;
        uint16_t maxDepth;

        // The area covered by this node
        SDL_Rect boundingBox;

        // A length-4 array of entities contained within this region (equivalent to ->vals)
        std::vector<Entity> entities {};

        // A length-4 array of pointers to child nodes (equivalent to ->next)
        std::array<std::shared_ptr<QuadTreeNode>, 4> childNodes;

    public: 

        QuadTreeNode(SDL_Rect &bb, uint16_t prevDepth, uint16_t maxDep, uint16_t maxEnt) :
            depth(prevDepth + 1), maxDepth(maxDep), boundingBox(bb), maxEntities(maxEnt) {
                
                entities.reserve(maxEntities);
                childNodes.fill(nullptr);
            }

        // Getters
        int GetMaxEntities() {return maxEntities;}
        uint16_t GetDepth() {return depth;}
        SDL_Rect& GetBoundingBox() {return boundingBox;}
        std::vector<Entity>& GetEntities() {return entities;}
        std::array<std::shared_ptr<QuadTreeNode>, 4>& GetChildNodes() {return childNodes;}

        // Setters/Insertion
        void SetChildNodes(std::array<std::shared_ptr<QuadTreeNode>, 4> &children) {childNodes = children;}
        bool InsertEntity(Entity e); 

        // Clearing/Removal
        bool RemoveEntity(Entity e);
        void ClearEntities() {entities.clear();}
        void ClearChildren() {childNodes.fill(nullptr);}

        // Checks
        bool CheckIsLeaf() const;
        bool CheckEntityPresence(Entity e);
        bool NeedsSubdivision();
};