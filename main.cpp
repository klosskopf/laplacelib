#include <string>
#include "laplace.h"

//This is a example showcase of a

#define MAXTIME 3
#define DELTAT 0.000001

int main()
{
	clFunction generator;
	double b[1] = {1.027};
	double a[5] = {1, 0.3345, 0.03138, 0.0010193, 0.00001096};
	clSystem system(0, 4, b, a);
	clPid controller(4.62,25.912,0.5544);
	clLimiter limiter(-5, 5);
	clAdd adder;
	clGraph graphA(0.02, 0.06, 5);

	graphA.addInput(limiter,'u');
	graphA.addInput(system,'y');
	adder.addInput(generator, 1);
	adder.addInput(system, -1);
	controller.addInput(adder);
	limiter.addInput(controller);
	system.addInput(limiter);

	for (double time = 0; time < MAXTIME; time += DELTAT)
	{
		graphA.magic(DELTAT);
		adder.magic(DELTAT);
		system.magic(DELTAT);
		controller.magic(DELTAT);
		limiter.magic(DELTAT);
		generator.magic(DELTAT);
	}
}