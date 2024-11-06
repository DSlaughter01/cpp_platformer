#include "Systems.hpp" 

void RenderSystem::Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVec) {

    std::bitset<World::MaxEntities> renderEntities = entityManager.GetRenderEntities();

    std::shared_ptr<CSpritesheet> spritesheet = nullptr;
    std::shared_ptr<CTransform> transform = nullptr;
    std::shared_ptr<CVelocity> velocity = nullptr;

    for (Entity e = 0; e < renderEntities.size(); e++) {

        if (renderEntities[e]) {

            spritesheet = entityManager.GetComponent<CSpritesheet>(e, ComponentID::cSpritesheet);
            transform = entityManager.GetComponent<CTransform>(e, ComponentID::cTransform);

            // Render to the screen
            if (spritesheet && transform) {

                SDL_RendererFlip flipVal = (spritesheet->m_dir == spritesheet->m_originalDir) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

                SDL_Rect src = {spritesheet->m_currentImageX, 0, spritesheet->m_imageWidth, spritesheet->m_imageHeight};
                SDL_RenderCopyEx(ren, textureVec[spritesheet->m_spritesheetID], &src, &(transform->m_rect), 0, NULL, flipVal);

                // Change spritesheet image, if there is more than one image on the spritesheet and the sprite is moving horizontally but not vertically
                velocity = entityManager.GetComponent<CVelocity>(e, ComponentID::cVelocity);

                bool cond = (velocity &&
                            velocity->dx != 0 &&
                            velocity->dy == 0 &&
                            spritesheet->m_frameDuration != std::nullopt &&
                            spritesheet->m_framesSinceSpawn % *spritesheet->m_frameDuration);

                if (cond)
                        spritesheet->m_currentImageX = (spritesheet->m_currentImageX + spritesheet->m_imageWidth) % spritesheet->m_spritesheetWidth;

                spritesheet->m_framesSinceSpawn++;
            }
        }
    }
}


void MovementSystem::Update() {

    std::bitset<World::MaxEntities> moveEntities = entityManager.GetMoveEntities();

    // Get the transform and velocity components
    std::shared_ptr<CTransform> transform = nullptr;
    std::shared_ptr<CVelocity> velocity = nullptr;
    std::shared_ptr<CSpritesheet> spritesheet = nullptr;

    // Move the transform components by the velocity
    for (Entity e = 0; e < World::MaxEntities; e++) {
        
        if (moveEntities[e]) {

            transform = entityManager.GetComponent<CTransform>(e, ComponentID::cTransform);
            velocity = entityManager.GetComponent<CVelocity>(e, ComponentID::cVelocity);
            spritesheet = entityManager.GetComponent<CSpritesheet>(e, ComponentID::cSpritesheet);

            if (transform && velocity) {

                transform->m_rect.x += velocity->dx;
                transform->m_rect.y += velocity->dy;

                // Change the direction spritesheets are rendered
                if (spritesheet) {

                    if (spritesheet->m_dir != Direction::Right && velocity->dx > 0)
                        spritesheet->m_dir = Direction::Right;

                    if (spritesheet->m_dir != Direction::Left && velocity->dx < 0) 
                        spritesheet->m_dir = Direction::Left;
                }
            }
        }
    }
}


void InputSystem::Update(const Uint8* keyboard) {

    Entity player = entityManager.GetPlayerEntity();

    std::shared_ptr<CVelocity> velocity = entityManager.GetComponent<CVelocity>(player, ComponentID::cVelocity);
    std::shared_ptr<CLanded> landed = entityManager.GetComponent<CLanded>(player, ComponentID::cLanded);

    if (!velocity) {
        std::cout << "Player doesn't have a velocity component" << std::endl;
        return;
    } 

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

    // Jump
    if (landed->m_hasLanded && keyboard[SDL_SCANCODE_SPACE]) {
        velocity->dy = Player::jumpDY;
        landed->m_hasLanded = false;
    }

    // Fall
    else if (!(landed->m_hasLanded) && velocity->dy < Player::maxFallDY)
        velocity->dy += World::gravity;
    else if (landed->m_hasLanded)
        velocity->dy = 0;
}


void CollisionSystem::Update() {

    collisionEntities = entityManager.GetCollisionEntities();

    std::shared_ptr<CTransform> tran = nullptr;
    std::shared_ptr<CCollisionState> coll = nullptr;
    std::shared_ptr<CLanded> land = nullptr;

    std::array<std::shared_ptr<Component>, 2> val {};

    for (Entity e = 0; e < World::MaxEntities; e++) {

        if (collisionEntities[e]) {

            coll = entityManager.GetComponent<CCollisionState>(e, ComponentID::cCollisionState);
            tran = entityManager.GetComponent<CTransform>(e, ComponentID::cTransform);
            land = entityManager.GetComponent<CLanded>(e, ComponentID::cLanded);

            coll->isCollidingLeft = false;
            coll->isCollidingRight = false;
            coll->isCollidingUp = false;
            coll->isCollidingDown = false;
            coll->horCollWith.clear();
            coll->vertCollWith.clear();

            val = {coll, tran};
            collisionComponentMap.insert({e, val});

            velocities[e] = entityManager.GetComponent<CVelocity>(e, ComponentID::cVelocity);

            if (land) {
                land->m_hasLanded = false;
            }
        }
    }

    
    CheckVerticalCollisions();
    
    if (anyVertCollisions)
        ResolveVerticalCollisions();
    
    CheckHorizontalCollisions();

    if (anyHorCollisions)
        ResolveHorizontalCollisions();
}


