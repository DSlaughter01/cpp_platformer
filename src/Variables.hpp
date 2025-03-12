#pragma once 
#include <math.h>
#include <bitset>

using Entity = int16_t;

// Using CMake so define root directory
#define PROJECT_ROOT PROJECT_ROOT_DIR

namespace Player {

    inline uint16_t width = 64;
    inline uint16_t height = 64;

    inline uint16_t maxHP = 5;
    inline uint16_t initHP = 3;

    inline int16_t maxDX = 3;
    inline int16_t jumpDY = -20;
    inline int16_t maxFallDY = 8;
};

enum Direction {None, Left, Right, Up, Down};

namespace World {

    inline int16_t gravity = 0;

    // Dimensions - currently does not allow for window resizing
    constexpr uint16_t WindowHeight = 640;
    constexpr uint16_t WindowWidth = 900;
    constexpr uint16_t TileDim = 64;

    // Level information for scrolling, to be updated according to the map in Game::LoadTilemap
    inline uint16_t levelWidth;
    inline uint16_t levelHeight;
    inline uint16_t leftScroll = (World::WindowWidth) / 8;
    inline uint16_t rightScroll = (World::WindowWidth * 7) / 8;

    // Time spacing
    constexpr uint16_t FPS = 60;
    constexpr uint16_t DesiredFrameTicks = 1000 / FPS;

    // Entities
    constexpr Entity MaxEntities = 1024;
    constexpr uint16_t MaxComponents = 32;
    constexpr std::bitset<World::MaxEntities> EmptyEntityBitset {};
    constexpr std::bitset<World::MaxComponents> EmptyComponentBitset {};
    constexpr Entity InvalidEntity = -1;

    constexpr uint16_t MaxTextures = 32;
};