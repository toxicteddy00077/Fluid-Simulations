#include "helper.h"
#include <cstdlib>  
#include <GL/glut.h> 

#define VELOCITY_MULTIPLIER 0.1f  
#define DENSITY_AMOUNT 4  
#define REFRESH_MILLIS 16  

static Fluid* fluid = nullptr;

static int winW, winH;
static int lastX, lastY;

void setFluid(Fluid* f) {
  fluid = f;
}

void initGL(int windowWidth, int windowHeight) {
  winW = windowWidth;  winH = windowHeight;
  glClearColor(0,0,0,1);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void onDisplay() {
  glClear(GL_COLOR_BUFFER_BIT);

  fluid->step();
  fluid->render(GREEN);
  fluid->fade();

  glutSwapBuffers();
}

void draw(int w, int h) {
  winW = w;  winH = h;
  glViewport(0,0,w,h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluOrtho2D(0, N*SCALE, 0, N*SCALE);
  glMatrixMode(GL_MODELVIEW);
}

void onMouse(int button, int state, int x, int y) {
  lastX = x;  lastY = y; 
}

void onMotion(int x, int y) {
  int i = x / SCALE;
  int j = (winH - y) / SCALE;
  fluid->addDensity(i,j,DENSITY_AMOUNT);
  float vx = (x - lastX) * VELOCITY_MULTIPLIER;
  float vy = (lastY - y) * VELOCITY_MULTIPLIER;
  fluid->addVelocity(i,j, vx, vy);
  lastX = x;  lastY = y;
}

void onKeyboard(unsigned char key, int x, int y) {
  if (key == 27) std::exit(0);            
}

void onTimer(int value) {
  glutPostRedisplay();
  glutTimerFunc(REFRESH_MILLIS, onTimer, 0);
}
