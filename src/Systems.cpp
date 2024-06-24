#include "Systems.hpp"

void RenderSystem::Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVector) {
    for (Entity e = 0; e < MAX_ENTITIES; e++) {

        if (entityManager.HasComponent(e, ComponentID::cSpritesheetID) &&
        entityManager.HasComponent(e, ComponentID::cTransformID)) {
        
            // Access each of the necessary components
            std::shared_ptr<CSpritesheet> spritesheet = std::dynamic_pointer_cast<CSpritesheet>(entityManager.GetComponentAtIndex(e, ComponentID::cSpritesheetID));
            std::shared_ptr<CTransform> transform = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(e, ComponentID::cTransformID));

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
            std::shared_ptr<CVelocity> vel = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(e, ComponentID::cVelocityID));
            std::shared_ptr<CTransform> pos = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(e, ComponentID::cTransformID));

            // Update entity position
            pos->m_transform.x += vel->m_velocity.dx;
            pos->m_transform.y += vel->m_velocity.dy;

            // Update entity rectangle
            pos->m_transform.rect = {pos->m_transform.x, 
                                     pos->m_transform.y, 
                                     pos->m_transform.w, 
                                     pos->m_transform.h};
        }
    }
}