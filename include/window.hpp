#include <GLFW/glfw3.h>
#include <bits/stdc++.h>

#ifndef WINDOW_H_
#define WINDOW_H_

class Window {
public:
    Window() {
        if (!glfwInit()) {
            fprintf(stderr, "failed to initialize glfw\n");
            exit(-1);
        }

        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow( 1024, 768, "Tutorial 02", NULL, NULL);
        if(!window){
            fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
            glfwTerminate();
            exit(-1);
        }

        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
        glfwMakeContextCurrent(window);

        glewExperimental = true;
        if (glewInit() != GLEW_OK) {
            fprintf(stderr, "Failed to initialize GLEW\n");
            exit(-1);
        }
    }

    bool shouldClose() {
        return glfwWindowShouldClose(window);
    }

    void swapBuffer() {
        glfwSwapBuffers(window);
    }

    void pollEvents() {
        glfwPollEvents();
    }

private:
    GLFWwindow* window;

    static void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
        glViewport(0, 0, width, height);
    }
};

#endif