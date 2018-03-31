
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/freeglut_ext.h>
#include <iostream>
#include <math.h>
using namespace std;

enum Color{
RED,GREEN,BLUE
};

typedef struct{
int width;
int height;
char* title;

float field_of_view_angle;
float z_near;
float z_far;
} glutWindow;
glutWindow win;

void swapBuffers(void);
int startGlut(int argc, char **argv);
void init(void);
void display(void);
void changed(int, int);
void keyboard ( unsigned char key, int mousePositionX, int mousePositionY );
void mouseEvent(int button, int state, int x, int y);
void mouseMotion(int x, int y);

void swapBuffers(void){
glutSwapBuffers();
}

int startGlut(int argc, char **argv)
{
//glGetIntegerv
win.width = 640;
win.height = 480;
win.title = "3D Car Showcase";
win.z_near = 1.0f;
win.z_far = 500.0f;
win.field_of_view_angle = 45.0f;

glutInit(&argc, argv);
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
glutInitWindowSize(win.width, win.height);
glutInitWindowPosition(100, 100);
glutCreateWindow(win.title);

glutDisplayFunc(display);
glutReshapeFunc(changed);
glutKeyboardFunc( keyboard );
glutMouseFunc(mouseEvent);
glutMotionFunc(mouseMotion);
init();
glutMainLoop();
return 1;
}

int main(int argc, char **argv){
startGlut(argc, argv);
return 0;
}


class Traingle{
private:
static GLfloat vertex[9];
static GLfloat index[6];
static GLfloat color[16];

public:
void draw(){
glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);
glVertexPointer(3, GL_FLOAT, 0, vertex);
glColorPointer(4, GL_FLOAT, 0, color);
glDrawElements(GL_TRIANGLE_STRIP, 6, GL_FLOAT, index);
glDisableClientState(GL_VERTEX_ARRAY);
glDisableClientState(GL_COLOR_ARRAY);
}
};
Traingle tri;

float Traingle::vertex[9] = {
1.0, 0.0, 0.0,
0.0, 1.0, 0.0,
-1.0, 0.0, 0.0 };


float Traingle::color[16] ={
1.0f, 0.0f, 0.0f, 1.0f,
1.0f, 0.0f, 0.0f, 1.0f,
1.0f, 0.0f, 0.0f, 1.0f,
1.0f, 0.0f, 0.0f, 1.0f,
};
float Traingle::index[6] ={
0, 1, 2,
2, 1, 3
};

int frame = 0;
void keyboard ( unsigned char key, int mousePositionX, int mousePositionY )
{
cout<<"keyboard"<<endl;
switch ( key )
{
case 27:
//exit ( 0 );
frame++;
break;

default:
break;
}
}


int mouseButton ;
int oldX,newX, oldY,newY;
bool rotaetAxixX = false;
float rotateAngle = 0.0;
float depth;

void checkState(int state, int x, int y){
switch(state){
case GLUT_UP:
//cout<<" UP"<<endl;
break;
case GLUT_DOWN:
//cout<<" DOWN"<<endl;
break;
};
}

void mouseEvent(int button, int state, int x, int y)
{
mouseButton = button;
switch(button){
case GLUT_LEFT_BUTTON:
if(state == GLUT_DOWN)
{
oldX = x;
oldY = y;
}
checkState(state, x, y);
break;
case GLUT_RIGHT_BUTTON:
//cout<<"RIGHT";
checkState(state, x, y);
break;
case GLUT_MIDDLE_BUTTON:
//cout<<"MIDDLE";
checkState(state, x, y);
break;
};
}


void mouseMotion(int x,int y)
{
if(mouseButton == GLUT_LEFT_BUTTON)
{
cout<<" delX="<<(x - oldX)<<" delY="<<(y - oldY)<<endl;

if(abs(x - oldX) > 0 && abs(x - oldX) > abs(y - oldY))
{
//rotate about Y
if((x - oldX) > 0)
{
rotateAngle += 10.0 * 0.11;
}
else
{
rotateAngle -= 10.0 * 0.11;
}
rotaetAxixX = false;
}
else if(abs(y - oldY) > 0 && abs(y - oldY) > abs(x - oldX))
{
//rotate about X
if((y - oldY) > 0)
{
rotateAngle += 10.0 * 0.11;
}
else
{
rotateAngle -= 10.0 * 0.11;
}
rotaetAxixX = true;
}

oldX = x;
oldY = y;
}
}

