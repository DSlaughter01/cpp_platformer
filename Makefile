all:
	g++ -I include/ -L lib/ -o main src/main.cpp src/Game.cpp src/GUI.cpp src/EntityManager.cpp src/Systems.cpp src/Variables.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image