#pragma once

#include <random>

class rnd
{
private:
	// nuts and bolts.. should not need to touch this.
	std::default_random_engine generator;
	int max;
	std::uniform_int_distribution<int> *intmax;
	std::uniform_real_distribution<double> *real01;

public:
	// constructor
	rnd() {
		max = 0x7fffffff;
		//cout << "genmax is " << generator.max() << endl;
		intmax = new std::uniform_int_distribution<int>(0, max);
		real01 = new std::uniform_real_distribution<double>(0.0, 1.0);
	}
	// destructor
	~rnd() { delete intmax; delete real01; }

	// set the random seed
	void   setSeed(int seed) { generator.seed(seed); }
	// member functions for generating random double in [0,1] and random integer in [0,max-1]
	double random01() { return (*real01)(generator); }
	int    randomInt(int max) { return (*intmax)(generator) % max; }
};

