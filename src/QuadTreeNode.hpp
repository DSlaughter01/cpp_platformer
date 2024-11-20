#include "SDL2/SDL.h"
#include "Variables.hpp"
#include <array>
#include <memory>
#include <algorithm>

// Represents a node on the QuadTree, containing 4 entities max. before subdividing, and having 0 or 4 child nodes
class QuadTreeNode {

    private:
        // Node depth in the tree, starting from 1 at the root (argument = 0 when created by the QuadTree)
        short int depth;

        // The area covered by this node
        SDL_Rect boundingBox;

        // A length-4 array of entities contained within this region (equivalent to ->vals)
        std::array<Entity, 4> entities = {World::InvalidEntity, World::InvalidEntity, World::InvalidEntity, World::InvalidEntity};

        // A length-4 array of pointers to child nodes (equivalent to ->next)
        std::array<std::shared_ptr<QuadTreeNode>, 4> childNodes {};

    public: 

        QuadTreeNode(SDL_Rect &bb, int prevDepth) :
            depth(prevDepth + 1), boundingBox(bb) {}

        // Getters
        short int GetDepth() {return depth;}
        SDL_Rect& GetBoundingBox() {return boundingBox;}
        std::array<Entity, 4>& GetEntities() {return entities;}
        std::array<std::shared_ptr<QuadTreeNode>, 4>& GetChildNodes() {return childNodes;}

        // Setters/Insertion
        void SetChildNodes(std::array<std::shared_ptr<QuadTreeNode>, 4> &children) {childNodes = children;}
        bool InsertEntity(Entity e); 

        // Clearing/Removal
        bool RemoveEntity(Entity e);
        void ClearEntities() {entities.fill(World::InvalidEntity);}
        void ClearChildren() {childNodes.fill(nullptr);}

        // Checks
        bool CheckIsLeaf() const;
        bool CheckEntityPresence(Entity e);
};