#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <GL/glu.h>

Display *dpy;
Window win;

void DrawObjects(int x, int y){
   
   XWindowAttributes winattr;

   GLfloat size[2];
   static GLfloat current_size;
   static int iscalled = 0;
   static int factor = 1;

   float nx, ny;

   XGetWindowAttributes(dpy, win, &winattr);
   glViewport(0, 0, winattr.width, winattr.height);

   glClearColor(1.7f, 1.7f, 0.7f, 0.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   glLoadIdentity();
   glOrtho(-10.0, 10.0, -10.0, 10.0, -10.0, 10.0);

   nx = ((float)x*20.0/(float)winattr.width)-10.0;
   ny = 10.0-((float)y*20.0/(float)winattr.height);

   glPushMatrix();

      glTranslatef(nx, ny, 0.0f);

      glBegin(GL_QUADS);
         glColor3f(2.0f,2.0f, 0.0f);

         glVertex3f(-2.0f, 2.0f, -0.1f);
         glVertex3f(2.0f, 2.0f, -0.1f);
         glVertex3f(2.0f, -2.0f, -0.1f);
         glVertex3f(-2.0f, -2.0f, -0.1f);
      glEnd();

   glPopMatrix();

   glXSwapBuffers(dpy, win);

   factor++;

}

int main(int argc, char *argv[]) {
   int screen;
   Window root_win;
   XEvent event;

   unsigned int depth;
   XSetWindowAttributes attrs;

   GLint att[] = {GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None};
   XVisualInfo *visual;
   GLXContext glc;

   Window root_return;
   Window child_return;
   int root_x_return, root_y_return;
   int win_x_return, win_y_return;
   unsigned int mask_return;

   dpy = XOpenDisplay(NULL);

   if (dpy == NULL) {
      fprintf(stderr, "Cannot open display\n");
      exit(1);
   }

   screen = DefaultScreen(dpy);
   depth = DefaultDepth(dpy, screen);
   root_win = RootWindow(dpy, screen);

   visual = glXChooseVisual(dpy, screen, att);

   attrs.border_pixel = BlackPixel(dpy, screen);
   attrs.background_pixel = WhitePixel(dpy, screen);
   attrs.override_redirect = True;

   attrs.colormap = XCreateColormap(dpy, root_win, visual->visual, AllocNone);
   attrs.event_mask = ExposureMask | KeyPressMask | PointerMotionMask;

   win = XCreateWindow(dpy, root_win, 100, 100, 600, 600, 0, visual->depth, InputOutput, visual->visual,
      CWBackPixel | CWColormap | CWBorderPixel | CWEventMask, &attrs);

   XMapWindow(dpy, win);

   glc = glXCreateContext(dpy, visual, NULL, GL_TRUE);
   glXMakeCurrent(dpy, win, glc);

   glEnable(GL_DEPTH_TEST);

   while(1) {
      XNextEvent(dpy, &event);

      if (event.type == KeyPress) {
         XDestroyWindow(dpy, win);
         XCloseDisplay(dpy);
         break;
      } else if (event.type = MotionNotify) {
         XQueryPointer(dpy, win, &root_return, &child_return,
            &root_x_return, &root_y_return,
            &win_x_return, &win_y_return,
            &mask_return);
         DrawObjects(win_x_return, win_y_return);
      }
   }

}