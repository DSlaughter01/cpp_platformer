#pragma once 

using Entity = short int;

namespace Player {

    extern int w;
    extern int h;

    extern int maxHP;
    extern int initHP;

    extern int maxDX;
    extern int jumpDY;
    extern int maxFallDY;
};

enum Direction {None, Left, Right, Up, Down};

namespace World {

    extern int gravity;

    // Dimensions - currently does not allow for window resizing
    const int windowHeight = 640;
    const int windowWidth = 900;
    const int tileDim = 64;

    // Time spacing
    const int FPS = 60;
    const int desiredFrameTicks = 1000 / FPS;

    // Entities
    const int maxEntities = 256;
    const int maxComponents = 32;
    const Entity noPlayer = -1;
    const int maxTextures = 32;
};