// Include standard headers
#include <bits/stdc++.h>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
using namespace glm;

#include <common/shader.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
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
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   // Open a window and create its OpenGL context
   window = glfwCreateWindow( 1024, 768, "T2 (One-coloured Triangle)", NULL, NULL);
   if( window == NULL ){
      fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
      getchar();
      glfwTerminate();
      return -1;
   }
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

   // White background
   glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

   GLuint VertexArrayID;
   glGenVertexArrays(1, &VertexArrayID);
   glBindVertexArray(VertexArrayID);

   // Create and compile our GLSL program from the shaders
   // GLuint programID = LoadShaders("SimpleVertexShader.vertexshader", "SimpleFragmentShader.fragmentshader" );
   GLuint programID = glCreateShader(GL_VERTEX_SHADER);
   static const GLfloat g_vertex_buffer_data[] = { 
      -0.75f, -0.75f, 0.0f,
       0.75f, -0.75f, 0.0f,
       0.0f,  0.75f, 0.0f,
   };

    const char* vertexShaderSourceCode = "\n"
    "    #version 330 core\n"
    "    layout (location = 0) in vec3 aPos;\n"
    "    void main() {\n"
    "        gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "    }\n";
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSourceCode, NULL);
    glCompileShader(vertexShader);
    const char* fragmentShaderSourceCode = "\n"
    "    #version 330 core\n"
    "   out vec4 FragColor;\n"
    "   void main() {\n"
    "       FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "   }\n";

   GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
   glShaderSource(fragmentShader, 1, &fragmentShaderSourceCode, NULL);
   glCompileShader(fragmentShader);
   GLuint shaderProgram = glCreateProgram();
   glAttachShader(shaderProgram, vertexShader);
   glAttachShader(shaderProgram, fragmentShader);
   glLinkProgram(shaderProgram);

   GLuint vertexbuffer;
   glGenBuffers(1, &vertexbuffer);
   GLuint vertexarray;
   glGenVertexArrays(1, &vertexarray);
   glBindVertexArray(vertexarray);
   glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
   glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
   glEnableVertexAttribArray(0);
   glUseProgram(shaderProgram);
   glBindVertexArray(vertexarray);

   do{
      // Clear the screen
      glClear( GL_COLOR_BUFFER_BIT );

      // Use our shader
      glUseProgram(programID);

      // 1rst attribute buffer : vertices
      glEnableVertexAttribArray(0);
      glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
      glVertexAttribPointer(
         0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
         3,                  // size
         GL_FLOAT,           // type
         GL_FALSE,           // normalized?
         0,                  // stride
         (void*)0            // array buffer offset
      );

      // Draw the triangle !
      glDrawArrays(GL_TRIANGLES, 0, 3); // 3 indices starting at 0 -> 1 triangle

      glDisableVertexAttribArray(0);

      // Swap buffers
      glfwSwapBuffers(window);
      glfwPollEvents();

   } // Check if the ESC key was pressed or the window was closed
   while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
         glfwWindowShouldClose(window) == 0 );

   // Cleanup VBO
   glDeleteBuffers(1, &vertexbuffer);
   glDeleteVertexArrays(1, &VertexArrayID);
   glDeleteProgram(programID);

   // Close OpenGL window and terminate GLFW
   glfwTerminate();

   return 0;
}

