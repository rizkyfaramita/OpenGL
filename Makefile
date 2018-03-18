all: compile

compile:
	g++ src/t3.cpp -o bin/t3 -lglfw -lGLEW -lGL
