#pragma once
#include "SDL.h"
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

    CTag(uint16_t tag) : 
        Component(ComponentID::cTag), m_tag(tag) {}
    uint16_t m_tag;
};


struct CSpritesheet : public Component {

    CSpritesheet(uint16_t spritesheetID, 
                 uint16_t noImages, 
                 uint16_t spritesheetWidth, 
                 uint16_t spritesheetHeight, 
                 std::optional<Direction> dir = std::nullopt, 
                 std::optional<Direction> originalDir = std::nullopt, 
                 std::optional<uint16_t> frameDuration = std::nullopt) :

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
    uint16_t m_spritesheetID; 
    uint16_t m_noImages;
    uint16_t m_spritesheetWidth;

    // Frame info
    int16_t m_framesSinceSpawn;
    int16_t m_maxFrames;
    uint16_t m_currentImageX;
    std::optional<uint16_t> m_frameDuration;

    // Current direction facing - use Direction enum
    std::optional<Direction> m_dir;
    std::optional<Direction> m_originalDir;

    uint16_t m_imageWidth;
    uint16_t m_imageHeight;
};

struct CTransform : public Component {

    CTransform(uint16_t x, uint16_t y, uint16_t w, uint16_t h) :
        Component(ComponentID::cTransform) {
            m_rect = {x, y, w, h};
            rotation = 0;
            right = x + w;
            bottom = y + h;
        }

    SDL_Rect m_rect;
    uint16_t right;
    uint16_t bottom;
    uint16_t rotation;
};

struct CVelocity : public Component {

    int16_t dx;
    int16_t dy;

    CVelocity(int16_t x, int16_t y) :
        Component(ComponentID::cVelocity), dx(x), dy(y) {}
};

struct CHealth : public Component {

    uint16_t m_health;

    CHealth(uint16_t h) : 
        Component(ComponentID::cHealth), m_health(h) {}
};

struct CIsAlive : public Component {

    bool m_isAlive;

    CIsAlive(bool alive) :
        Component(ComponentID::cIsAlive), m_isAlive(alive) {}
};

struct CCollisionState : public Component {

    bool dynamic;
    bool solid;

    CCollisionState(bool dyn, bool sol) :
        Component(ComponentID::cCollisionState),
        dynamic(dyn), solid(sol) {}
};

struct CLanded : public Component {

    bool m_hasLanded;

    CLanded(bool hasLanded) :
        Component(ComponentID::cLanded), m_hasLanded(hasLanded) {}
};