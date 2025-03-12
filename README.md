This C++ project uses Entities, Components, and Systems to create a data-driven approach to creating a platformer game.

## Building
In order to build and run the code, change the directory to the root directory, then type the following commands in the terminal

    mkdir build && cd build
    cmake ../
    cmake --build .

Then to run the executable, type

    ./Platformer

## Overview
Each Entity is represented as an int16_t, and is essentially an index used to access a game item's specific components, contained within the EntityManager's entityComponentMap. 

Components allow different attributes - spritesheets, position, velocity, etc. to be added, deleted, and modified for each entity without the need for the multiple inheritance issues common with other systems.  

Systems are called on each frame of the game loop by the SystemManager, and control everything that happens in the game, from rendering (although currently this is slightly confused with the GUI class), to movement, user input, and collision detection and reaction. Each system accesses and modifies the components as appropriate. Physics appropriate to a 2D platformer game are included, allowing for example jumping, falling, landing, side-to-side movement, and being stopped by solid objects.

## Flexibility
This implementation uses smart pointers in order to simply add and remove components of different types with intuitive function calls within the Game class, which contains an EntityManager object
The user specifies an entity (either new or existing), creates or refers to a component, and adds the component to the entity.

    Entity e = entityManager.CreateEntity();  
    CVelocity vel(1, 2);  
    entityManager.AddComponent(e, vel);  

This allows the entityManager under the hood to ascertain what type of component is added, and add it to the correct maps, system vectors, etc. to be used and modified later.

Therefore the Game class is able to load a level (tilemap etc.) and easily generate entities with the correct components. Once the Systems have taken care of the game logic, the GUI reflects the changes for the player to see and interact with.

## Efficiency
The current format allows for small platformer games to be easily created by modifying the Game class. However, it has some major scaling drawbacks.  

The smart pointer approach, although it allows for easy adaptation to different levels and tilemaps in the Game class, does not allow for the efficient looping that arrays of physical components would allow, as each pointer to a component forces the CPU to look to another address in memory and therefore prevents looping over blocks of contiguous blocks of stack memory. Given that each system loops through the entities it is provided with, and extracts, casts, and acts upon the relevant components as required, this approach has fairly large performance ramifications.  

To avoid looping over entities which are irrelevant to a particular system, when components are added or removed from entities or when entities are deleted, the entityManager adds these entities to vectors specific to different systems, if they satisfy the necessary requirements. For example, entities are added to the moveEntities vector only when they have a transform component and a velocity component. Only these entities are passed to the MovementSystem, rather than all of the static entities as well. There is a slight overhead for doing this, but given that the systems operate 60 (or whatever the chosen FPS is) times per second and entities are added or removed from these vectors relatively infrequently, this overhead is minimal by comparison.

Another point which I have not quite worked out is how to efficiently use a QuadTree to divide the entities in the level up into different sections (nodes) in order to avoid the O(n^2) approach of classic collision detection. The programme does indeed implement a version of a QuadTree to divide entities into the appropriate node and only compare entities within that node, resulting in O(k^2) efficiency where k << n is the number of entities in a given node (carried out over each node which can still be quite high). However, the QuadTree appears to take a lot of iterations to update when there are more than a few mobile entities in the game.

## Next steps
There are a few annoying bugs and extra features to include (this list is not exhaustive):
- Collision issues when colliding with multiple objects simultaneously
- QuadTree efficiency
- Extra component options to include
- Enemies to add in which can give and take damage
- Coins (and therefore points)
