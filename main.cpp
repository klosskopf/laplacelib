#include <string>
#include "laplace.h"

#define SIMDAUER 3
#define DELTAT 0.000001

int main()
{
	Function generator;
	double s_b[1] = { 1.027 };
	double s_a[5] = { 1,0.3345, 0.03138, 0.0010193, 0.00001096};
	System Strecke(0, 4, s_b, s_a);
	PID Regler(4.62, 0.17829, 0.12, 0.01);
	Begrenzer begrenzer(-4,4);
	ADD adder;
	Graph graphA(2,0.01, 0.15, 40);

	graphA.addinput(begrenzer);
	graphA.addinput(Strecke);
	adder.addinput(generator,1);
	adder.addinput(Strecke,-1);
	Regler.addinput(adder);
	begrenzer.addinput(Regler);
	Strecke.addinput(begrenzer);
	
	for (double time =0; time < SIMDAUER; time+=DELTAT)
	{
		graphA.magic(DELTAT);
		adder.magic(DELTAT);
		Strecke.magic(DELTAT);
		Regler.magic(DELTAT);
		begrenzer.magic(DELTAT);
		generator.magic(DELTAT);
	}
	getchar();
}