#pragma once
#include "SDL2/SDL.h"

// Ideas here https://www.reddit.com/r/roguelikedev/comments/b8ozdk/looking_for_a_good_general_list_of_useful_ecs/

using Entity = short int;

struct Transform {
    Transform(int fx, int fy, int fw, int fh) {rect = {fx, fy, fw, fh};}
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
    enum {cTag, cTransform, cSpritesheet, cDimensions, cVelocity, cBBox, 
          cHealth, cEnemy, cRigidBody, cDamage, cIsAlive, cIsColliding,
          cCollidingWith, cLanded};
}

namespace TagID {
    enum {Player, Tile};
}


struct Component {
    // Add a virtual destructor to make polymorphic, allowing dyamic_cast
    Component(short int id) : 
        componentID(id) {}
    virtual ~Component() = default;
    short int componentID;
};

struct CTag : public Component {

    CTag(short int tag) : 
        Component(ComponentID::cTag), m_tag(tag) {}
    short int m_tag;
};

struct CSpritesheet : public Component {

    CSpritesheet(short int spritesheet) :
        Component(ComponentID::cSpritesheet), m_spritesheetID(spritesheet) {}
    short int m_spritesheetID; 
};

struct CTransform : public Component {

    CTransform(int x, int y, int w, int h) :
        Component(ComponentID::cTransform), m_transform(x, y, w, h) {}
    Transform m_transform;
};

struct CDimensions : public Component {

    CDimensions(int x, int y) :
        Component(ComponentID::cDimensions), m_dimensions(x, y) {}
    Dimensions m_dimensions;
};

struct CVelocity : public Component {

    CVelocity(int x, int y) :
        Component(ComponentID::cVelocity), m_velocity(x, y) {}
    Velocity m_velocity;
};

struct CHealth : public Component {

    CHealth(int h) : 
        Component(ComponentID::cHealth), m_health(h) {}
    int m_health;
};

// Which player tag type this entity is an enemy to
struct CEnemy : public Component {

    CEnemy(short int enemy) : 
        Component(ComponentID::cEnemy), m_enemy(enemy) {}
    short int m_enemy;
};

struct CRigidBody : public Component {

    CRigidBody(bool isRB) : 
        Component(ComponentID::cRigidBody), m_rigidBody(isRB) {}
    bool m_rigidBody;
};

struct CDamage : public Component {

    CDamage(int dam) :
        Component(ComponentID::cDamage), m_damage(dam) {}
    int m_damage;
};

struct CIsAlive : public Component {

    CIsAlive(bool alive) :
        Component(ComponentID::cIsAlive), m_isAlive(alive) {}
    bool m_isAlive;
};

struct CIsColliding : public Component {

    CIsColliding(bool coll) :
            Component(ComponentID::cIsColliding), m_isColliding(coll) {}
    bool m_isColliding;
};

struct CCollidingWith : public Component {
    
    CCollidingWith(Entity e) :
            Component(ComponentID::cCollidingWith), m_collidingWith(e) {}
    Entity m_collidingWith;
};

struct CLanded : public Component {
    CLanded(bool hasLanded) :
        Component(ComponentID::cLanded), m_hasLanded(hasLanded) {}
    bool m_hasLanded;
};