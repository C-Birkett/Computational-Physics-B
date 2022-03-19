//
//  DLASystem.cpp
//

#include "DLASystem.h"

// colors
namespace colours {
	GLfloat blue[] = { 0.1, 0.3, 0.9, 1.0 };   // blue
	GLfloat red[] = { 1.0, 0.2, 0.1, 0.2 };   // red
	GLfloat green[] = { 0.3, 0.6, 0.3, 1.0 };     // green
	GLfloat paleGrey[] = { 0.7, 0.7, 0.7, 1.0 };     // green
	GLfloat darkGrey[] = { 0.2, 0.2, 0.2, 1.0 };     // green
}


// this function gets called every step,
//   if there is an active particle then it gets moved,
//   if not then add a particle
void DLASystem::Update() {
	if (lastParticleIsActive == 1)
		moveLastParticle();
	else if (numParticles < nRange.second) {
    if(recording) updateRecording();
		addParticleOnAddCircle();
		setParticleActive();
	}
	else if (numParticles == nRange.second) {if(recording) updateRecording();}
	if (lastParticleIsActive == 0 || slowNotFast == 1)
		glutPostRedisplay(); //Tell GLUT that the display has changed
}


void DLASystem::clearParticles() {
	// delete particles and the particle list
	for (int i = 0; i < numParticles; i++) {
		delete particleList[i];
	}
	particleList.clear();
	numParticles = 0;
}

// remove any existing particles and setup initial condition
void DLASystem::Reset() {
	// stop running
	running = 0;

	clearParticles();

	lastParticleIsActive = 0;

	// set the grid to zero
	for (int i = 0; i < gridSize; i++) {
		for (int j = 0; j < gridSize; j++) {
			grid[i][j] = 0;
		}
	}

	// setup initial condition and parameters
	addCircle = 10;
	killCircle = 2.0*addCircle;
	clusterRadius = 0.0;
	// add a single particle at the origin
	double pos[] = { 0.0, 0.0 };
	addParticle(pos);

	// set the view
	int InitialViewSize = 40;
	setViewSize(InitialViewSize);

  //clear temp data arrays
  //dataSet cleared separately as needs to persist between sims
  numData->clear();
  radiusData->clear();
}

// set the value of a grid cell for a particular position
// note the position has the initial particle at (0,0)
// but this corresponds to the middle of the grid array ie grid[ halfGrid ][ halfGrid ]
void DLASystem::setGrid(double pos[], int val) {
	int halfGrid = gridSize / 2;
	grid[(int)(pos[0] + halfGrid)][(int)(pos[1] + halfGrid)] = val;
}

// read the grid cell for a given position
int DLASystem::readGrid(double pos[]) {
	int halfGrid = gridSize / 2;
	return grid[(int)(pos[0] + halfGrid)][(int)(pos[1] + halfGrid)];
}

// check if the cluster is big enough and we should stop:
// to be safe, we need the killCircle to be at least 2 less than the edge of the grid
int DLASystem::checkStop() {
	if (killCircle + 2 >= gridSize / 2) {
		pauseRunning();
		cout << "STOP" << endl;
		glutPostRedisplay(); // update display
    //if(recording) writeDataCSV();  //write data up to this point
    //use m key for this
		return 1;
	}
	else return 0;
}

// add a particle to the system at a specific position
void DLASystem::addParticle(double pos[]) {
	// create a new particle
	Particle * p = new Particle(pos);
	// push_back means "add this to the end of the list"
	particleList.push_back(p);
	numParticles++;

	// pos coordinates should be -gridSize/2 < x < gridSize/2
	setGrid(pos, 1);
}

// add a particle to the system at a random position on the addCircle
// if we hit an occupied site then we do nothing except print a message
// (this should never happen)
void DLASystem::addParticleOnAddCircle() {
	double pos[2];
	double theta = rgen.random01() * 2 * M_PI;
	pos[0] = ceil(addCircle * cos(theta));
	pos[1] = ceil(addCircle * sin(theta));
	if (readGrid(pos) == 0)
		addParticle(pos);
	else
		cout << "FAIL " << pos[0] << " " << pos[1] << endl;
}

// send back the position of a neighbour of a given grid cell
// NOTE: there is no check that the neighbour is inside the grid,
// this has to be done separately...
void DLASystem::setPosNeighbour(double setpos[], double pos[], int val) {
	switch (val) {
	case 0:
		setpos[0] = pos[0] + 1.0;
		setpos[1] = pos[1];
		break;
	case 1:
		setpos[0] = pos[0] - 1.0;
		setpos[1] = pos[1];
		break;
	case 2:
		setpos[0] = pos[0];
		setpos[1] = pos[1] + 1.0;
		break;
	case 3:
		setpos[0] = pos[0];
		setpos[1] = pos[1] - 1.0;
		break;
	}
}

