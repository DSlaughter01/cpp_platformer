#pragma once
#include "EntityManager.hpp"
#include <map>
#include "Variables.hpp"

class System {

    public: 
        virtual void Update() = 0;
        virtual ~System() = default;
};


class EventSystem : public System {

    public:
        EventSystem(EntityManager &em) : entityManager(em) {}

        void Update() override {}
        void Update(const Uint8* currentKeyboardState);

    private:
        EntityManager &entityManager;
};


class RenderSystem : public System {

    public:
        RenderSystem(EntityManager &em) : entityManager(em) {}
        
        void Update() override {}
        void Update(SDL_Renderer* ren, std::set<Entity> &renderSet, std::vector<SDL_Texture*> &textureVector);
    
    private:
        EntityManager &entityManager;
};


class MovementSystem : public System {

    public:
        MovementSystem(EntityManager &em) : entityManager(em) {}
        void Update() override {}
        void Update(std::set<Entity> &moveSet);

    private:
        EntityManager &entityManager;
};