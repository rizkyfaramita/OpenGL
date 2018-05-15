all: lighting

test:
	g++ common/shader.cpp common/texture.cpp common/controls.cpp src/main2.cpp -o bin/lighting -lglfw -lGLEW -lGL -lassimp -I src -g -std=c++11

lighting:
	g++ common/shader.cpp common/texture.cpp common/controls.cpp src/main.cpp -o bin/lighting -lglfw -lGLEW -lGL -lassimp -I src -g -std=c++11