// if the view is smaller than the kill circle then increase the view area (zoom out)
void DLASystem::updateViewSize() {
	double mult = 1.2;
	if (viewSize < 2.0*killCircle) {
		setViewSize(viewSize * mult);
	}
}

// set the view to be the size of the add circle (ie zoom in on the cluster)
void DLASystem::viewAddCircle() {
	setViewSize(2.0*addCircle);  // factor of 2 is to go from radius to diameter
}

// when we add a particle to the cluster, we should update the cluster radius
// and the sizes of the addCircle and the killCircle
void DLASystem::updateClusterRadius(double pos[]) {

	double rr = distanceFromOrigin(pos);
	if (rr > clusterRadius) {
		clusterRadius = rr;
		// this is how big addCircle is supposed to be:
		//   either 20% more than cluster radius, or at least 5 bigger.
		double check = clusterRadius * addRatio;
		if (check < clusterRadius + 5)
			check = clusterRadius + 5;
		// if it is smaller then update everything...
		if (addCircle < check) {
			addCircle = check;
			killCircle = killRatio * addCircle;
			updateViewSize();
		}
		checkStop();
	}
}

// make a random move of the last particle in the particleList
void DLASystem::moveLastParticle() {
	int rr = rgen.randomInt(4);  // pick a random number in the range 0-3, which direction do we hop?
	double newpos[2];

	Particle *lastP = particleList[numParticles - 1];

	setPosNeighbour(newpos, lastP->pos, rr);

	if (distanceFromOrigin(newpos) > killCircle) {
		//cout << "#deleting particle" << endl;
		setGrid(lastP->pos, 0);
		particleList.pop_back();  // remove particle from particleList
		numParticles--;
		setParticleInactive();
	}
	// check if destination is empty
	else if (readGrid(newpos) == 0) {
		setGrid(lastP->pos, 0);  // set the old grid site to empty
		// update the position
		particleList[numParticles - 1]->pos[0] = newpos[0];
		particleList[numParticles - 1]->pos[1] = newpos[1];
		setGrid(lastP->pos, 1);  // set the new grid site to be occupied

		// check if we stick
		if (checkStick()) {
			//cout << "stick" << endl;
			setParticleInactive();  // make the particle inactive (stuck)
			updateClusterRadius(lastP->pos);  // update the cluster radius, addCircle, etc.

			if (numParticles % 100 == 0 && logfile.is_open()) {
				logfile << numParticles << " " << clusterRadius << endl;
			}
		}
	}
	else {
		// if we get to here then we are trying to move to an occupied site
		// (this should never happen as long as the sticking probability is 1.0)
		//cout << "reject " << rr << endl;
		//cout << lastP->pos[0] << " " << lastP->pos[1] << endl;
		//cout << newpos[0] << " " << newpos[1] << " " << (int)newpos[0] << endl;
		//printOccupied();
    
    //don't update position, checkstick and if false wait for next update
    //cout << "trying to move into adjacent particle - chcking stick" << endl;
    if(checkStick()){
			setParticleInactive();  // make the particle inactive (stuck)
			updateClusterRadius(lastP->pos);  // update the cluster radius, addCircle, etc.

			if (numParticles % 100 == 0 && logfile.is_open()) {
				logfile << numParticles << " " << clusterRadius << endl;
			}
    }
	}
}

// check if the last particle should stick (to a neighbour)
int DLASystem::checkStick() {
	Particle *lastP = particleList[numParticles - 1];
	int result = 0;
	// loop over neighbours
	for (int i = 0; i < 4; i++) {
		double checkpos[2];
		setPosNeighbour(checkpos, lastP->pos, i);
		// if the neighbour is occupied...
		if (readGrid(checkpos) == 1)
      if(randomStick){
	      double rr = static_cast<double>(rgen.randomInt(1000));
        if (static_cast<double>(rr/1000.0) < stickChance){result = 1;}
        else {result = 0;}
      }
      else {result = 1;}
	}
	return result;
}