void init(void)
{
glClearColor(0.8f, 0.8f, 0.6f, 1.0f);
glClearDepth(1.0f);
glEnable(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
glShadeModel(GL_SMOOTH);
/*GL_LINE_SMOOTH_HINT, GL_POLYGON_SMOOTH_HINT, GL_TEXTURE_COMPRESSION_HINT, and GL_FRAGMENT_SHADER_DERIVATIVE_HINT*/
glHint(GL_POLYGON_SMOOTH_HINT , GL_NICEST);
}

void setColor(Color color){
switch(color){
case RED:
glColor3f(1.0, 0.0, 0.0);
break;
case GREEN:
glColor3f(0.0, 1.0, 0.0);
break;
case BLUE:
glColor3f(0.0, 0.0, 1.0);
break;
}
}




void display(void){
glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//rotateAngle += 0.11f;
depth -= 0.003f;

glMatrixMode(GL_MODELVIEW);
glLoadIdentity();

gluLookAt(0.0, 2.0, 0.0, 0.0, 0.0, -4.0, 0.0, 1.0, 0.0);//Eye
glTranslatef(0.0, 0.0, -4.0 + depth);
if(rotaetAxixX == true)
glRotatef(rotateAngle, 1.0, 0.0, 0.0);
else
glRotatef(rotateAngle, 0.0, 1.0, 0.0);

//tri.draw();
setColor(BLUE);
glBegin(GL_TRIANGLE_STRIP);
//front
glVertex3f(0.5, -0.5, 0.5);
glVertex3f(-0.5, -0.5, 0.5);
glVertex3f(0.5, 0.5, 0.5);
glVertex3f(-0.5, 0.5, 0.5);
glEnd();

glBegin(GL_TRIANGLE_STRIP);
//right
glVertex3f(0.5, -0.5, -0.5);
glVertex3f(0.5, -0.5, 0.5);
glVertex3f(0.5, 0.5, -0.5);
glVertex3f(0.5, 0.5, 0.5);
glEnd();

glBegin(GL_TRIANGLE_STRIP);
//back
glVertex3f(-0.5, -0.5, -0.5);
glVertex3f(0.5, -0.5, -0.5);
glVertex3f(-0.5, 0.5, -0.5);
glVertex3f(0.5, 0.5, -0.5);
glEnd();

glBegin(GL_TRIANGLE_STRIP);
//left
glVertex3f(-0.5, -0.5, -0.5);
glVertex3f(-0.5, -0.5, 0.5);
glVertex3f(-0.5, 0.5, -0.5);
glVertex3f(-0.5, 0.5, 0.5);
glEnd();


setColor(RED);
glBegin(GL_TRIANGLE_STRIP);
//top
glVertex3f(0.5, 0.5, -0.5);
glVertex3f(0.5, 0.5, 0.5);
glVertex3f(-0.5, 0.5, -0.5);
glVertex3f(-0.5, 0.5, 0.5);
glEnd();

glBegin(GL_TRIANGLE_STRIP);
//bottom
glVertex3f(0.5, -0.5, -0.5);
glVertex3f(0.5, -0.5, 0.5);
glVertex3f(-0.5, -0.5, -0.5);
glVertex3f(-0.5, -0.5, 0.5);
glEnd();

swapBuffers();
glutPostRedisplay();//to draw window continuously
}

void changed(int w, int h){
glMatrixMode(GL_PROJECTION);
glLoadIdentity();
glViewport(0, 0, win.width, win.height);
float ratio = (float)w / h;
gluPerspective(win.field_of_view_angle, ratio, win.z_near, win.z_far);
}