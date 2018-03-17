all: compile

compile:
	g++ -o main t2.c -lX11 -lGL -lGLU -I/usr/local/include/glm
