#include <GL/glut.h>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <math.h>
#include <string>
#include <unistd.h>

#include "DLASystem.h"
#include "Window.h" 

using namespace std;

// functions which are needed for openGL go into a namespace so that we can identify them
namespace drawFuncs {
  void handleKeypress(unsigned char key, int x, int y);
  void display(void);
  void update(int val);
  void introMessage();
}

//forward declaration
void multipleSimulation (DLASystem* sys, int numSims, pair<int,int> range, int interval);

// this is a global pointer, which is how we access the system itself
DLASystem *sys;

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

  // create the system
  sys = new DLASystem(win);

  //set background to white by default
  sys->setWinBackgroundWhite();
  
  // this is the seed for the random numbers
  
  //int seed = 6;
  //cout << "setting seed " << seed << endl;
  //sys->setSeed(seed);
  
  //use true random number as seed
  cout << "setting random seed" << endl;
  sys->setRandomSeed();

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
        cout << "  0 to print number of particles and radius" << endl;
        cout << "  m to save current recorded data to './data.csv' " << endl;
        cout << "  v to change view zoom level" << endl;
        cout << "  t to setup data recording" << endl;
        cout << "  j to set sticking chance" << endl;
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
    if(sys->slowNotFast <= 0) sys->setSuperFast();
    else sys->setFast();
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
  //end recording data (use when STOP)
  case 'm':{
    cout << "writing current data to csv & ending recording" << endl;
    sys->writeDataCSV();
    break;}
  case 'v':
    cout << "input viewsize: ";
    double vwsze;
    cin >> vwsze;
    //tried to do a zoom-in but it doesnt work
    if(vwsze == 0){
      for (int i = static_cast<int>(sys->getViewSize()); i >= 40; i -= 10){
        usleep(100000);
        sys->setViewSize(i);
	      glutPostRedisplay();
      }
    }
    else sys->setViewSize(vwsze);

  //set sticking chance
  case 'j':{
    cout << "input probability of particle sticking on contact" << endl;

    double prob;
    cin >> prob;
    if(prob >= 1){
      sys->randomStick = false;
      sys->stickChance = 1.0;
    }else{
      sys->randomStick = true;
      sys->stickChance = prob;
    }
    cout << "sticking probability set to " << sys->stickChance << endl;
    break;
  }
  //perform multiple simulations
  case 't':{
    cout << "Set up recording parameters:" << endl;

    cout << "input lower bound for number of particles: ";
    int r_low;
    cin >> r_low;
    cout << "lower bound set as: "<< r_low << endl;

    cout << "input upper bound for number of particles: ";
    int r_high;
    cin >> r_high;
    cout << "upper bound set as: "<< r_high << endl;

    pair<int, int> range = {r_low, r_high};

    cout << "input recording interval for number of particles: ";
    int intrvl;
    cin >> intrvl;
    cout << "interval set as: "<< intrvl << endl;

    cout << "input number of simulations to perform: ";
    int nSims;
    cin >> nSims;
    cout << "number of simulations to perform set as: "<< nSims << endl;

    cout << "input initial probability of particle sticking on contact" << endl;
    double prob;
    cin >> prob;
    if(prob >= 1){
      sys->randomStick = false;
      sys->stickChance = 1.0;
    }else{
      sys->randomStick = true;
      sys->stickChance = prob;
    }
    cout << "initial sticking probability set to " << sys->stickChance << endl;

    cout << "input amount to change probability of particle sticking on contact per simulation" << endl;
    double prob_diff;
    cin >> prob_diff;
    if(prob_diff >= 1 || prob_diff <=0){
      sys->stickDiff = 0;
    }else{
      sys->randomStick = true;
      sys->stickDiff = prob_diff;
    }
    cout << "sticking probability interval set to " << sys->stickDiff << endl;

    cout << "input number of simulations to perform before lowering stick probability" << endl;
    double sim_stick_num;
    cin >> sim_stick_num;
    if(sim_stick_num >= 1){
      sys->simStickNum = sim_stick_num;
    }else{
      sys->simStickNum = 0;
    }
    cout << "simStickNum set to " << sys->simStickNum << endl;

    // setup data recording
    sys->recordData(nSims, intrvl, range);

    cout << "input any int to start" << endl;
    int tmp;
    cin >> tmp;

    sys->setFast();
    sys->setRunning();
    glutTimerFunc(0, drawFuncs::update, 0);
    break;
  }
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
