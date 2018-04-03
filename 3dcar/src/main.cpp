#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "mesh.hpp"
#include "shader.hpp"
#include "model.hpp"

using namespace std;

bool mouseClicked = false;
double clickedX = 0.0;
double clickedY = 0.0;
double rotateX = 0.0;
double rotateY = 0.0;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// screen settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Camera, Mouse, and Scroll
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

int main(int argc, char** argv) {
	// init window
    if (!glfwInit()) {
        fprintf(stderr, "failed to initialize glfw\n");
        exit(-1);
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow( SCR_WIDTH, SCR_HEIGHT, "3D Car", NULL, NULL);
    if(!window){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        glfwTerminate();
        exit(-1);
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);
    glfwMakeContextCurrent(window);

    glewExperimental = true;
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        exit(-1);
    }

    // init shaders
    Shader shader("src/vertex_shader.vs", "src/fragment_shader.fs");

    // enable depth buffer
    glEnable(GL_DEPTH_TEST);

    Model model("data/car.obj");

    while(!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // set view matrix
        glm::vec3 cameraPosition = glm::vec3(0.0f, 20.0f, 40.0f);
        glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        glm::vec3 cameraRight = (glm::cross(glm::vec3(-cameraPosition.x,0,-cameraPosition.z), glm::vec3(0,1,0)));
        glm::vec3 cameraUp = glm::cross(cameraRight, cameraDirection);

        // set | rotation
        glm::mat4 cameraRotationY = glm::rotate(glm::mat4(), (float) glm::radians((float) -rotateY/1.0), cameraUp);
        cameraPosition = glm::vec3(cameraRotationY * glm::vec4(cameraPosition, 1.0));
        cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraRight = glm::cross(cameraDirection, cameraUp);

        // set - rotation
        glm::mat4 cameraRotationX = glm::rotate(glm::mat4(), (float) glm::radians((float) -rotateX/1.0), cameraRight);
        cameraPosition = glm::vec3(cameraRotationX * glm::vec4(cameraPosition, 1.0));
        cameraDirection = glm::normalize(cameraTarget - cameraPosition);
        cameraUp = glm::cross(cameraRight, cameraDirection);

        glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget, cameraUp);

        // get ratio
        GLint vp[4];
        glGetIntegerv(GL_VIEWPORT, vp);
        float ratio = vp[2] * 1.0 / vp[3];

        // set projection matrix
        glm::mat4 projection = glm::perspective(glm::radians(30.0f), ratio, 0.1f, 100.0f);

        // set model matrix
        glm::mat4 rotation = glm::rotate(glm::mat4(), (float) glfwGetTime() * 0.0f, glm::vec3(0.0f, 1.0f, 0.0f));

        shader.use();
        shader.setMat4("projection", projection);
        shader.setMat4("view", view);
        shader.setMat4("model", rotation);
        model.Draw(shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseClicked = true;
        glfwGetCursorPos(window, &clickedX, &clickedY);
    }

    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mouseClicked = false;
    }
}

void mouseCallback(GLFWwindow* window, double xpos, double ypos) {
    static double offsetX = 0.0;
    static double offsetY = 0.0;
    if (mouseClicked) {
        rotateY = offsetY + xpos - clickedX;
        rotateX = offsetX + ypos - clickedY;
    } else {
        offsetX = rotateX;
        offsetY = rotateY;
    }
}

void keyboardCallback(GLFWwindow* window, double xpos, double ypos) {
    static double offsetX = 0.0;
    static double offsetY = 0.0;
    // // up button pressed
    // if (mouseClicked) {
    //     rotateY = offsetY + xpos - clickedX;
    //     rotateX = offsetX + ypos - clickedY;
    // // right button pressed
    // } else if () {
    //     offsetX = rotateX;
    //     offsetY = rotateY;
    // // bottom button pressed
    // } else if () {

    // // left button pressed
    // } else if () {
        
    // }
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    printf("Scroll is working\n");
}
