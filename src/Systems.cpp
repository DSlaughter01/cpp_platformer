#include "Systems.hpp"

void InputSystem::Update(const Uint8* currentKeyboardState) {
    
    // Some checks need to be put in place
        // If there is no player entity, then vel doesn't exist
        // If there is no Velocity component, the same is true
        // This should extend to any component the program tries to access

    Entity player = entityManager.playerEntity;
    std::shared_ptr<CVelocity> playerVel = nullptr;
    std::shared_ptr<CLanded> playerLand = nullptr;

    // Get player entity
    if (player && entityManager.HasComponent(player, ComponentID::cVelocity)) {

        playerVel = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(player, ComponentID::cVelocity));

        if (playerVel == nullptr)
            return;

        // Player moves horizontally
        if (currentKeyboardState[SDL_SCANCODE_LEFT]) {
            playerVel->m_velocity.dx = -Player::maxDX;
        }
        else if (currentKeyboardState[SDL_SCANCODE_RIGHT]) 
            playerVel->m_velocity.dx = Player::maxDX;
        else 
            playerVel->m_velocity.dx = 0;

        // Player jumps (or falls)
        if (currentKeyboardState[SDL_SCANCODE_SPACE]) {
            playerVel->m_velocity.dy = Player::jumpDY;            
        }
        else {
            if (entityManager.HasComponent(player, ComponentID::cLanded)) 

                playerLand = std::dynamic_pointer_cast<CLanded>(entityManager.GetComponentAtIndex(player, ComponentID::cLanded));

                if (playerLand->m_hasLanded) 
                    playerVel->m_velocity.dy = 0;
                else if (playerVel->m_velocity.dy < Player::maxFallDY)
                    playerVel->m_velocity.dy += World::gravity;
        }
    }
}


void RenderSystem::Update(SDL_Renderer* ren, std::set<Entity> &renderSet, std::vector<SDL_Texture*> &textureVector) {


    std::shared_ptr<CSpritesheet> spritesheet = nullptr;
    std::shared_ptr<CTransform> transform = nullptr;

    for (auto &e : renderSet) {

        // Check
        if (entityManager.HasComponent(e, ComponentID::cSpritesheet) &&
        entityManager.HasComponent(e, ComponentID::cTransform)) {
        
            // Access each of the necessary components
            spritesheet = std::dynamic_pointer_cast<CSpritesheet>(entityManager.GetComponentAtIndex(e, ComponentID::cSpritesheet));
            transform = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(e, ComponentID::cTransform));

            // Render to the screen
            SDL_RenderCopy(ren, textureVector[spritesheet->m_spritesheetID], NULL, &(transform->m_transform.rect));
        }
    }
}


void MovementSystem::HandleVerticalCollision(std::vector<EntityPair> &collidingEntities) {

    bool willPlayerFall = true;
    Entity player = entityManager.playerEntity;
    std::shared_ptr<CLanded> CplayerLanded = nullptr;

    if (entityManager.HasComponent(player, ComponentID::cLanded)) {
        CplayerLanded = std::dynamic_pointer_cast<CLanded>(entityManager.GetComponentAtIndex(player, ComponentID::cLanded));
    }

    for (int i = 0; i < collidingEntities.size(); i++) {

        // collidingEntities is ordered in {top, bottom} pairs
        Entity top = collidingEntities[i].first;
        Entity bottom = collidingEntities[i].second;

        std::shared_ptr<CTag> tagTop = std::dynamic_pointer_cast<CTag>(entityManager.GetComponentAtIndex(top, ComponentID::cTag));

        // Set the higher rectangle's bottom to the lower rectangle's top
        std::shared_ptr<CTransform> tranTop = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(top, ComponentID::cTransform));
        std::shared_ptr<CTransform> tranBot = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(bottom, ComponentID::cTransform));

        tranTop->m_transform.rect.y = tranBot->m_transform.rect.y - tranTop->m_transform.rect.h;

        // Change the velocity of the 2 appropriately
        std::shared_ptr<CVelocity> velTop = nullptr;
        std::shared_ptr<CVelocity> velBot = nullptr;

        if (entityManager.HasComponent(top, ComponentID::cVelocity))
            velTop = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(top, ComponentID::cVelocity));

        if (entityManager.HasComponent(bottom, ComponentID::cVelocity))  
            velBot = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(bottom, ComponentID::cVelocity));

         
        if (!velTop && !velBot) {
            continue;
        } 
        
        if (top == entityManager.playerEntity) {
            willPlayerFall = false;
            CplayerLanded->m_hasLanded = true; 
        }
    }

    if (willPlayerFall) 
        CplayerLanded->m_hasLanded = false;
}



