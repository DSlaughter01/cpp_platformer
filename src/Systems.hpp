#pragma once
#include "EntityManager.hpp"
#include <map>

class System {

    public: 
        virtual void Update() = 0;
        virtual ~System() = default;
};


class RenderSystem : public System {

    public:
        RenderSystem(EntityManager &em) : entityManager(em) {}
        void Update() override {};
        void Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVector);
    
    private:
        EntityManager &entityManager;
};


class MovementSystem : public System {

    public:
        MovementSystem(EntityManager &em) : entityManager(em) {}
        void Update() override;

    private:

        EntityManager &entityManager;
};