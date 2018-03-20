#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <cstdlib>

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main( void )
{
   GLFWwindow *window;
    
   // Initialize the library
   if (!glfwInit()) {
        return -1;
   }
    
   // Create a windowed mode window and its OpenGL context
   window = glfwCreateWindow( SCREEN_WIDTH, SCREEN_HEIGHT, "T3 (Three-coloured Triangle)", NULL, NULL );    
    
   int screen_width, screen_height;
   glfwGetFramebufferSize( window, &screen_width, &screen_height );

   if (!window) {
        glfwTerminate();
        return -1;
   }
    
   // Make the window's context current
   glfwMakeContextCurrent(window);
    
   glViewport(0.0f, 0.0f, screen_width, screen_height); // specifies the part of the window to which OpenGL will draw (in pixels), convert from normalised to pixels
   glMatrixMode(GL_PROJECTION); // projection matrix defines the properties of the camera that views the objects in the world coordinate frame. Here you typically set the zoom factor, aspect ratio and the near and far clipping planes
   glLoadIdentity(); // replace the current matrix with the identity matrix and starts us a fresh because matrix transforms such as glOrpho and glRotate cumulate, basically puts us at (0, 0, 0)
   glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 0, 600); // essentially set coordinate system
   glMatrixMode(GL_MODELVIEW); // (default matrix mode) modelview matrix defines how your objects are transformed (meaning translation, rotation and scaling) in your world
   glLoadIdentity( ); // same as above comment
    
   GLfloat half_screen_width = SCREEN_WIDTH / 2;
   GLfloat half_screen_height = SCREEN_HEIGHT / 2;
    
   GLfloat half_side_length = 250;
    
   GLfloat vertices[] = {
        half_screen_width, half_screen_height + half_side_length, 0.0, // top center vertex
        half_screen_width - half_side_length * 1.2, half_screen_height - half_side_length, 0.0, // bottom left corner
        half_screen_width + half_side_length * 1.2, half_screen_height - half_side_length, 0.0 // bottom right corner
   };
    
   GLfloat colour[] = {
         255, 0, 0,
         0, 255, 0,
         0, 0, 255
   };
    
   // Loop until the user closes the window
   while (!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT);
      // White background
      glClearColor(1.0f, 1.0f, 1.0f, 1.0f);        
      
      // Render OpenGL here
      glEnableClientState(GL_VERTEX_ARRAY); // tell OpenGL that you're using a vertex array for fixed-function attribute
      glEnableClientState(GL_COLOR_ARRAY);
      glVertexPointer(3, GL_FLOAT, 0, vertices); // point to the vertices to be used
      glColorPointer(3, GL_FLOAT, 0, colour);
      glDrawArrays(GL_TRIANGLES, 0, 3); // draw the vertixes
      glDisableClientState(GL_COLOR_ARRAY);
      glDisableClientState(GL_VERTEX_ARRAY); // tell OpenGL that you're finished using the vertex array attribute

      // Swap front and back buffers
      glfwSwapBuffers(window);

      // Poll for and process events
      glfwPollEvents();
   }

   glfwTerminate();
    
   return 0;
}




