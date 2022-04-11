// MAIN_IsingModel.cpp
//

#include <GL/glut.h>
#include <iostream>
#include "IsingSystem.h"

using namespace std;

// functions which are needed for openGL go into a namespace so that we can identify them
namespace drawFuncs {
	void handleKeypress(unsigned char key, int x, int y);
	void display(void);
	void update(int val);
	void introMessage();
	void quitMe(int val);
}

// this is a global pointer, which is how we access the system itself
IsingSystem *sys;

int main(int argc, char **argv) {
	// turn on glut
	glutInit(&argc, argv);

	int window_size[] = { 480,480 };
	string window_title("simple Ising simulation");

	Window *win = new Window(window_size, window_title);

	sys = new IsingSystem(win);

	// this is the seed for the random numbers
	int seed = 6;
	cout << "setting seed " << seed << endl;
	sys->setSeed(seed);

	drawFuncs::introMessage();

	// tell openGL how to redraw the screen and respond to the keyboard
	glutDisplayFunc(drawFuncs::display);
	glutKeyboardFunc(drawFuncs::handleKeypress);

	// tell openGL to do its first update after waiting 10ms
	int wait = 10;
	int val = 0;
	glutTimerFunc(wait, drawFuncs::update, val);

	// start the openGL stuff
	glutMainLoop();

	cout << "the end" << endl;
	cin.get();

	return 0;
}

void drawFuncs::introMessage() {
	cout << "Keys (while in graphics window):" << endl;
	cout << "  ? to print this message" << endl;
	cout << "  q or e to quit (or exit)" << endl;
	cout << "  f for fast; s for slow" << endl;
	cout << "  g to go; p to pause" << endl;
	cout << "  h for hotter; c for colder" << endl;
	cout << "  u for update once" << endl;
	cout << "  m to print magnetisation" << endl;
}

void drawFuncs::quitMe(int val) {
	cout << "delete system" << endl;
	delete sys;
	cout << "leave loop" << endl;
	exit(1);
}

// openGL function that deals with the keyboard
void drawFuncs::handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':
	case 'e':
		cout << "Exiting..." << endl;
		// pause
		sys->pauseRunning();
		// wait 500ms for any redrawing etc to finish, then quit
		glutTimerFunc(500, drawFuncs::quitMe, 17);
		break;
	case 's':
		cout << "slow" << endl;
		sys->setSlow();
		break;
	case 'f':
		cout << "fast" << endl;
		sys->setFast();
		break;
	case 'h':
		cout << "hotter" << endl;
		sys->Hotter();
		break;
	case 'c':
		cout << "colder" << endl;
		sys->Colder();
		break;
	case 'g':
		cout << "go" << endl;
		sys->setRunning();
		drawFuncs::update(0);
		break;
	case 'p':
		cout << "pause" << endl;
		sys->pauseRunning();
		break;
	case 'u':
		cout << "upd" << endl;
		sys->Update();
		break;
	case 'r':
		cout << "reset" << endl;
		sys->Reset();
		break;
	case '?':
		drawFuncs::introMessage();
		break;
  case 'm':
    cout << sys->getMagnetisation();
	}
	glutPostRedisplay();
}

// this function gets called whenever the algorithm should do its update
void drawFuncs::update(int val) {
	int wait;  // time to wait between updates (milliseconds)

	if (sys->isRunning()) {
		sys->Update();

		glutPostRedisplay(); // tells GLUT that the display has changed
							 // if you comment out this line then it will refresh
							 // the screen only when you press a key

		if (sys->isSlow() == 1)
			wait = 200;
		else
			wait = 20;
		// tell openGL to call this funtion again after "wait" milliseconds
		glutTimerFunc(wait, drawFuncs::update, 0);
	}
}

// this function redraws the window when necessary
void drawFuncs::display() {
	//  Clear the window or more specifically the frame buffer...
	//  This happens by replacing all the contents of the frame
	//  buffer by the clear color (black in our case)
	glClear(GL_COLOR_BUFFER_BIT);

	// this puts the camera at the origin (not sure why) with (I think) z axis out of page and y axis up
	// there is also the question of the GL perspective which is not set up in any clear way at the moment
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1.0,   /* camera position */
		0.0, 0.0, -1.0,        /* point to look at */
		0.0, 1.0, 0.0);		   /* up direction */

	sys->DrawSquares();

	//  Swap contents of backward and forward frame buffers
	glutSwapBuffers();
}


