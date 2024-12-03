# Define variables 
#CC = g++			# Compiler
#CFLAGS = 
#CPPFILES = src/main.cpp src/Game.cpp src/GUI.cpp src/EntityManager.cpp src/Systems.cpp src/QuadTree.cpp src/QuadTreeNode.cpp
#OBJECTS = src/main.o src/Game.o src/GUI.o src/EntityManager.o src/Systems.o src/QuadTree.o src/QuadTreeNode.o

# Build rule - target on left, and dependencies on right
all:
	g++ -I include/ -L lib/ -o main src/main.cpp src/Game.cpp src/GUI.cpp src/EntityManager.cpp src/Systems.cpp src/QuadTree.cpp src/QuadTreeNode.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image

#%.o: %.cpp 
#	$(CC) -c $^ -o $@

#clean:
#	rm 