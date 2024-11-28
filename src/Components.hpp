#pragma once
#include "SDL2/SDL.h"
#include <optional>
#include "Variables.hpp"

// Ideas here https://www.reddit.com/r/roguelikedev/comments/b8ozdk/looking_for_a_good_general_list_of_useful_ecs/

using Entity = int16_t;
using CompID = uint16_t;

enum ComponentID {cTag, cTransform, cSpritesheet, cVelocity, cCollisionState,
                  cHealth, cIsAlive, cLanded};
                  
namespace TagID {
    enum {Player, Tile};
}

// Components form the base class of all the different types of component to follow
struct Component {

    // Virtual destructor allows polymorphism => dyamic_cast possible
    Component(CompID id) : 
        componentID(id) {}
    virtual ~Component() = default;
    CompID componentID;
};

struct CTag : public Component {

    CTag(short int tag) : 
        Component(ComponentID::cTag), m_tag(tag) {}
    short int m_tag;
};


struct CSpritesheet : public Component {

    CSpritesheet(int spritesheetID, 
                 int noImages, 
                 int spritesheetWidth, 
                 int spritesheetHeight, 
                 std::optional<Direction> dir = std::nullopt, 
                 std::optional<Direction> originalDir = std::nullopt, 
                 std::optional<int> frameDuration = std::nullopt) :

        Component(ComponentID::cSpritesheet), 
        m_spritesheetID(spritesheetID),
        m_noImages(noImages), 
        m_spritesheetWidth(spritesheetWidth),
        m_imageHeight(spritesheetHeight),
        m_dir(dir),
        m_originalDir(originalDir),
        m_frameDuration(frameDuration),
        m_framesSinceSpawn(0),
        m_currentImageX(0) {
            
            m_imageWidth = m_spritesheetWidth / m_noImages;

            if (m_frameDuration != std::nullopt)
                m_maxFrames = *m_frameDuration * 1000;
        }

    // Basic spritesheet info      
    int m_spritesheetID; 
    int m_noImages;
    int m_spritesheetWidth;

    // Frame info
    short int m_framesSinceSpawn;
    short int m_maxFrames;
    int m_currentImageX;
    std::optional<int> m_frameDuration;

    // Current direction facing - use Direction enum
    std::optional<Direction> m_dir;
    std::optional<Direction> m_originalDir;

    int m_imageWidth;
    int m_imageHeight;
};

struct CTransform : public Component {

    CTransform(int x, int y, int w, int h) :
        Component(ComponentID::cTransform) {
            m_rect = {x, y, w, h};
            rotation = 0;
            right = x + w;
            bottom = y + h;
        }

    SDL_Rect m_rect;
    int right;
    int bottom;
    int rotation;
};

struct CVelocity : public Component {

    bool m_canAccelerate;   // If false then may still move but at constant speed

    int dx;
    int dy;

    CVelocity(bool canAccelerate, int x, int y) :
        Component(ComponentID::cVelocity), dx(x), dy(y), m_canAccelerate(canAccelerate) {}
};

struct CHealth : public Component {

    int m_health;

    CHealth(int h) : 
        Component(ComponentID::cHealth), m_health(h) {}
};

struct CIsAlive : public Component {

    bool m_isAlive;

    CIsAlive(bool alive) :
        Component(ComponentID::cIsAlive), m_isAlive(alive) {}
};

struct CCollisionState : public Component {

    bool dynamic;

    bool isCollidingUp;
    bool isCollidingDown; 
    bool isCollidingLeft;
    bool isCollidingRight;

    std::unordered_map<Entity, Direction> horCollWith;
    std::unordered_map<Entity, Direction> vertCollWith;

    CCollisionState(bool dyn) :
        Component(ComponentID::cCollisionState),
        dynamic(dyn),
        isCollidingUp(false), isCollidingDown(false),
        isCollidingLeft(false), isCollidingRight(false) {
            horCollWith.clear();
            vertCollWith.clear();
        }
};

struct CLanded : public Component {

    bool m_hasLanded;

    CLanded(bool hasLanded) :
        Component(ComponentID::cLanded), m_hasLanded(hasLanded) {}
};