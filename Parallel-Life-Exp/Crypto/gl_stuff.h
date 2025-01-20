#pragma once

// Provide OpenGL environment

#include <glew/glew.h>
#include <glew/wglew.h>
#include <GL\freeglut.h>
#include <CoreStructures\CoreStructures.h>
#include "Array2D.h"

// Initialise OpenGL environment and create main window on-screen
void gl_init(int argc, char* argv[], void(*update)(void), char* title, bool _showEmptyCells);

// Clears the OpenGL display buffer ready for drawing
void clearScreen();

// Main scene render function
void renderLatestGeneration(Array2D<int>&);

// Present the OpenGL display buffer to the monitor
void presentLatestGeneration();