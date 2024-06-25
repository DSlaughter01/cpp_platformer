#pragma once 

namespace Player {

    extern int w;
    extern int h;

    extern int maxHP;
    extern int initHP;

    extern int maxDX;
    extern int jumpDY;
    extern int maxFallDY;
};


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
};