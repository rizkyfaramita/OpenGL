#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
#include <bits/stdc++.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
using namespace std;
using namespace glm;
GLFWwindow* window;

#include "../common/shader.hpp"
#include "../common/texture.hpp"
#include "../common/controls.hpp"
#include "../common/objloader.hpp"

#include "mesh.hpp"
#include "shader.hpp"
#include "model.hpp"

bool mouseClicked = false;
double clickedX = 0.0;
double clickedY = 0.0;
double rotateX = 0.0;
double rotateY = 0.0;
double posX = 0.0;
double posY = 0.0;
bool modelRotation = true;
float modelRotationAngle = 0.0;
float frontOffset = 0.0;
float light_temp = 0.0f;
float lum = 0.0f;

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseCallback(GLFWwindow* window, double xpos, double ypos);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);

// screen settings
const unsigned int SCR_WIDTH = 1024;
const unsigned int SCR_HEIGHT = 768;

// Camera, Mouse, and Scroll
float lastX = SCR_WIDTH;
float lastY = SCR_HEIGHT;



// CPU representation of a particle
struct Particle{
    glm::vec3 pos, speed;
    unsigned char r,g,b,a; // Color
    float size, angle, weight;
    float life; // Remaining life of the particle. if <0 : dead and unused.
    float cameradistance; // *Squared* distance to the camera. if dead : -1.0f

    bool operator<(const Particle& that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameradistance > that.cameradistance;
    }
};

// Rain Counter
const int MaxParticlesRain = 2000;
int CurrentParticlesRain = 2000;
int countParticlesRain = 0;
Particle ParticlesContainerRain[MaxParticlesRain];
int LastUsedParticleRain = 0;

// Smoke Counter
const int MaxParticlesSmoke = 100000;
Particle ParticlesContainerSmoke[MaxParticlesSmoke];
int LastUsedParticleSmoke = 0;

bool raining = false;
bool exhausting = false;

