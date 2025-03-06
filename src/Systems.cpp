#include "Systems.hpp" 

void RenderSystem::Update(SDL_Renderer* ren, std::vector<SDL_Texture*> &textureVec, int &xOffset) {

    renderEntities = entityManager.GetRenderEntities();

    std::shared_ptr<CTransform> transform = nullptr;
    std::shared_ptr<CVelocity> velocity = nullptr;
    std::shared_ptr<CSpritesheet> spritesheet = nullptr;

    // To refactor after - update xOffset according to player position
    Entity player = entityManager.GetPlayerEntity();
    transform = entityManager.GetComponent<CTransform>(player, ComponentID::cTransform);

    int diff {};

    if (transform->m_rect.x + transform->m_rect.w - xOffset > World::rightScroll &&
        transform->m_rect.x + transform->m_rect.w < World::levelWidth - (World::WindowWidth - World::rightScroll)) {

        diff = World::rightScroll - (transform->m_rect.x + transform->m_rect.w - xOffset);
        xOffset -= diff;
    }

    else if (transform->m_rect.x - xOffset < World::leftScroll &&
             transform->m_rect.x > World::leftScroll) {

        diff = World::leftScroll - (transform->m_rect.x - xOffset);
        xOffset -= diff;
    }

    transform = nullptr;

    for (Entity e = 0; e < renderEntities.size(); e++) {

        spritesheet = entityManager.GetComponent<CSpritesheet>(renderEntities[e], ComponentID::cSpritesheet);
        transform = entityManager.GetComponent<CTransform>(renderEntities[e], ComponentID::cTransform);

        // Render to the screen
        if (spritesheet && transform) {   

            SDL_RendererFlip flipVal = (spritesheet->m_dir == spritesheet->m_originalDir) ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL;

            SDL_Rect src = {spritesheet->m_currentImageX, 0, spritesheet->m_imageWidth, spritesheet->m_imageHeight};
            SDL_Rect dest = {transform->m_rect.x - xOffset, transform->m_rect.y, transform->m_rect.w, transform->m_rect.h};
            SDL_RenderCopyEx(ren, textureVec[spritesheet->m_spritesheetID], &src, &dest, 0, NULL, flipVal);

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


void MovementSystem::Update() {

    moveEntities = entityManager.GetMoveEntities();

    // Get the transform and velocity components
    std::shared_ptr<CTransform> transform = nullptr;
    std::shared_ptr<CVelocity> velocity = nullptr;
    std::shared_ptr<CSpritesheet> spritesheet = nullptr;

    // Move the transform components by the velocity
    for (Entity e = 0; e < moveEntities.size(); e++) {

        transform = entityManager.GetComponent<CTransform>(moveEntities[e], ComponentID::cTransform);
        velocity = entityManager.GetComponent<CVelocity>(moveEntities[e], ComponentID::cVelocity);
        spritesheet = entityManager.GetComponent<CSpritesheet>(moveEntities[e], ComponentID::cSpritesheet);

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


void InputSystem::Update(const Uint8* keyboard) {

    Entity player = entityManager.GetPlayerEntity();

    if (player == World::InvalidEntity)
        return;

    std::shared_ptr<CVelocity> velocity = entityManager.GetComponent<CVelocity>(player, ComponentID::cVelocity);
    std::shared_ptr<CTransform> transform = entityManager.GetComponent<CTransform>(player, ComponentID::cTransform);
    std::shared_ptr<CLanded> landed = entityManager.GetComponent<CLanded>(player, ComponentID::cLanded);

    // std::cout << transform->m_rect.x << ", " << transform->m_rect.y << ", " << transform->m_rect.w << ", " << transform->m_rect.h << std::endl; 
    if (!velocity) {
        std::cout << "Player doesn't have a velocity component" << std::endl;
        return;
    } 

    // Move horizontally if uninhibited by other collidable objects
    if (keyboard[SDL_SCANCODE_LEFT] && (transform->m_rect.x >= Player::maxDX)) {
        velocity->dx = -Player::maxDX;
    }

    else if (keyboard[SDL_SCANCODE_RIGHT] && transform->m_rect.x + transform->m_rect.w <= World::levelWidth - Player::maxDX) {
        velocity->dx = Player::maxDX;
    }
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

    std::vector<Entity> createdEntities = entityManager.GetNewCollisionEntities();
    std::vector<Entity> deletedEntities = entityManager.GetRemovedCollisionEntities();

    quadTree.Update(createdEntities, deletedEntities, collisionEntities);
    leafNodes = quadTree.GetLeafNodes();

    std::shared_ptr<CLanded> land = nullptr;

    for (Entity e = 0; e < collisionEntities.size(); e++) {

        velocities[e] = entityManager.GetComponent<CVelocity>(e, ComponentID::cVelocity);

        land = entityManager.GetComponent<CLanded>(e, ComponentID::cLanded);
        if (land)
            land->m_hasLanded = false;
    } 
 
    collidingPairs = BroadPass();

    if (collidingPairs.size() > 0) {
        std::vector<CollisionSystem::EntityPair> vertCollidingPairs = CheckVerticalCollisions(collidingPairs);
        
        if (!vertCollidingPairs.empty()) {
            ResolveVerticalCollisions(vertCollidingPairs);
        }
    }
    
    collidingPairs = BroadPass();

    if (!collidingPairs.empty()) {
        std::vector<CollisionSystem::EntityPair> horCollidingPairs = CheckHorizontalCollisions(collidingPairs);
        if (horCollidingPairs.size() > 0) {
            ResolveHorizontalCollisions(horCollidingPairs);
        }
    } 
}


std::vector<CollisionSystem::EntityPair> CollisionSystem::BroadPass() {

    std::vector<CollisionSystem::EntityPair> collidingPairs {};

    std::shared_ptr<CTransform> tran1 = nullptr;
    std::shared_ptr<CTransform> tran2 = nullptr;

    for (auto &node : leafNodes) {

        std::vector<Entity> entitiesInNode = node->GetEntities();

        if (entitiesInNode.size() == 0)
            continue;

        for (int i = 0; i < entitiesInNode.size() - 1; i++) {

            tran1 = entityManager.GetComponent<CTransform>(entitiesInNode[i], ComponentID::cTransform);

            for (int j = i + 1; j < entitiesInNode.size(); j++) {
                    
                tran2 = entityManager.GetComponent<CTransform>(entitiesInNode[j], ComponentID::cTransform);

                const SDL_Rect rect1 = tran1->m_rect;
                const SDL_Rect rect2 = tran2->m_rect;

                // If the 2 rectangles don't collide, move on
                if (SDL_HasIntersection(&rect1, &rect2)) {
                    CollisionSystem::EntityPair newPair = {entitiesInNode[i], entitiesInNode[j]};
                    collidingPairs.emplace_back(newPair);
                }
            }
        }
    }

    return collidingPairs;
}


std::vector<CollisionSystem::EntityPair> CollisionSystem::CheckVerticalCollisions(std::vector<EntityPair>& collidingPairs) {

    std::vector<CollisionSystem::EntityPair> vertCollidingPairs = {};
    
    std::shared_ptr<CTransform> tran1 = nullptr;
    std::shared_ptr<CTransform> tran2 = nullptr;

    // Do a triangular loop
    for (auto &node : leafNodes) {

        std::vector<Entity> entitiesInNode = node->GetEntities();

        for (int i = 0; i < entitiesInNode.size(); i++) {

            if (!entitiesInNode[i]) 
                continue;
            
            tran1 = entityManager.GetComponent<CTransform>(entitiesInNode[i], ComponentID::cTransform);

            for (int j = i + 1; j < entitiesInNode.size(); j++) {

                if (!entitiesInNode[j]) 
                    continue;
                    
                tran2 = entityManager.GetComponent<CTransform>(entitiesInNode[j], ComponentID::cTransform);

                const SDL_Rect rect1 = tran1->m_rect;
                const SDL_Rect rect2 = tran2->m_rect;

                // Check collision direction
                bool a = (rect1.y < rect2.y && rect1.y + rect1.h > rect2.y); // i on top, j on bottom
                bool b = (rect2.y < rect1.y && rect2.y + rect2.h > rect1.y); // i on bottom, j on top

                // i on top, j on bottom
                if (a) {
                    CollisionSystem::EntityPair newPair = {entitiesInNode[i], entitiesInNode[j]};
                    vertCollidingPairs.emplace_back(newPair);
                }

                // j on top, i on bottom
                else if (b) {
                    CollisionSystem::EntityPair newPair = {entitiesInNode[j], entitiesInNode[i]};
                    vertCollidingPairs.emplace_back(newPair);
                }
            }
        }
    }
    return vertCollidingPairs;
}


std::vector<CollisionSystem::EntityPair> CollisionSystem::CheckHorizontalCollisions(std::vector<EntityPair>& collidingPairs) {

    std::vector<CollisionSystem::EntityPair> horCollidingPairs {};
    std::shared_ptr<CTransform> tran1 = nullptr;
    std::shared_ptr<CTransform> tran2 = nullptr;
    
    // Triangular loop on each leaf node in the quad tree
    for (auto &node : leafNodes) {

        std::vector<Entity> entitiesInNode = node->GetEntities();

        for (int i = 0; i < entitiesInNode.size(); i++) {

            if (!entitiesInNode[i])
                continue;

            tran1 = entityManager.GetComponent<CTransform>(entitiesInNode[i], ComponentID::cTransform);

            for (int j = i + 1; j < entitiesInNode.size(); j++) {

                if (!entitiesInNode[j])
                    continue;
                
                tran2 = entityManager.GetComponent<CTransform>(entitiesInNode[j], ComponentID::cTransform);

                const SDL_Rect rect1 = tran1->m_rect;
                const SDL_Rect rect2 = tran2->m_rect;

                bool a = (rect1.x < rect2.x && rect1.x + rect1.w > rect2.x); // i on left, j on right
                bool b = (rect2.x < rect1.x && rect2.x + rect2.w > rect1.x); // i on right, j on left

                // i on left, j on right
                if (a) {
                    CollisionSystem::EntityPair newPair = {entitiesInNode[i], entitiesInNode[j]};
                    horCollidingPairs.emplace_back(newPair);
                } 
                
                // j on left, i on right
                else if (b) {
                    CollisionSystem::EntityPair newPair = {entitiesInNode[j], entitiesInNode[i]};
                    horCollidingPairs.emplace_back(newPair);
                }
            }
        }
    }
    return horCollidingPairs;
}


void CollisionSystem::ResolveHorizontalCollisions(std::vector<EntityPair>& collidingPairs) {

    std::shared_ptr<CTransform> leftTran = nullptr;
    std::shared_ptr<CTransform> rightTran = nullptr;

    // Move the object out of the way
    for (auto &pair : collidingPairs) {

        Entity left = pair.e1;
        Entity right = pair.e2;

        leftTran = entityManager.GetComponent<CTransform>(left, ComponentID::cTransform);
        rightTran = entityManager.GetComponent<CTransform>(right, ComponentID::cTransform);

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


void CollisionSystem::ResolveVerticalCollisions(std::vector<EntityPair>& collidingPairs) {

    std::shared_ptr<CTransform> topTran = nullptr;
    std::shared_ptr<CTransform> bottomTran = nullptr;

    // Move the object out of the way
    for (auto &pair : collidingPairs) {

        Entity top = pair.e1;
        Entity bottom = pair.e2;

        topTran = entityManager.GetComponent<CTransform>(top, ComponentID::cTransform);
        bottomTran = entityManager.GetComponent<CTransform>(bottom, ComponentID::cTransform);

        std::shared_ptr<CLanded> land = entityManager.GetComponent<CLanded>(top, ComponentID::cLanded);

        // Falling object landed (if it has a landed component)
        if (land)
            land->m_hasLanded = true;

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
