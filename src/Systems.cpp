#include "Systems.hpp"

void EventSystem::Update(const Uint8* currentKeyboardState) {

    std::shared_ptr<CVelocity> vel = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(entityManager.playerEntity, ComponentID::cVelocityID));

    // Player moves horizontally
    if (currentKeyboardState[SDL_SCANCODE_LEFT]) 
        vel->m_velocity.dx = -Player::maxDX;
    else if (currentKeyboardState[SDL_SCANCODE_RIGHT]) 
        vel->m_velocity.dx = Player::maxDX;
    else 
        vel->m_velocity.dx = 0;

    // Player jumps (or falls)
    if (currentKeyboardState[SDL_SCANCODE_SPACE])
        vel->m_velocity.dy = Player::jumpDY;
    else {
        if (vel->m_velocity.dy < Player::maxFallDY) 
            vel->m_velocity.dy += World::gravity;
    }
}


void RenderSystem::Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVector) {

    for (Entity e = 0; e < World::maxEntities; e++) {

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

    for (Entity e = 0; e < World::maxEntities; e++) {

        if (entityManager.HasComponent(e, ComponentID::cVelocityID) &&
        entityManager.HasComponent(e, ComponentID::cTransformID)) {
        
            // Access each of the necessary components
            std::shared_ptr<CVelocity> vel = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(e, ComponentID::cVelocityID));
            std::shared_ptr<CTransform> pos = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(e, ComponentID::cTransformID));

            // Update entity position
            pos->m_transform.x += vel->m_velocity.dx;
            pos->m_transform.y += vel->m_velocity.dy;

            // Update entity rectangle
            pos->m_transform.rect.x += vel->m_velocity.dx;
            pos->m_transform.rect.y += vel->m_velocity.dy;
        }
    }
}