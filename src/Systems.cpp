#include "Systems.hpp" 

void RenderSystem::Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVec) {

    std::array<bool, World::maxEntities> entities = entityManager.GetEntities();

    std::shared_ptr<CSpritesheet> spritesheet = nullptr;
    std::shared_ptr<CTransform> transform = nullptr;

    for (Entity e = 0; e < World::maxEntities; e++) {

        if (entities[e]) {

            spritesheet = entityManager.GetComponent<CSpritesheet>(e, ComponentID::cSpritesheet);
            transform = entityManager.GetComponent<CTransform>(e, ComponentID::cTransform);

            // Render to the screen
            if (spritesheet && transform) {

                SDL_Rect src = {spritesheet->m_currentImageX, 0, spritesheet->m_imageWidth, spritesheet->m_imageHeight};
                SDL_RenderCopy(ren, textureVec[spritesheet->m_spritesheetID], &src, &(transform->m_rect));
            }
        }
    }
}


void MovementSystem::Update() {

    std::array<bool, World::maxEntities> entities = entityManager.GetEntities();

    // Get the transform and velocity components
    std::shared_ptr<CTransform> transform = nullptr;
    std::shared_ptr<CVelocity> velocity = nullptr;

    // Move the transform components by the velocity
    for (Entity e = 0; e < World::maxEntities; e++) {
        
        if (entities[e]) {

            transform = entityManager.GetComponent<CTransform>(e, ComponentID::cTransform);
            velocity = entityManager.GetComponent<CVelocity>(e, ComponentID::cVelocity);

            if (transform && velocity) {

                transform->m_rect.x += velocity->dx;
                transform->m_rect.y += velocity->dy;
            }
        }
    }
}


void InputSystem::Update(const Uint8* keyboard) {

    Entity player = entityManager.GetPlayerEntity();

    std::shared_ptr<CVelocity> velocity = entityManager.GetComponent<CVelocity>(player, ComponentID::cVelocity);
    std::shared_ptr<CLanded> landed = entityManager.GetComponent<CLanded>(player, ComponentID::cLanded);

    if (!velocity) 
        return;


    // Move horizontally if uninhibited by other collidable objects
    if (keyboard[SDL_SCANCODE_LEFT])
        velocity->dx = -Player::maxDX;
    else if (keyboard[SDL_SCANCODE_RIGHT]) 
        velocity->dx = Player::maxDX;
    else
        velocity->dx = 0;

    if (!landed) {
        std::cout << "Player doesn't have a Landed component" << std::endl;
        return;
    }

    // Fall
    if (landed->m_hasLanded && keyboard[SDL_SCANCODE_SPACE]) {
        velocity->dy = Player::jumpDY;
        landed->m_hasLanded = false;
    }

    else if (!(landed->m_hasLanded) && velocity->dy < Player::maxFallDY)
        velocity->dy += World::gravity;
    else if (landed->m_hasLanded)
        velocity->dy = 0;

}


void CollisionSystem::Update() {

    GetEntityData();
    CheckVerticalCollisions();

    if (anyVertCollisions)
        ResolveVerticalCollisions();

    CheckHorizontalCollisions();

    if (anyHorCollisions)
        ResolveHorizontalCollisions();
}


void CollisionSystem::GetEntityData() {

    entities = entityManager.GetEntities();

    for (Entity i = 0; i < World::maxEntities; i++) {

        if (entities[i]) {
            transforms[i] = entityManager.GetComponent<CTransform>(i, ComponentID::cTransform);
            collStates[i] = entityManager.GetComponent<CCollisionState>(i, ComponentID::cCollisionState); 
            velocities[i] = entityManager.GetComponent<CVelocity>(i, ComponentID::cVelocity); 
            landComps[i] = entityManager.GetComponent<CLanded>(i, ComponentID::cLanded);
        }
    }
}