// Finds a Particle in ParticlesContainerRain which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticleRain(){

    for(int i=LastUsedParticleRain; i<MaxParticlesRain; i++){
        if (ParticlesContainerRain[i].life < 0){
            LastUsedParticleRain = i;
            return i;
        }
    }

    for(int i=0; i<LastUsedParticleRain; i++){
        if (ParticlesContainerRain[i].life < 0){
            LastUsedParticleRain = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void SortParticlesRain(){
    std::sort(&ParticlesContainerRain[0], &ParticlesContainerRain[MaxParticlesRain]);
}

// Finds a Particle in ParticlesContainerSmoke which isn't used yet.
// (i.e. life < 0);
int FindUnusedParticleSmoke(){

    for(int i=LastUsedParticleSmoke; i<MaxParticlesSmoke; i++){
        if (ParticlesContainerSmoke[i].life < 0){
            LastUsedParticleSmoke = i;
            return i;
        }
    }

    for(int i=0; i<LastUsedParticleSmoke; i++){
        if (ParticlesContainerSmoke[i].life < 0){
            LastUsedParticleSmoke = i;
            return i;
        }
    }

    return 0; // All particles are taken, override the first one
}

void SortParticlesSmoke(){
    std::sort(&ParticlesContainerSmoke[0], &ParticlesContainerSmoke[MaxParticlesSmoke]);
}

bool cmp(glm::vec3 a, glm::vec3 b){
    pair<pair<float,float>, float> A = make_pair(make_pair(a.x,a.y),a.z);
    pair<pair<float,float>, float> B = make_pair(make_pair(b.x,b.y),b.z);
    return A < B;
}

int main( void )
{
    // Initialise GLFW
    if( !glfwInit() )
    {
        fprintf( stderr, "Failed to initialize GLFW\n" );
        getchar();
        return -1;
    }

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_RESIZABLE,GL_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow( 1024, 768, "Tutorial 18 - Particules", NULL, NULL);
    if( window == NULL ){
        fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
        getchar();
        glfwTerminate();
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        fprintf(stderr, "Failed to initialize GLEW\n");
        getchar();
        glfwTerminate();
        return -1;
    }

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    // Hide the mouse and enable unlimited mouvement
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Set the mouse at the center of the screen
    glfwPollEvents();
    glfwSetCursorPos(window, 1024/2, 768/2);

    // Dark blue background
    glClearColor(0.0f, 0.0f, 0.4f, 0.0f);

    // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint VertexArrayIDRain;
    glGenVertexArrays(1, &VertexArrayIDRain);
    glBindVertexArray(VertexArrayIDRain);

    GLuint VertexArrayIDSmoke;
    glGenVertexArrays(1, &VertexArrayIDSmoke);
    glBindVertexArray(VertexArrayIDSmoke);


    // Create and compile our GLSL program from the shaders
    GLuint programID = LoadShaders( "src/Particle.vertexshader", "src/Particle.fragmentshader" );

    // Vertex shader
    GLuint CameraRight_worldspace_ID  = glGetUniformLocation(programID, "CameraRight_worldspace");
    GLuint CameraUp_worldspace_ID  = glGetUniformLocation(programID, "CameraUp_worldspace");
    GLuint ViewProjMatrixID = glGetUniformLocation(programID, "VP");

    // fragment shader
    GLuint TextureID  = glGetUniformLocation(programID, "myTextureSampler");

    static GLfloat* g_particule_position_size_data_rain = new GLfloat[MaxParticlesRain * 4];
    static GLubyte* g_particule_color_data_rain         = new GLubyte[MaxParticlesRain * 4];

    for (int i = 0; i < MaxParticlesRain; i++){
        ParticlesContainerRain[i].life = -1.0f;
        ParticlesContainerRain[i].cameradistance = -1.0f;
    }

    GLuint TextureRain = loadDDS("src/particle.DDS");

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data_rain[] = { 
         -0.2f, -3.0f, 0.0f,
          0.2f, -3.0f, 0.0f,
         -0.2f,  3.0f, 0.0f,
          0.2f,  3.0f, 0.0f,
    };
    GLuint billboard_vertex_buffer_rain;
    glGenBuffers(1, &billboard_vertex_buffer_rain);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer_rain);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_rain), g_vertex_buffer_data_rain, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer_rain;
    glGenBuffers(1, &particles_position_buffer_rain);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer_rain);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticlesRain * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    GLuint particles_color_buffer_rain;
    glGenBuffers(1, &particles_color_buffer_rain);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer_rain);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticlesRain * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);
    
    static GLfloat* g_particule_position_size_data_smoke = new GLfloat[MaxParticlesSmoke * 4];
    static GLubyte* g_particule_color_data_smoke         = new GLubyte[MaxParticlesSmoke * 4];

    for (int i = 0; i < MaxParticlesSmoke; i++){
        ParticlesContainerSmoke[i].life = -1.0f;
        ParticlesContainerSmoke[i].cameradistance = -1.0f;
    }

    GLuint TextureSmoke = loadDDS("src/smoke.dds");

    // The VBO containing the 4 vertices of the particles.
    // Thanks to instancing, they will be shared by all particles.
    static const GLfloat g_vertex_buffer_data_smoke[] = { 
         -0.5f, -0.5f, 0.0f,
          0.5f, -0.5f, 0.0f,
         -0.5f,  0.5f, 0.0f,
          0.5f,  0.5f, 0.0f,
    };
    GLuint billboard_vertex_buffer_smoke;
    glGenBuffers(1, &billboard_vertex_buffer_smoke);
    glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer_smoke);
    glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data_smoke), g_vertex_buffer_data_smoke, GL_STATIC_DRAW);

    // The VBO containing the positions and sizes of the particles
    GLuint particles_position_buffer_smoke;
    glGenBuffers(1, &particles_position_buffer_smoke);
    glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer_smoke);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticlesSmoke * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

    // The VBO containing the colors of the particles
    GLuint particles_color_buffer_smoke;
    glGenBuffers(1, &particles_color_buffer_smoke);
    glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer_smoke);
    // Initialize with empty (NULL) buffer : it will be updated later, each frame.
    glBufferData(GL_ARRAY_BUFFER, MaxParticlesSmoke * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW);


    // Load the texture
    GLuint TextureCar = loadDDS("src/car.DDS");
    
    // Read our .obj file for car
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec2> uvs;
    std::vector<glm::vec3> normals;
    bool res = loadOBJ("data/car.obj", vertices, uvs, normals);
    if (!res){
        puts("Error found, image not found");
        return -1;
    }


    // ParticlesContainerSmoke[particleIndexSmoke].pos = glm::vec3(-5.0f,-3.0f,-18.0f);
    // find left most first;

    for (auto& it : vertices){
        it += glm::vec3(-3, 3, 0);
        swap(it.x, it.z);
        it *= 0.2f;
        it += glm::vec3(-2.35, 3.45f, -17.5);
    }
    // sort(vertices.begin(), vertices.end(), cmp);
    glm::vec3 leftmost;
    bool visited = 0;
    for (auto& it : vertices){
        cout << it.x << " " << it.y << " " << it.z << endl;
        if (!visited){
            visited = 1;
            leftmost = it;
        }
        if (leftmost.x > it.x){
            leftmost = it;
        } else if (leftmost.x == it.x && leftmost.y > it.y){
            leftmost = it;
        } else if (leftmost.x == it.x && leftmost.y == it.y && leftmost.z > it.z){
            leftmost = it;
        }
    }

    // Translate
    // glm::vec3 translate = glm::vec3(-5.0f,-3.0f,-18.0f) - leftmost;
    // for (auto& it : vertices){
    //     it += translate;
    // }

    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

    GLuint uvbuffer;
    glGenBuffers(1, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);


    double lastTime = glfwGetTime();
    bool pressed;
    float up, right;
    glm::vec3 moved;
    do {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        pressed = 1;
        right = up = 0;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS){
            ++up;
        } else if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS){
            --right;
        } else if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS){
            ++right;
        } else if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS){
            --up;
        } else {
            pressed = 0;
        }

        if (pressed){
            moved = glm::vec3(right/100.0f, up/100.0f, 0);
            for (auto& it : vertices){
                it += moved;
            }
            glGenBuffers(1, &vertexbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
        }

        double currentTime = glfwGetTime();
        double delta = currentTime - lastTime;
        lastTime = currentTime;


        computeMatricesFromInputs();
        glm::mat4 ProjectionMatrix = getProjectionMatrix();
        glm::mat4 ViewMatrix = getViewMatrix();

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureCar);
        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
            0,                  // attribute
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );

        // 2nd attribute buffer : UVs
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
        glVertexAttribPointer(
            1,                                // attribute
            2,                                // size
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // Draw the triangle !
        glDrawArrays(GL_TRIANGLES, 0, vertices.size() );




        // We will need the camera's position in order to sort the particles
        // w.r.t the camera's distance.
        // There should be a getCameraPosition() function in common/controls.cpp, 
        // but this works too.
        glm::vec3 CameraPosition(glm::inverse(ViewMatrix)[3]);

        glm::mat4 ViewProjectionMatrix = ProjectionMatrix * ViewMatrix;


        // Generate 10 new particule each millisecond,
        // but limit this to 16 ms (60 fps), or if you have 1 long frame (1sec),
        // newparticlesRain will be huge and the next frame even longer.
        int newparticlesRain = CurrentParticlesRain/ (int)(3.5f/delta);
        if (newparticlesRain > (int)(0.016f*10000.0))
            newparticlesRain = (int)(0.016f*10000.0);
        if (!raining) newparticlesRain = 0;
        
        for (int i = 0; i < newparticlesRain; i++){
            int particleIndexRain = FindUnusedParticleRain();
            ParticlesContainerRain[particleIndexRain].life = 3.5f; // This particle will live 5 seconds.
            ParticlesContainerRain[particleIndexRain].pos = glm::vec3((rand()%2000)*0.01f-10.0f,15.0f,-20.0f);

            float spread = 1.5f;
            glm::vec3 maindirRain = glm::vec3(0.0f, 2.0f, 0.0f);
            // Very bad way to generate a random direction; 
            // See for instance http://stackoverflow.com/questions/5408276/python-uniform-spherical-distribution instead,
            // combined with some user-controlled parameters (main direction, spread, etc)
            glm::vec3 randomdirRain = glm::vec3(
                -0.5f,
                (rand()%2000 - 1000.0f)/1000.0f,
                0.5f
            );
            
            ParticlesContainerRain[particleIndexRain].speed = maindirRain + randomdirRain;

            // Very bad way to generate a random color
            ParticlesContainerRain[particleIndexRain].r = 255;
            ParticlesContainerRain[particleIndexRain].g = 255;
            ParticlesContainerRain[particleIndexRain].b = 255;
            ParticlesContainerRain[particleIndexRain].a = (rand() % 256) / 3;

            ParticlesContainerRain[particleIndexRain].size = 0.1f;
            
        }

        // Simulate all particles
        int ParticlesCountRain = 0;
        for(int i = 0; i<MaxParticlesRain; i++){

            Particle& p = ParticlesContainerRain[i]; // shortcut

            if(p.life > 0.0f){

                // Decrease life
                p.life -= delta;
                if (p.life > 0.0f){

                    // Simulate simple physics : gravity only, no collisions
                    p.speed += glm::vec3(0.0f,-9.81f, 0.0f) * (float)delta * 0.5f;
                    p.pos += p.speed * (float)delta;
                    p.cameradistance = glm::length2( p.pos - CameraPosition );
                    //ParticlesContainerRain[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                    // Fill the GPU buffer
                    g_particule_position_size_data_rain[4*ParticlesCountRain+0] = p.pos.x;
                    g_particule_position_size_data_rain[4*ParticlesCountRain+1] = p.pos.y;
                    g_particule_position_size_data_rain[4*ParticlesCountRain+2] = p.pos.z;
                                                   
                    g_particule_position_size_data_rain[4*ParticlesCountRain+3] = p.size;
                                                   
                    g_particule_color_data_rain[4*ParticlesCountRain+0] = p.r;
                    g_particule_color_data_rain[4*ParticlesCountRain+1] = p.g;
                    g_particule_color_data_rain[4*ParticlesCountRain+2] = p.b;
                    g_particule_color_data_rain[4*ParticlesCountRain+3] = p.a;

                }else{
                    // Particles that just died will be put at the end of the buffer in SortParticlesRain();
                    p.cameradistance = -1.0f;
                }

                ParticlesCountRain++;

            }
        }

        SortParticlesRain();


        //printf("%d ",ParticlesCountRain);


        // Update the buffers that OpenGL uses for rendering.
        // There are much more sophisticated means to stream data from the CPU to the GPU, 
        // but this is outside the scope of this tutorial.
        // http://www.opengl.org/wiki/Buffer_Object_Streaming


        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer_rain);
        glBufferData(GL_ARRAY_BUFFER, MaxParticlesRain * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCountRain * sizeof(GLfloat) * 4, g_particule_position_size_data_rain);

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer_rain);
        glBufferData(GL_ARRAY_BUFFER, MaxParticlesRain * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCountRain * sizeof(GLubyte) * 4, g_particule_color_data_rain);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Use our shader
        glUseProgram(programID);

        // Bind our texture in Texture Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureRain);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

        glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer_rain);
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        
        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer_rain);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : x + y + z + size => 4
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer_rain);
        glVertexAttribPointer(
            2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : r + g + b + a => 4
            GL_UNSIGNED_BYTE,                 // type
            GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

        // Draw the particules !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCountRain) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
        // but faster.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCountRain);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        countParticlesRain += newparticlesRain;
        if (countParticlesRain >= CurrentParticlesRain){
            CurrentParticlesRain = rand()%2001;
        }    
         
        int newparticlesSmoke = (int)(delta*10000.0);
        if (newparticlesSmoke > (int)(0.016f*10000.0))
            newparticlesSmoke = (int)(0.016f*10000.0);
        if (!exhausting) newparticlesSmoke = 0;
        
        for (int i = 0; i < newparticlesSmoke; i++){
            int particleIndexSmoke = FindUnusedParticleSmoke();
            ParticlesContainerSmoke[particleIndexSmoke].life = 2.0f; // This particle will live 5 seconds.
            ParticlesContainerSmoke[particleIndexSmoke].pos = glm::vec3(-5.0f,-3.0f,-18.0f);

            float spread = 1.5f;
            glm::vec3 maindirSmoke = glm::vec3(10.0f, 0.0f, 0.0f);
            glm::vec3 randomdirSmoke = glm::vec3(
                (rand()%2000 - 1000.0f)/1000.0f,
                (rand()%2000 - 1000.0f)/1000.0f,
                (rand()%2000 - 1000.0f)/1000.0f
            );
            
            ParticlesContainerSmoke[particleIndexSmoke].speed = maindirSmoke + randomdirSmoke*spread;

            // Very bad way to generate a random color
            ParticlesContainerSmoke[particleIndexSmoke].r = 255;
            ParticlesContainerSmoke[particleIndexSmoke].g = 255;
            ParticlesContainerSmoke[particleIndexSmoke].b = 255;
            ParticlesContainerSmoke[particleIndexSmoke].a = (rand() % 256) / 3;

            ParticlesContainerSmoke[particleIndexSmoke].size = (rand()%1000)/2000.0f + 0.1f;
            
        }

        // Simulate all particles
        int ParticlesCountSmoke = 0;
        for(int i = 0; i<MaxParticlesSmoke; i++){

            Particle& p = ParticlesContainerSmoke[i]; // shortcut

            if(p.life > 0.0f){

                // Decrease life
                p.life -= delta;
                if (p.life > 0.0f){

                    // Simulate simple physics : gravity only, no collisions
                    p.speed += glm::vec3(0.0f, 0.0f, 0.0f) * (float)delta * 0.5f;
                    p.pos -= p.speed * (float)delta;
                    p.cameradistance = glm::length2( p.pos - CameraPosition );
                    //ParticlesContainerSmoke[i].pos += glm::vec3(0.0f,10.0f, 0.0f) * (float)delta;

                    // Fill the GPU buffer
                    g_particule_position_size_data_smoke[4*ParticlesCountSmoke+0] = p.pos.x;
                    g_particule_position_size_data_smoke[4*ParticlesCountSmoke+1] = p.pos.y;
                    g_particule_position_size_data_smoke[4*ParticlesCountSmoke+2] = p.pos.z;
                                                   
                    g_particule_position_size_data_smoke[4*ParticlesCountSmoke+3] = p.size;
                                                   
                    g_particule_color_data_smoke[4*ParticlesCountSmoke+0] = p.r;
                    g_particule_color_data_smoke[4*ParticlesCountSmoke+1] = p.g;
                    g_particule_color_data_smoke[4*ParticlesCountSmoke+2] = p.b;
                    g_particule_color_data_smoke[4*ParticlesCountSmoke+3] = p.a;

                }else{
                    // Particles that just died will be put at the end of the buffer in SortParticlesSmoke();
                    p.cameradistance = -1.0f;
                }

                ParticlesCountSmoke++;

            }
        }

        SortParticlesSmoke();


        //printf("%d ",ParticlesCountSmoke);


        // Update the buffers that OpenGL uses for rendering.
        // There are much more sophisticated means to stream data from the CPU to the GPU, 
        // but this is outside the scope of this tutorial.
        // http://www.opengl.org/wiki/Buffer_Object_Streaming


        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer_smoke);
        glBufferData(GL_ARRAY_BUFFER, MaxParticlesSmoke * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCountSmoke * sizeof(GLfloat) * 4, g_particule_position_size_data_smoke);

        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer_smoke);
        glBufferData(GL_ARRAY_BUFFER, MaxParticlesSmoke * 4 * sizeof(GLubyte), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, ParticlesCountSmoke * sizeof(GLubyte) * 4, g_particule_color_data_smoke);

        // Use our shader
        glUseProgram(programID);

        // Bind our texture in TextureSmoke Unit 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, TextureSmoke);
        // Set our "myTextureSampler" sampler to use Texture Unit 0
        glUniform1i(TextureID, 0);

        // Same as the billboards tutorial
        glUniform3f(CameraRight_worldspace_ID, ViewMatrix[0][0], ViewMatrix[1][0], ViewMatrix[2][0]);
        glUniform3f(CameraUp_worldspace_ID   , ViewMatrix[0][1], ViewMatrix[1][1], ViewMatrix[2][1]);

        glUniformMatrix4fv(ViewProjMatrixID, 1, GL_FALSE, &ViewProjectionMatrix[0][0]);

        // 1rst attribute buffer : vertices
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer_smoke);
        glVertexAttribPointer(
            0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
            3,                  // size
            GL_FLOAT,           // type
            GL_FALSE,           // normalized?
            0,                  // stride
            (void*)0            // array buffer offset
        );
        
        // 2nd attribute buffer : positions of particles' centers
        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer_smoke);
        glVertexAttribPointer(
            1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : x + y + z + size => 4
            GL_FLOAT,                         // type
            GL_FALSE,                         // normalized?
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // 3rd attribute buffer : particles' colors
        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer_smoke);
        glVertexAttribPointer(
            2,                                // attribute. No particular reason for 1, but must match the layout in the shader.
            4,                                // size : r + g + b + a => 4
            GL_UNSIGNED_BYTE,                 // type
            GL_TRUE,                          // normalized?    *** YES, this means that the unsigned char[4] will be accessible with a vec4 (floats) in the shader ***
            0,                                // stride
            (void*)0                          // array buffer offset
        );

        // These functions are specific to glDrawArrays*Instanced*.
        // The first parameter is the attribute buffer we're talking about.
        // The second parameter is the "rate at which generic vertex attributes advance when rendering multiple instances"
        // http://www.opengl.org/sdk/docs/man/xhtml/glVertexAttribDivisor.xml
        glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1
        glVertexAttribDivisor(2, 1); // color : one per quad                                  -> 1

        // Draw the particules !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCountSmoke) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4), 
        // but faster.
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ParticlesCountSmoke);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } // Check if the ESC key was pressed or the window was closed
    while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
           glfwWindowShouldClose(window) == 0 );


    delete[] g_particule_position_size_data_rain;
    delete[] g_particule_position_size_data_smoke;

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer_rain);
    glDeleteBuffers(1, &particles_position_buffer_rain);
    glDeleteBuffers(1, &billboard_vertex_buffer_rain);
    glDeleteProgram(programID);
    glDeleteTextures(1, &TextureRain);
    glDeleteVertexArrays(1, &VertexArrayIDRain);

    // Cleanup VBO and shader
    glDeleteBuffers(1, &particles_color_buffer_smoke);
    glDeleteBuffers(1, &particles_position_buffer_smoke);
    glDeleteBuffers(1, &billboard_vertex_buffer_smoke);
    glDeleteTextures(1, &TextureSmoke);
    glDeleteVertexArrays(1, &VertexArrayIDSmoke);
    

    // Close OpenGL window and terminate GLFW
    glfwTerminate();

    return 0;
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
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
    if (mouseClicked) {
        rotateY = xpos - clickedX;
        rotateX = ypos - clickedY;
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static bool up_pressed = false;
    static bool right_pressed = false;
    static bool down_pressed = false;
    static bool left_pressed = false;
    static bool enter_pressed = false;
    static bool w_pressed = false;
    static bool d_pressed = false;
    static bool s_pressed = false;
    static bool r_pressed = false;

    if (key == GLFW_KEY_UP)
        up_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : up_pressed);
    if (key == GLFW_KEY_RIGHT)
        right_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : right_pressed);
    if (key == GLFW_KEY_DOWN)
        down_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : down_pressed);
    if (key == GLFW_KEY_LEFT)
        left_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : left_pressed);
    if (key == GLFW_KEY_W)
        w_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : w_pressed);
    if (key == GLFW_KEY_D)
        d_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : d_pressed);
    if (key == GLFW_KEY_S)
        s_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : s_pressed);
    if (key == GLFW_KEY_R)
        r_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : r_pressed);
    
    if (up_pressed)
        rotateX -= 2.0;
    if (right_pressed)
        rotateY += 2.0;
    if (down_pressed)
        rotateX += 2.0;
    if (left_pressed)
        rotateY -= 2.0;

    if (action == GLFW_PRESS)
        modelRotation = !modelRotation;

    if (key == GLFW_KEY_ENTER)
        enter_pressed = action == GLFW_PRESS ? 1 : (action == GLFW_RELEASE ? 0 : enter_pressed);

    if (enter_pressed || w_pressed){
        light_temp = lum;
        lum += 1;
        if (lum >= 10.f){
            lum = 10.f;
        }
    } 
    if (d_pressed){
        light_temp = lum;
        lum -= 1;
        if (lum <= 0.f){
            lum = 0.f;
        }
    }
    if (s_pressed)
        exhausting = !exhausting;
    if (r_pressed)
        raining = !raining;
}
