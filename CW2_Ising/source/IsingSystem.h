#pragma once

#include <GL/glut.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>
#include <string>
#include <sstream>

#include "Window.h"
#include "rnd.h"

#include "CSVWrite.h"

using namespace std;

// IsingSystem class
// this is where the Ising simulation happens
class IsingSystem {
private:
	Window *win;  // pointer to the window in which the system is running
				  // (this is not actually used but we keep it, just in case)

				  // size of grid: "static const" means that this number is a fixed constant
	static const int gridSize = 40;
	int **grid;  // this will be a 2d array that stores whether each site is occupied

				 // random number generator, class name is rnd, instance is rgen
	rnd rgen;

	// this variable is 1 if the system is running, 0 for paused
	int isActive;

	// this variable is 1 if we are in "slow" mode, 0 in fast mode
	int slowNotFast;

	// this is the inverse temperature parameter beta
	double inverseTemperatureBeta;

	// this an output file (in case we need one)
	ofstream logfile;

  // vector of every position in grid
  // NOT DOABLE due to pos being of type int[]
  vector<int> positions;
  
  int tmpPos[2];

  // data recording parameters:
  bool isRecording;

  // number of initial sweeps to ignore
  int initialSweeps;

  // interval between measurements
  int sweepInterval;

  // number of sweeps performed
  int numSweeps;

  // number of sims performed
  int numSims;

  // number to perform before Reset() or write data, 1 indexed
  int maxSweeps;
  int maxSims;

  // recording data:
  vector<double>* sweepData;
  vector<double>* magnetisationData;
  vector<double>* energyData;

  //2d dataSet to write to csv
  vector<vector<double>>* dataSet;

public:
	// constructor
	IsingSystem(Window *set_win);
	// destructor
	~IsingSystem();

	// this sets the seed for the random numbers
	void setSeed(int s) { rgen.setSeed(s); }

	// these functions are used to update the system
	double computeLocalField(int pos[]);
	void attemptSpinFlip();
	void MCsweep();

	// increase or decrease the temperature (arbitrary increments)
	void Hotter() { inverseTemperatureBeta -= 0.05; }
	void Colder() { inverseTemperatureBeta += 0.05; }

	void setTemperature(double TT) { inverseTemperatureBeta = 1.0 / TT; }

	// set and read grid entries associated with a given position
	// NOTE! the grid is not accessed directly, only via these functions
	void setGrid(int pos[], int val);
	int readGrid(int pos[]);
	void flipSpin(int pos[]);

	// reset temperature to default value and re-initialise the grid
	void Reset();

	// "user interface" for the isActive variable
	int  isRunning() { return isActive; }
	void setRunning() { isActive = 1; }
	void pauseRunning() { isActive = 0; }

	// "user interface" for the slowNotFast variable
	int isSlow() { return slowNotFast; }
	void setSlow() { slowNotFast = 1; }
	void setFast() { slowNotFast = 0; }

	// set setpos to the position of a neighbour of pos
	// the neighbour to use is determined by val (=0,1,2,3)
	// beware: behavior is not defined for val>=4 or val<0
	void setPosNeighbour(int setpos[], int pos[], int val);

	// update the system: makes a Monte Carlo sweep
	void Update();

	// draws the system as squares
	void DrawSquares();

  // functions written for CW:
  
  /* Not used
  // vector of every grid value from a given list of positions
  // default all positions
  vector<int> getFullGridValues();
  vector<int> getFullGridValues(vector<int*> positions);
  */

  // ints to int pos[2]
  int* toPos(int x, int y){
    tmpPos[0] = x; tmpPos[1]=y;
    return tmpPos;
  };

  // sim num diagnostics
  void printSimInfo(){
    cout << "numSims = " << numSims << ", numSweeps = " << numSweeps << endl;
  };

  // return system magnetisation
  double getMagnetisation();

  //all energy values dimensionless
  //sum over nearest neighbours at pos
  int getPosEnergy(int pos[]);

  //energy of whole system (/N i.e per spin)
  //double counts over nearest neighbours
  double getEnergy();

  // manage data recording
  // record s sims for n sweeps past n0 with interval nInt
  void setRecording(int sims, int n0, int n, int nInt);
  
  void setRecording(bool r){isRecording = r;};
  
  void updateSimRecording();

  void endRecording();
};