void CollisionSystem::CheckVerticalCollisions() {

    anyVertCollisions = false;

    for (Entity i = 0; i < World::maxEntities - 1; i++) {

        if (collStates[i]) {

            collStates[i]->isCollidingUp = false;
            collStates[i]->isCollidingDown = false;
            collStates[i]->vertCollWith.clear();
        }

        if (landComps[i])
            landComps[i]->m_hasLanded = false;
    }

    // Do a triangular loop
    for (Entity i = 0; i < World::maxEntities - 1; i++) {

        if (!entities[i]) 
            continue; 

        for (Entity j = i + 1; j < World::maxEntities; j++) {

            if (!entities[j]) 
                continue;

            // Transform and collision components are needed for a collision to be signaled 
            if (!(transforms[i] && transforms[j] && collStates[i] && collStates[j]))
                continue;

            const SDL_Rect rect1 = transforms[i]->m_rect;
            const SDL_Rect rect2 = transforms[j]->m_rect;

            // If the 2 rectangles don't collide, move on
            if (!(SDL_HasIntersection(&rect1, &rect2)))
                continue;

            // Same for vertical collision
            bool a = (rect1.y < rect2.y && rect1.y + rect1.h > rect2.y); // i on top, j on bottom
            bool b = (rect2.y < rect1.y && rect2.y + rect2.h > rect1.y); // i on bottom, j on top

            if ((a || b) && !anyVertCollisions)
                anyVertCollisions = true;

            if (a) {

                collStates[i]->isCollidingDown = true;
                collStates[j]->isCollidingUp = true;

                collStates[i]->vertCollWith.insert({j, Direction::Down});
                collStates[j]->vertCollWith.insert({i, Direction::Up});
            }

            else if (b) {

                collStates[i]->isCollidingUp = true;
                collStates[j]->isCollidingDown = true;

                collStates[i]->vertCollWith.insert({j, Direction::Up});
                collStates[j]->vertCollWith.insert({i, Direction::Down});
            }
        }
    }
}


void CollisionSystem::CheckHorizontalCollisions() {

    anyHorCollisions = false;

    for (Entity i = 0; i < World::maxEntities - 1; i++) {

        if (collStates[i]) {
            collStates[i]->isCollidingLeft = false;
            collStates[i]->isCollidingRight = false;
            collStates[i]->horCollWith.clear();
        }
    }

    // Do a triangular loop
    for (Entity i = 0; i < World::maxEntities - 1; i++) {

        if (!entities[i]) 
            continue; 

        for (Entity j = i + 1; j < World::maxEntities; j++) {

            if (!entities[j]) 
                continue;

            // Transform and collision components are needed for a collision to be signaled 
            if (!(transforms[i] && transforms[j] && collStates[i] && collStates[j]))
                continue;

            const SDL_Rect rect1 = transforms[i]->m_rect;
            const SDL_Rect rect2 = transforms[j]->m_rect;

            // If the 2 rectangles don't collide, move on
            if (!(SDL_HasIntersection(&rect1, &rect2)))
                continue;

            // Work out whether there is horizontal collision
            bool a = (rect1.x < rect2.x && rect1.x + rect1.w > rect2.x); // i on left, j on right
            bool b = (rect2.x < rect1.x && rect2.x + rect2.w > rect1.x); // i on right, j on left 

            if ((a || b) && !anyHorCollisions)
                anyHorCollisions = true;

            if (a) {

                collStates[i]->isCollidingRight = true;
                collStates[j]->isCollidingLeft = true;

                collStates[i]->horCollWith.insert({j, Direction::Right});
                collStates[j]->horCollWith.insert({i, Direction::Left});
            }

            else if (b) {

                collStates[i]->isCollidingLeft = true;
                collStates[j]->isCollidingRight = true;

                collStates[i]->horCollWith.insert({j, Direction::Left});
                collStates[j]->horCollWith.insert({i, Direction::Right});
            }
        }
    }
}


