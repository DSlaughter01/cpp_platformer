#pragma once 
#include <math.h>
#include <bitset>

using Entity = int16_t;

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

    // Level information for scrolling, to be updated according to the map in Game::LoadTilemap
    inline int levelWidth;
    inline int levelHeight;
    inline int leftScroll = (World::WindowWidth) / 8;
    inline int rightScroll = (World::WindowWidth * 7) / 8;

    // Time spacing
    constexpr int FPS = 60;
    constexpr int DesiredFrameTicks = 1000 / FPS;

    // Entities
    constexpr Entity MaxEntities = 1024;
    constexpr int MaxComponents = 32;
    constexpr std::bitset<World::MaxEntities> EmptyEntityBitset {};
    constexpr std::bitset<World::MaxComponents> EmptyComponentBitset {};
    constexpr Entity InvalidEntity = -1;

    constexpr int MaxTextures = 32;
};