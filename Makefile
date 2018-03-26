all: compile

compile:
	g++ src/t3.cpp -o bin/t3 -lglfw -lGLEW -lGL
	g++ src/t2.cpp -o bin/t2 -lglfw -lGLEW -lGL
	g++ src/t1.cpp -o bin/t1 -lglfw -lGLEW -lGL
	g++ -Wall -o bin/car1 src/car1.cpp -lglut -lGLU -lGL 