void CollisionSystem::ResolveHorizontalCollisions() {

    // Move the object out of the way
    for (Entity i = 0; i < World::maxEntities; i++) {

        // Needs all 3 components, and for the velocity to be changeable
        if (!(collStates[i] && transforms[i] && velocities[i]) || !velocities[i]->m_canAccelerate)
            continue;

        // For each entity that Entity i is colliding with
        for (auto &ent : collStates[i]->horCollWith) {

            if (!transforms[ent.first] || !collStates[ent.first])
                continue;
 
            Entity left = (ent.second == Direction::Right) ? i : ent.first;
            Entity right = (left == i) ? ent.first : i;

            // Fixed objects may not have a velocity component
            if (!velocities[right]) {    // Left object moving
                velocities[left]->dx = 0;
                transforms[left]->m_rect.x = transforms[right]->m_rect.x - transforms[left]->m_rect.w;
            }
            else if (!velocities[left]) {
                velocities[right]->dx = 0;
                transforms[right]->m_rect.x = transforms[left]->m_rect.x + transforms[left]->m_rect.w;
            }

            // No object moving left - glue left transform to right, set left dx to right dx
            else if (velocities[left]->dx > 0 && velocities[right]->dx >= 0) {
                transforms[left]->m_rect.x = transforms[right]->m_rect.x - transforms[left]->m_rect.w;
                velocities[left]->dx = velocities[right]->dx;
            }

            // No object moving right - glue right transform to left, set right dx to left dx 
            else if (velocities[right]->dx < 0 && velocities[left]->dx <= 0) {

                transforms[right]->m_rect.x = transforms[left]->m_rect.x + transforms[left]->m_rect.w;
                velocities[right]->dx = velocities[left]->dx;
            }

            // Both objects moving towards each other
            else if (velocities[left]->dx > 0 && velocities[right]->dx < 0) {
                
                velocities[left]->dx = 0;
                velocities[right]->dx = 0;

                // Move each object apart by half of the overlap
                int xDiff = transforms[left]->m_rect.x + transforms[left]->m_rect.w - transforms[right]->m_rect.x;

                // Add 1 to get the right separation if xDiff is odd
                if (xDiff % 2 != 0) {
                    xDiff += 1;
                }

                transforms[left]->m_rect.x -= (xDiff / 2);
                transforms[right]->m_rect.x += (xDiff / 2);
            }
        }
    }
}


void CollisionSystem::ResolveVerticalCollisions() {

    // Move the object out of the way
    for (Entity i = 0; i < World::maxEntities; i++) {

        // Needs all 3 components, and for the velocity to be changeable
        if (!(collStates[i] && transforms[i] && velocities[i]) || !velocities[i]->m_canAccelerate) 
            continue;
        
        // For each entity that Entity i is colliding with
        for (auto &ent : collStates[i]->vertCollWith) {
 
            if (!transforms[ent.first] || !collStates[ent.first])
                continue;

            Entity top = (ent.second == Direction::Down) ? i : ent.first;
            Entity bottom = (top == i) ? ent.first : i;

            // Falling object landed (if it has a landed component)
            if (landComps[top]) {
                landComps[top]->m_hasLanded = true;
            }

            // Fixed objects may not have a velocity component
            if (!velocities[bottom]) {
                velocities[top]->dy = 0;
                transforms[top]->m_rect.y = transforms[bottom]->m_rect.y - transforms[top]->m_rect.h;
            }
            else if (!velocities[top]) {
                velocities[bottom]->dy = 0;
                transforms[bottom]->m_rect.y = transforms[top]->m_rect.y + transforms[top]->m_rect.h;
            }

            // Top object moving faster than bottom object (means moving down)
            else if (velocities[top]->dy > velocities[bottom]->dy) {

                transforms[top]->m_rect.y = transforms[bottom]->m_rect.y - transforms[top]->m_rect.h;
                velocities[top]->dy = velocities[bottom]->dy;
            }

            // Bottom object moving faster (more negatively, given since they collide) than top object
            else if (velocities[bottom]->dy < velocities[top]->dy) {
                transforms[bottom]->m_rect.y = transforms[top]->m_rect.y + transforms[top]->m_rect.h;
                velocities[bottom]->dy = velocities[top]->dy;
            }

            // Both objects moving towards each other
            else if (velocities[top]->dy > 0 && velocities[bottom]->dy < 0) {
                
                velocities[top]->dy = 0;
                velocities[bottom]->dy = 0;

                // Move each object apart by half of the overlap
                int yDiff = transforms[top]->m_rect.y + transforms[top]->m_rect.h - transforms[bottom]->m_rect.y;

                // Add 1 to get the right separation if xDiff is odd
                if (yDiff % 2 != 0) {
                    yDiff += 1;
                }

                transforms[top]->m_rect.y -= (yDiff / 2);
                transforms[bottom]->m_rect.y += (yDiff / 2);
            }
        }
    }
}