// constructor
DLASystem::DLASystem(Window *set_win) {
	cout << "creating system, gridSize " << gridSize << endl;
	win = set_win;
	numParticles = 0;

	// allocate memory for the grid, remember to free the memory in destructor
	grid = new int*[gridSize];
	for (int i = 0; i < gridSize; i++) {
		grid[i] = new int[gridSize];
	}
	slowNotFast = 1;

  //init arrays to store data
  numData = new vector<double>;
  radiusData = new vector<double>;

  dataSet = new vector<vector<double>>;

	// reset initial parameters
	Reset();

	addRatio = 1.2;   // how much bigger the addCircle should be, compared to cluster radius
	killRatio = 1.7;   // how much bigger is the killCircle, compared to the addCircle

	// this opens a logfile, if we want to...
	//logfile.open("opfile.txt");
}

// destructor
DLASystem::~DLASystem() {
	// strictly we should not print inside the destructor but never mind...
	cout << "deleting system" << endl;
	// delete the particles
	clearParticles();
	// delete the grid
	for (int i = 0; i < gridSize; i++)
		delete[] grid[i];
	delete[] grid;

	if (logfile.is_open())
		logfile.close();
}

// this draws the system
void DLASystem::DrawSquares() {

	// draw the particles
	double halfSize = 0.5;
	for (int p = 0; p < numParticles; p++) {
		double *vec = particleList[p]->pos;
		glPushMatrix();
		if (p == numParticles - 1 && lastParticleIsActive == 1)
			glColor4fv(colours::red);
		else if (p == 0)
			glColor4fv(colours::green);
		else
			glColor4fv(colours::blue);
		glRectd(drawScale*(vec[0] - halfSize),
			drawScale*(vec[1] - halfSize),
			drawScale*(vec[0] + halfSize),
			drawScale*(vec[1] + halfSize));
		glPopMatrix();
	}

	// print some information (at top left)
	// this ostringstream is a way to create a string with numbers and words (similar to cout << ... )
	ostringstream str;
	str << "num " << numParticles << " size " << clusterRadius;

	// print the string
	win->displayString(str, -0.9, 0.9, colours::red);

	// if we are paused then print this (at bottom left)
	if (running == 0) {
		ostringstream pauseStr;
		pauseStr << "paused";
		win->displayString(pauseStr, -0.9, -0.9, colours::red);
	}
}

void DLASystem::recordData(int numOfSims, int nInterval_, pair<int, int> nRange_) {
  if(running != 0 && !recording){cout << "please pause simulation before recording data" << endl;}
  else{ 
    if (numOfSims >= 2){
      cout << "Starting DLA multiple recording" << endl;
      simNum = numOfSims;
      numSims = numOfSims;
    }else cout << "Starting DLA recording" << endl;
    
    recording = true;

    nInterval = nInterval_;
    nRange = nRange_;
  }
}

//record data on update
void DLASystem::updateRecording() {
  if(numParticles == nRange.second){
    //cout << "recording data at N = " << numParticles << endl;
    numData->push_back(static_cast<double>(numParticles));
    radiusData->push_back(clusterRadius);

    if (simNum == numSims) {dataSet->push_back(*numData);} //only need one column of this

    //when finished in range write data to dataSet & reset sim
    if (simNum >= 1){
      dataSet->push_back(*radiusData);
      cout << "Sucessfully recorded sim number " << simNum << " data in range (" << nRange.first << ", " << nRange.second << ") with interval " << nInterval << endl;

      //reset initial conditions
      Reset(); //will reset numData and radiusData
      setRandomSeed();
      setFast();

      if (simStickNum != 0 && simNum % simStickNum == 1 && simNum != numSims){
        if (stickDiff != 0){
          stickChance -= stickDiff;
          if(stickChance<=0){
            cout << "sticking chance is now 0, finishing recording" << endl;
            writeDataCSV();
            recording = false;
            pauseRunning();
          }
          else cout << "new sticking chance set to " << stickChance << endl;
        }
      }

      setRunning();

      if (simNum == 1) {writeDataCSV();}

      simNum--;
      //cout << "conditions reset for simulation number " << simNum << endl;
    }

    if (simNum <= 0) {recording = false; pauseRunning();}
  }
  else if(numParticles > nRange.first
          && numParticles % nInterval == 0
          && numParticles > numData->size() * nInterval + nRange.first){
    //cout << "recording data at N = " << numParticles << endl;
    numData->push_back(static_cast<double>(numParticles)); //dont really need to do this...
    radiusData->push_back(clusterRadius);
  }
}

//write recorded data to a CSV file
void DLASystem::writeDataCSV(){
  auto csv = new CSVWrite("./data.csv");

  csv->WriteVector(dataSet);
  csv->CSVClose();

  //clearDataSet();

  recording = false; //should be false anyways
}
