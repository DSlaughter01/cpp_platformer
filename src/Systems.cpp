#include "Systems.hpp"

// How do I pass the entity spritesheets to the GUI?
void RenderSystem::Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVector) {
    for (Entity e = 0; e < MAX_ENTITIES; e++) {

        if (entityManager.HasComponent(e, ComponentID::cSpritesheetID) &&
        entityManager.HasComponent(e, ComponentID::cTransformID)) {
        
            // Access each of the necessary components
            auto spritesheet = std::dynamic_pointer_cast<CSpritesheet>(entityManager.GetComponentAtIndex(e, ComponentID::cSpritesheetID));
            auto transform = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(e, ComponentID::cTransformID));

            // Render to the screen
            SDL_RenderCopy(ren, textureVector[spritesheet->m_spritesheetID], NULL, &(transform->m_transform.rect));
        }
    }
}


void MovementSystem::Update() {

    for (Entity e = 0; e < MAX_ENTITIES; e++) {

        if (entityManager.HasComponent(e, ComponentID::cVelocityID) &&
        entityManager.HasComponent(e, ComponentID::cTransformID)) {
        
            // Access each of the necessary components
            auto vel = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(e, ComponentID::cVelocityID));
            auto pos = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(e, ComponentID::cTransformID));

            // Update entity position
            pos->m_transform.x += vel->m_velocity.dx;
            pos->m_transform.y += vel->m_velocity.dy;
        }
    }
}