void CollisionSystem::CheckVerticalCollisions() {

    anyVertCollisions = false;
    
    std::shared_ptr<CCollisionState> coll1 = nullptr;
    std::shared_ptr<CCollisionState> coll2 = nullptr;
    std::shared_ptr<CTransform> tran1 = nullptr;
    std::shared_ptr<CTransform> tran2 = nullptr;

    // Do a triangular loop
    for (Entity i = 0; i < World::MaxEntities - 1; i++) {

        coll1 = std::dynamic_pointer_cast<CCollisionState>(collisionComponentMap[i][collIdx]);
        tran1 = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[i][tranIdx]);

        for (Entity j = i + 1; j < World::MaxEntities; j++) {

            if (!collisionEntities[j]) 
                continue;
                
            coll2 = std::dynamic_pointer_cast<CCollisionState>(collisionComponentMap[j][collIdx]);
            tran2 = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[j][tranIdx]);

            const SDL_Rect rect1 = tran1->m_rect;
            const SDL_Rect rect2 = tran2->m_rect;

            // If the 2 rectangles don't collide, move on
            if (!(SDL_HasIntersection(&rect1, &rect2)))
                continue;

            // Check collision direction
            bool a = (rect1.y < rect2.y && rect1.y + rect1.h > rect2.y); // i on top, j on bottom
            bool b = (rect2.y < rect1.y && rect2.y + rect2.h > rect1.y); // i on bottom, j on top

            if ((a || b) && !anyVertCollisions)
                anyVertCollisions = true;

            if (a) {

                coll1->isCollidingDown = true;
                coll2->isCollidingUp = true;

                coll1->vertCollWith.insert({j, Direction::Down});
                coll2->vertCollWith.insert({i, Direction::Up});
            }

            else if (b) {

                coll1->isCollidingUp = true;
                coll2->isCollidingDown = true;

                coll1->vertCollWith.insert({j, Direction::Up});
                coll2->vertCollWith.insert({i, Direction::Down});
            }
        }
    }
}


void CollisionSystem::CheckHorizontalCollisions() {

    anyHorCollisions = false;

    std::shared_ptr<CCollisionState> coll1 = nullptr;
    std::shared_ptr<CCollisionState> coll2 = nullptr;
    std::shared_ptr<CTransform> tran1 = nullptr;
    std::shared_ptr<CTransform> tran2 = nullptr;

    // TODO : Collisions based on tags
    
    // Do a triangular loop
    for (Entity i = 0; i < World::MaxEntities - 1; i++) {

        if (!collisionEntities[i])
            continue;

        coll1 = std::dynamic_pointer_cast<CCollisionState>(collisionComponentMap[i][collIdx]);
        tran1 = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[i][tranIdx]);

        for (Entity j = i + 1; j < World::MaxEntities; j++) {

            if (!collisionEntities[j])
                continue;

            coll2 = std::dynamic_pointer_cast<CCollisionState>(collisionComponentMap[j][collIdx]);
            tran2 = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[j][tranIdx]);

            const SDL_Rect rect1 = tran1->m_rect;
            const SDL_Rect rect2 = tran2->m_rect;

            // If the 2 rectangles don't collide, move on
            if (!(SDL_HasIntersection(&rect1, &rect2)))
                continue;

            // Work out whether there is horizontal collision
            bool a = (rect1.x < rect2.x && rect1.x + rect1.w > rect2.x); // i on left, j on right
            bool b = (rect2.x < rect1.x && rect2.x + rect2.w > rect1.x); // i on right, j on left 

            if ((a || b) && !anyHorCollisions)
                anyHorCollisions = true;

            if (a) {

                coll1->isCollidingRight = true;
                coll2->isCollidingLeft = true;

                coll1->horCollWith.insert({j, Direction::Right});
                coll2->horCollWith.insert({i, Direction::Left});
            }

            else if (b) {

                coll1->isCollidingLeft = true;
                coll2->isCollidingRight = true;

                coll1->horCollWith.insert({j, Direction::Left});
                coll2->horCollWith.insert({i, Direction::Right});
            }
        }
    }
}


