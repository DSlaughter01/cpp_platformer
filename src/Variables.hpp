#pragma once 

using Entity = short int;

namespace Player {

    inline int width = 64;
    inline int height = 64;

    inline int maxHP = 5;
    inline int initHP = 3;

    inline int maxDX = 3;
    inline int jumpDY = -20;
    inline int maxFallDY = 8;
};

enum Direction {None, Left, Right, Up, Down};

namespace World {

    inline int gravity = 1;

    // Dimensions - currently does not allow for window resizing
    constexpr int WindowHeight = 640;
    constexpr int WindowWidth = 900;
    constexpr int TileDim = 64;

    // Time spacing
    constexpr int FPS = 60;
    constexpr int DesiredFrameTicks = 1000 / FPS;

    // Entities
    constexpr int MaxEntities = 256;
    constexpr int MaxComponents = 32;
    constexpr Entity InvalidEntity = -1;
    constexpr int MaxTextures = 32;
};