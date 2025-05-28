#pragma once
#include <GL/glut.h>
#include "fluid.h"

void initGL(int windowWidth, int windowHeight);
void onDisplay();
void draw(int w, int h);
void onMouse(int button, int state, int x, int y);
void onMotion(int x, int y);
void onKeyboard(unsigned char key, int x, int y);
void onTimer(int value);

void setFluid(Fluid* f);
