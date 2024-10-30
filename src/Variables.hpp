#pragma once 

using Entity = short int;

namespace Player {

    inline  int w = 64;
    inline  int h = 64;

    inline  int maxHP = 5;
    inline  int initHP = 3;

    inline  int maxDX = 3;
    inline  int jumpDY = -20;
    inline  int maxFallDY = 8;
};

enum Direction {None, Left, Right, Up, Down};

namespace World {

    inline int gravity = 1;

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