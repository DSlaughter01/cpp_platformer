#pragma once
#include "SDL2/SDL.h"

struct Transform {
    Transform(int fx, int fy, int fw, int fh) :
        x(fx), y(fy), w(fw), h(fh) {
            rect = {fx, fy, fw, fh};
        }
    float x, y, w, h;
    SDL_Rect rect;
};

struct Dimensions {
    Dimensions(int fx, int fy) : x(fx), y(fy) {}
    int x, y;
};
struct Velocity {
    Velocity(int fx, int fy) : dx(fx), dy(fy) {}
    int dx, dy;
};

namespace ComponentID {
    enum {cTagID, cTransformID, cSpritesheetID, cDimensionsID, cVelocityID, cBBoxID};
    enum {PlayerTag, TileTag, EnemyTag};
}

struct Component {
    // Add a virtual destructor to make polymorphic, allowing dyamic_cast
    Component(short int id) : componentID(id) {}
    virtual ~Component() = default;
    short int componentID;
};

struct CTag : public Component {

    CTag() : Component(ComponentID::cTagID) {}
    short int m_tag;
};

struct CSpritesheet : public Component {

    CSpritesheet(short int spritesheet) :
        Component(ComponentID::cSpritesheetID), m_spritesheetID(spritesheet) {}
    short int m_spritesheetID; 
};

struct CTransform : public Component {
    CTransform(float x, float y, float w, float h) :
        Component(ComponentID::cTransformID), m_transform(x, y, w, h) {}
    Transform m_transform;
};

struct CDimensions : public Component {
    CDimensions(int x, int y) :
        Component(ComponentID::cDimensionsID), m_dimensions(x, y) {}
    Dimensions m_dimensions;
};

struct CVelocity : public Component {
    CVelocity(int x, int y) :
        Component(ComponentID::cVelocityID), m_velocity(x, y) {}
    Velocity m_velocity;
};