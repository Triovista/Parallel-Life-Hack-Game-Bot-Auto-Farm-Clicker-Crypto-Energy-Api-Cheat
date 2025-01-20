
#include "gl_stuff.h"
#include <iostream>

using namespace CoreStructures;
using namespace std;

// Variables needed to track where the mouse pointer is so we can determine which direction it's moving in
static int								mouse_x, mouse_y;
static bool								mDown = false;
static GUPivotCamera					*mainCamera = nullptr;
static GUMatrix4						modelTransform = GUMatrix4();
static bool								showEmptyCells = false;

// Private callback functions
void displayCallback(void);
void mouseButtonDown(int button_id, int state, int x, int y);
void mouseMove(int x, int y);
void mouseWheel(int wheel, int direction, int x, int y);
void keyDown(unsigned char key, int x, int y);
void closeWindow(void);

// Initialise OpenGL environment and create main window on-screen
void gl_init(int argc, char* argv[], void(*update)(void), char* title, bool _showEmptyCells) {

	// Initialise FreeGLUT
	glutInit(&argc, argv);

	// Specify OpenGL environment properties
	glutInitContextVersion(3, 3);
	glutInitContextProfile(GLUT_COMPATIBILITY_PROFILE);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE/* | GLUT_MULTISAMPLE*/);
	glutSetOption(GLUT_MULTISAMPLE, 4);

	// Setup window and display
	int windowWidth = 800;
	int windowHeight = 600;
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(64, 64);
	glutCreateWindow(title);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

	// Register callback functions - so when things happen our functions respond!
	glutIdleFunc(update); // Main scene update function (see update function in glDemo.cpp)
	glutDisplayFunc(displayCallback); // Main render function
	glutKeyboardFunc(keyDown); // Key down handler
	glutMouseFunc(mouseButtonDown); // Mouse button handler
	glutMotionFunc(mouseMove); // Mouse move handler
	glutMouseWheelFunc(mouseWheel); // Mouse wheel event handler
	glutCloseFunc(closeWindow); // Main resource cleanup handler

	// Initialise glew (So we can access latest OpenGL functionality)
	glewInit();

	// Initialise OpenGL state

	//wglSwapIntervalEXT(0);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glFrontFace(GL_CCW);

	// Setup colour to clear the screen (we'll use black here)
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Setup main camera
	float viewportAspect = (float)windowWidth / (float)windowHeight;
	mainCamera = new GUPivotCamera(0.0f, 0.0f, 15.0f, 55.0f, viewportAspect, 0.1f);

	// Setup environment flags
	showEmptyCells = _showEmptyCells;
}

// Clears the OpenGL display buffer ready for drawing
void clearScreen() {

	// Turn off shaders
	glUseProgram(0);

	// Clear the screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
}

void setupCameraTransforms() {

	if (mainCamera) {

		// Setup camera aspect ratio to matrix w:h ratio of the on-screen window
		mainCamera->setAspectRatio((float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT));

		// Setup transformations based on camera position and orientation
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glMultMatrixf((const float*)mainCamera->projectionTransform().M);

		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glMultMatrixf((const float*)mainCamera->viewTransform().M);
	}
	
	glMultMatrixf((const float*)modelTransform.M);
}

void renderLatestGeneration(Array2D<int>& array) {

	clearScreen();
	setupCameraTransforms();

	if (!showEmptyCells) {
		glPolygonMode(GL_FRONT, GL_FILL);
	}

	// Render array
	const float cellSize = 0.1f;

	for (int y = 0; y < array.h; y++) {

		for (int x = 0; x < array.w; x++) {

			if (showEmptyCells) {

				if (array(x, y) == 0)
					glPolygonMode(GL_FRONT, GL_LINE);
				else
					glPolygonMode(GL_FRONT, GL_FILL);

				float x_ = (-(array.w) / 2.0f + float(x)) * cellSize;
				float y_ = ((array.h) / 2.0f - float(y)) * cellSize;

				glBegin(GL_QUADS);

				glVertex3f(x_, y_, 0.0f);
				glVertex3f(x_ + cellSize, y_, 0.0f);
				glVertex3f(x_ + cellSize, y_ + cellSize, 0.0f);
				glVertex3f(x_, y_ + cellSize, 0.0f);

				glEnd();
			}
			else {

				// Only interested in rendering non-empty ie. alive cells
				if (array(x, y) == 1) {

					float x_ = (-(array.w) / 2.0f + float(x)) * cellSize;
					float y_ = ((array.h) / 2.0f - float(y)) * cellSize;

					glBegin(GL_QUADS);

					glVertex3f(x_, y_, 0.0f);
					glVertex3f(x_ + cellSize, y_, 0.0f);
					glVertex3f(x_ + cellSize, y_ + cellSize, 0.0f);
					glVertex3f(x_, y_ + cellSize, 0.0f);

					glEnd();
				}
			}
		}
	}

	presentLatestGeneration();
}

// Present the OpenGL display buffer to the monitor
void presentLatestGeneration() {

	glutSwapBuffers();
}


#pragma region Private callback functions

void displayCallback() { // Placeholder for FreeGLUT - we manually call displayActual above so ignore this!
}

void mouseButtonDown(int button_id, int state, int x, int y) {

	if (button_id == GLUT_LEFT_BUTTON) {

		if (state == GLUT_DOWN) {

			mouse_x = x;
			mouse_y = y;

			mDown = true;

		}
		else if (state == GLUT_UP) {

			mDown = false;
		}
	}
}

void mouseMove(int x, int y) {

	int dx = x - mouse_x;
	int dy = y - mouse_y;

	// ctrl + drag rotates the camera...
	if ((glutGetModifiers() & GLUT_ACTIVE_CTRL) != 0 && mainCamera != nullptr) {

		mainCamera->transformCamera((float)-dy, (float)-dx, 0.0f);
	}
	else {

		// Normal drag pans the camera left / right and up / down
		// Note: The fourth column of modelTransform (elements 12, 13, 14 and 15) represent the translation, where element 12 and 13 represent the x, y translation respectively.
		modelTransform.M[12] += (float)dx * 0.01f;
		modelTransform.M[13] -= (float)dy * 0.01f;
	}
		
	mouse_x = x;
	mouse_y = y;
}

void mouseWheel(int wheel, int direction, int x, int y) {

	if (mainCamera) {

		if (direction < 0)
			mainCamera->scaleCameraRadius(1.1f);
		else if (direction > 0)
			mainCamera->scaleCameraRadius(0.9f);
	}
}

void keyDown(unsigned char key, int x, int y) {

	static const int KEY_ESCAPE = 27;

	if (key=='f' || key=='F') {

		// Toggle fullscreen (This does not adjust the display mode / resolution however!)
		glutFullScreenToggle();
	}
	else if (key=='r' || key=='R') {

		// Reset camera
		if (mainCamera) {

			GUPivotCamera* newCamera = new GUPivotCamera(0.0f, 0.0f, mainCamera->cameraRadius(), 55.0f, (float)glutGet(GLUT_WINDOW_WIDTH) / (float)glutGet(GLUT_WINDOW_HEIGHT), 0.1f);

			if (newCamera) {

				mainCamera->release();
				mainCamera = newCamera;
			}
		}
		
		// Reset model transform
		modelTransform.M[12] = 0.0f;
		modelTransform.M[13] = 0.0f;
	}
	else if (int(key) == KEY_ESCAPE) {

		// Quit on Escape key press
		glutLeaveMainLoop();
	}
}

void closeWindow(void) {

	// Clean-up scene resources

	if (mainCamera)
		mainCamera->release();
}

#pragma endregion