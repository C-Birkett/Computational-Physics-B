// MAIN_IsingModel.cpp
//
// TODO:
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
	cout << "  q to quit" << endl;
	cout << "  f for fast; s for slow" << endl;
	cout << "  g to go; p to pause" << endl;
	cout << "  h for hotter; c for colder" << endl;
	cout << "  u for update once" << endl;
	cout << "  m to print system magnetisation" << endl;
	cout << "  m to print system energy per spin" << endl;
	cout << "  t to set up data recording" << endl;
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
		cout << "upd: " << endl;
		sys->Update();
    sys->printSimInfo();
		break;
	case 'r':
		cout << "reset" << endl;
		sys->Reset();
		break;
	case '?':
		drawFuncs::introMessage();
		break;
  case 'm':
    cout << "System magnetisation:" << endl;
    cout << sys->getMagnetisation() << endl;
    break;
  case 'e':
    cout << "System energy per spin:" << endl;
    cout << sys->getEnergy() << endl;
    break;
  case 't':
    cout << endl << "Set up recording parameters:" << endl;
    cout << "These are the preset sim parameters used in the data analysis of the coursework," << endl;
    cout << "select one of the following:" << endl << endl;
    cout << "0 -> input custom parameters" << endl;
    cout << "1 -> ex 1: convergence from initial condition to equilibrium T in [1, 5]" << endl;
    cout << "2 -> ex 1: '' T in [2.5, 3]" << endl;
    cout << "3 -> ex 1: '' T in [2.5, 2.6]" << endl;
    cout << "4 -> ex 2: equilibrium values for T in [1, 4] inital sweeps ignored = 200 take avg over 10 vals taken every 50 sweeps" << endl;
    cout << "5 -> " << endl;

    cout << endl << "selection: ";


    int presetNum;
    cin >> presetNum;

    // should be switch here, but want to use break; to escape handleKeypress switch
    // only have a few so should be ok to use else if
    // #1 -> look at convergence of magnetisation and energy from initial state
    if(presetNum==1){
      int nSims = 900;
      int nISweeps = 0; //looking at convergence from initial conditions
      int nSweeps = 50;
      int intSweeps = 1;

      double t = 1;
      double tInt = 0.5;
      double tSimInt = 100; //100 sims at each t from 1 to 5
      sys->setRecording(nSims, nISweeps, nSweeps, intSweeps, t, tInt, tSimInt);
      cout << "press g (go) in the window to start" << endl;
      sys->setFast();
      break;
    }
    // #2 -> look more closely at T in range [2.5, 3]
    else if(presetNum==2){
      int nSims = 600;
      int nISweeps = 0; //looking at convergence from initial conditions
      int nSweeps = 100;
      int intSweeps = 1;

      double t = 2.5;
      double tInt = 0.1;
      double tSimInt = 100; //100 sims at each t from 1 to 5
      sys->setRecording(nSims, nISweeps, nSweeps, intSweeps, t, tInt, tSimInt);
      cout << "press g (go) in the window to start" << endl;
      sys->setFast();
      break;
    }
    // #3 -> look more closely at T in range [2.5, 2.6]
    else if(presetNum==3){
      int nSims = 600;
      int nISweeps = 0; //looking at convergence from initial conditions
      int nSweeps = 200;
      int intSweeps = 1;

      double t = 2.5;
      double tInt = 0.02;
      double tSimInt = 100; //100 sims at each t from 1 to 5
      sys->setRecording(nSims, nISweeps, nSweeps, intSweeps, t, tInt, tSimInt);
      cout << "press g (go) in the window to start" << endl;
      sys->setFast();
      break;
    }
    // #4 -> 
    else if(presetNum==4){
      int nSims = 700;
      int nISweeps = 200; // ignore first 200
      int nSweeps = 700;  // measure every 50 past 200 -> 10 measurements
      int intSweeps = 50; // assumption is that in equilibrium the states are independent after 50 sweeps

      double t = 1;
      double tInt = 0.5;
      double tSimInt = 100; //100 sims at each t from 1 to 4
      sys->setRecording(nSims, nISweeps, nSweeps, intSweeps, t, tInt, tSimInt);
      cout << "press g (go) in the window to start" << endl;
      sys->setFast();
      break;
    }
    // #5 -> 
    else if(presetNum==5){
    }
    // #6 -> 
    else if(presetNum==6){
    }

    cout << "input number of sweeps to perform:";
    int nSweeps;
    cin >> nSweeps;
    cout << "number of sweeps set as: "<< nSweeps << endl;

    cout << "input initial number of sweeps to  ignore:";
    int nISweeps;
    cin >> nISweeps;
    cout << "number of initial sweeps set as: "<< nISweeps << endl;

    cout << "input interval number of sweeps to record in:";
    int intSweeps;
    cin >> intSweeps;
    cout << "sweep interval set as: "<< intSweeps << endl;

    cout << "input number of simulations to perform:";
    int nSims;
    cin >> nSims;
    cout << "number of sims set as: "<< nSims << endl;

    cout << "input initial temperature:";
    int t;
    cin >> t;
    cout << "initial temperature set as: "<< t << endl;

    cout << "input temperature interval:";
    int tInt;
    cin >> tInt;
    cout << "temperature interval set as: "<< tInt << endl;

    cout << "input number of sims to do at each temperature:";
    int tSimInt;
    cin >> tSimInt;
    cout << "temperature sim interval set as: "<< tSimInt << endl;

    sys->setRecording(nSims, nISweeps, nSweeps, intSweeps, t, tInt, tSimInt);

    cout << "press g (go) in the window to start" << endl;

    sys->setFast();
		//drawFuncs::update(0);
    break;
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


