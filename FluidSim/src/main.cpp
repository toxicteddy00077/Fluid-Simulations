
#include <GL/glut.h>
#include "fluid.h"
#include "helper.h"
#include <iostream>
#include <cstdlib>  
#include <cmath>   
#include <algorithm> 
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>

constexpr int REFRESH_MILLIS = 16; 

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    int windowSize = N * SCALE;
    glutInitWindowSize(windowSize, windowSize);
    glutCreateWindow("Fluid Simulation");

    static Fluid fluid;
    setFluid(&fluid);

    initGL(windowSize, windowSize);

    glutDisplayFunc(onDisplay);
    glutReshapeFunc(draw);
    glutMouseFunc(onMouse);
    glutMotionFunc(onMotion);
    glutKeyboardFunc(onKeyboard);
    glutTimerFunc(REFRESH_MILLIS, onTimer, 0);

    glutMainLoop();
    return 0;
}