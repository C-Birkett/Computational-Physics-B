#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string>

#include "DLASystem.h"
#include "Window.h" 

//TODO Fix CSVWrite includes!!!
//#include "CSVWrite.h"

using namespace std;

// functions which are needed for openGL go into a namespace so that we can identify them
namespace drawFuncs {
  void handleKeypress(unsigned char key, int x, int y);
  void display(void);
  void update(int val);
  void introMessage();
}

// this is a global pointer, which is how we access the system itself
DLASystem *sys;

//REDUNDANT
//declare data recording function
//void DLAData(DLASystem* system, int nInterval, pair<int, int> nRange);

int main(int argc, char **argv) {

  //Breaks includes for some reason
  //Test CSV writing system
  //CSVTest();

  // turn on glut
	glutInit(&argc, argv);

  int window_size[] = { 480,480 };
  string window_title("simple DLA simulation");

  // create a window
  Window *win = new Window(window_size,window_title);

  // set conditions for simulation

  //data recording range sets size of sim
  pair<int, int> range = {0,10000};

  // create the system
  sys = new DLASystem(win, range.second);

  // data recording
  sys->recordData(500, range);

  //set background to white by default
  sys->setWinBackgroundWhite();
  
  // this is the seed for the random numbers
  int seed = 6;
  cout << "setting seed " << seed << endl;
  sys->setSeed(seed);
  
  // print the "help" message to the console
  drawFuncs::introMessage();
  
  // tell openGL how to redraw the screen and respond to the keyboard
	glutDisplayFunc(  drawFuncs::display );
	glutKeyboardFunc( drawFuncs::handleKeypress );
  
  // tell openGL to do its first update after waiting 10ms
  int wait = 10;
  int val = 0;
	glutTimerFunc(wait, drawFuncs::update, val);

  // start the openGL stuff
 	glutMainLoop();

  return 0;
}

// this is just a help message
void drawFuncs::introMessage() {
        cout << "Keys (while in graphics window):" << endl << "  q or e to quit (or exit)" << endl;
        cout << "  h to print this message (help)" << endl;
        cout << "  u for a single update" << endl;
        cout << "  g to start running (go)" << endl;
        cout << "  p to pause running" << endl;
        cout << "  s to run in slow-mode" << endl;
        cout << "  f to run in fast-mode" << endl;
        cout << "  r to clear everything (reset)" << endl;
        cout << "  z to pause and zoom in" << endl;
        cout << "  w or b to change background colour to white or black" << endl;
}

// openGL function deals with the keyboard
void drawFuncs::handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
  case 'h':
    drawFuncs::introMessage();
    break;
	case 'q':
	case 'e':
		cout << "Exiting..." << endl;
    // delete the system
    delete sys;
		exit(0);
		break;
  case 'p':
    cout << "pause" << endl;
    sys->pauseRunning();
    break;
  case 'g':
    cout << "go" << endl;
    sys->setRunning();
    glutTimerFunc(0, drawFuncs::update, 0);
    break;
  case 's':
    cout << "slow" << endl;
    sys->setSlow();
    break;
  case 'w':
    cout << "white" << endl;
    sys->setWinBackgroundWhite();
    break;
  case 'b':
    cout << "black" << endl;
    sys->setWinBackgroundBlack();
    break;
  case 'f':
    cout << "fast" << endl;
    sys->setFast();
    break;
  case 'r':
    cout << "reset" << endl;
    sys->Reset();
    break;
  case 'z':
    cout << "zoom" << endl;
    sys->pauseRunning();
    sys->viewAddCircle();
    break;
  case 'u':
    cout << "upd" << endl;
    sys->Update();
    break;
  //print number of particles and radius
  case '0':
    cout << "number of particles and radius:" << endl;
    cout << sys->getNumParticles() << ", " << sys->getClusterRadius() << endl;
    cout << "simple fractal dimension with a = 1: " << endl;
    cout << sys->simpleFracDim() << endl;
    break;
  //start recording data
  case 'm':{
    cout << "recording data to csv" << endl;
    pair<int, int> range = {0,1000};
    sys->recordData(0, range);
    break;}
  case 'v':
    double vwsze;
    cin >> vwsze;
    sys->setViewSize(vwsze);
	}
  // tell openGL to redraw the window
	glutPostRedisplay();
}

// this function gets called whenever the algorithm should do its update
void drawFuncs::update(int val) {
  int wait;  // time to wait between updates (milliseconds)
  
  if ( sys->running ) {
    if ( sys->slowNotFast == 1)
      wait = 10;
    else
      wait = 0;
    
    sys->Update();
  
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

      //sys->DrawSpheres();
      sys->DrawSquares();

      //  Swap contents of backward and forward frame buffers
      glutSwapBuffers();
}