void CollisionSystem::ResolveHorizontalCollisions() {

    std::shared_ptr<CTransform> leftTran = nullptr;
    std::shared_ptr<CTransform> rightTran = nullptr;

    // Move the object out of the way
    for (Entity i = 0; i < World::MaxEntities; i++) {

        if (!collisionEntities[i])
            continue;

        std::unordered_map<Entity, Direction> horColls = (std::dynamic_pointer_cast<CCollisionState>(collisionComponentMap[i][collIdx]))->horCollWith;

        // For each entity that Entity i is colliding with
        for (auto &ent : horColls) {

            Entity left = (ent.second == Direction::Right) ? i : ent.first;
            Entity right = (left == i) ? ent.first : i;

            leftTran = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[left][tranIdx]);
            rightTran = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[right][tranIdx]);

            // Fixed objects may not have a velocity component
            if (!velocities[right]) {    // Left object moving
                velocities[left]->dx = 0;
                leftTran->m_rect.x = rightTran->m_rect.x - leftTran->m_rect.w;
            }
            else if (!velocities[left]) {
                velocities[right]->dx = 0;
                rightTran->m_rect.x = leftTran->m_rect.x + leftTran->m_rect.w;
            }

            // No object moving left - glue left transform to right, set left dx to right dx
            else if (velocities[left]->dx > 0 && velocities[right]->dx >= 0) {
                leftTran->m_rect.x = rightTran->m_rect.x - leftTran->m_rect.w;
                velocities[left]->dx = velocities[right]->dx;
            }

            // No object moving right - glue right transform to left, set right dx to left dx 
            else if (velocities[right]->dx < 0 && velocities[left]->dx <= 0) {

                rightTran->m_rect.x = leftTran->m_rect.x + leftTran->m_rect.w;
                velocities[right]->dx = velocities[left]->dx;
            }

            // Both objects moving towards each other
            else if (velocities[left]->dx > 0 && velocities[right]->dx < 0) {
                
                velocities[left]->dx = 0;
                velocities[right]->dx = 0;

                // Move each object apart by half of the overlap
                int xDiff = leftTran->m_rect.x + leftTran->m_rect.w - rightTran->m_rect.x;

                // Add 1 to get the right separation if xDiff is odd
                if (xDiff % 2 != 0) {
                    xDiff += 1;
                }

                leftTran->m_rect.x -= (xDiff / 2);
                rightTran->m_rect.x += (xDiff / 2);
            }
        }
    }
}



void CollisionSystem::ResolveVerticalCollisions() {

    std::shared_ptr<CTransform> topTran = nullptr;
    std::shared_ptr<CTransform> bottomTran = nullptr;

    // Move the object out of the way
    for (Entity i = 0; i < World::MaxEntities; i++) {

        if (collisionEntities[i] && velocities[i]) {

            std::unordered_map<Entity, Direction> vertColls = (std::dynamic_pointer_cast<CCollisionState>(collisionComponentMap[i][collIdx]))->vertCollWith;
            
            // For each entity that Entity i is colliding with
            for (auto &ent : vertColls) {

                Entity top = (ent.second == Direction::Down) ? i : ent.first;
                Entity bottom = (top == i) ? ent.first : i;

                topTran = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[top][tranIdx]);
                bottomTran = std::dynamic_pointer_cast<CTransform>(collisionComponentMap[bottom][tranIdx]);

                std::shared_ptr<CLanded> land = entityManager.GetComponent<CLanded>(top, ComponentID::cLanded);

                // Falling object landed (if it has a landed component)
                if (land) {
                    land->m_hasLanded = true;
                }

                // Fixed objects may not have a velocity component
                if (!velocities[bottom]) {
                    velocities[top]->dy = 0;
                    topTran->m_rect.y = bottomTran->m_rect.y - topTran->m_rect.h;
                }
                else if (!velocities[top]) {
                    velocities[bottom]->dy = 0;
                    bottomTran->m_rect.y = topTran->m_rect.y + topTran->m_rect.h;
                }

                // Top object moving faster than bottom object (means moving down)
                else if (velocities[top]->dy > velocities[bottom]->dy) {
                    topTran->m_rect.y = bottomTran->m_rect.y - topTran->m_rect.h;
                    velocities[top]->dy = velocities[bottom]->dy;
                }

                // Bottom object moving faster upwards (more negatively, given since they collide) than top object
                else if (velocities[bottom]->dy < velocities[top]->dy) {
                    bottomTran->m_rect.y = topTran->m_rect.y + topTran->m_rect.h;
                    velocities[bottom]->dy = velocities[top]->dy;
                }

                // Both objects moving towards each other
                else if (velocities[top]->dy > 0 && velocities[bottom]->dy < 0) {
                    
                    velocities[top]->dy = 0;
                    velocities[bottom]->dy = 0;

                    // Move each object apart by half of the overlap
                    int yDiff = topTran->m_rect.y + topTran->m_rect.h - bottomTran->m_rect.y;

                    // Add 1 to get the right separation if xDiff is odd
                    if (yDiff % 2 != 0) {
                        yDiff += 1;
                    }

                    topTran->m_rect.y -= (yDiff / 2);
                    bottomTran->m_rect.y += (yDiff / 2);
                }
            }
        }
    }
}