void MovementSystem::Update(std::set<Entity> &moveSet) {

    for (auto &e : moveSet) {

        // Check
        if (entityManager.HasComponent(e, ComponentID::cVelocity) &&
        entityManager.HasComponent(e, ComponentID::cTransform)) {
        
            // Access each of the necessary components
            std::shared_ptr<CVelocity> vel = std::dynamic_pointer_cast<CVelocity>(entityManager.GetComponentAtIndex(e, ComponentID::cVelocity));
            std::shared_ptr<CTransform> pos = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(e, ComponentID::cTransform));

            // Update entity position
            pos->m_transform.rect.x += vel->m_velocity.dx;
            pos->m_transform.rect.y += vel->m_velocity.dy;
        }
    }
}


std::vector<EntityPair> CollisionSystem::CheckIsCollidingRect(std::vector<Entity> &collideVec) {

    std::vector<EntityPair> sol = {};

    // Get the transform components of each entity in collideVec
    std::vector<std::shared_ptr<CTransform>> transformVec;

    for (int i = 0; i < collideVec.size(); i++) {
        if (entityManager.HasComponent(collideVec[i], ComponentID::cTransform)) {
            std::shared_ptr<CTransform> pos = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(collideVec[i], ComponentID::cTransform));
            transformVec.emplace_back(pos);
        }
    }

    // See if each pair of rectangles in the transform components intersect 
    for (int i = 0; i < transformVec.size(); i++) {

        for (int j = i + 1; j < transformVec.size(); j++) {

            // Get the two rectangles 
            const SDL_Rect rect1 = transformVec[i]->m_transform.rect;
            const SDL_Rect rect2 = transformVec[j]->m_transform.rect;

            // Add to solution if collide
            if (SDL_HasIntersection(&rect1, &rect2)) {
                EntityPair newPair = {collideVec[i], collideVec[j]};
                sol.emplace_back(newPair);
            }
        }
    }

    return sol;
}


std::vector<EntityPair> CollisionSystem::CheckVerticalCollision(std::vector<EntityPair> &collidingPairs) {

    std::vector<EntityPair> sol = {};
    std::shared_ptr<CTransform> tran1 = nullptr;
    std::shared_ptr<CTransform> tran2 = nullptr;
    SDL_Rect rect1;
    SDL_Rect rect2;
    EntityPair newPair = {};

    // Get each pair
    for (auto &pair : collidingPairs) {

        // Get transform components
        tran1 = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(pair.first, ComponentID::cTransform));
        tran2 = std::dynamic_pointer_cast<CTransform>(entityManager.GetComponentAtIndex(pair.second, ComponentID::cTransform));
        
        // Get rectangles and check for vertical overlap
        rect1 = tran1->m_transform.rect;
        rect2 = tran2->m_transform.rect;

        // If the bottom edge of rect1 is lower than the top edge of rect2
        if (rect1.y + rect1.h >= rect2.y && rect1.y < rect2.y) {
            std::cout << "Top: " << rect1.y << std::endl;
            std::cout << "Bottom: " << rect2.y << std::endl;
            newPair = {pair.first, pair.second};
            sol.emplace_back(newPair);
        }

        // If the bottom edge of rect2 is lower than the top edge of rect1
        else if (rect2.y + rect2.h >= rect1.y && rect2.y < rect1.y) {
            newPair = {pair.second, pair.first};
            std::cout << "Top: " << rect2.y << std::endl;
            std::cout << "Bottom: " << rect1.y << std::endl;
            sol.emplace_back(newPair);
        }
    }

    return sol;
}


std::vector<EntityPair> CollisionSystem::CheckHorizontalCollision(std::vector<EntityPair> &collidingPairs, char direction) {

    std::vector<EntityPair> sol = {};

    return sol;


}
