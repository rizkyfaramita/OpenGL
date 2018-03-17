all: compile

compile:
	g++ -o main t2.cpp -lX11 -lGL -lGLU -I/usr/local/include/glm
