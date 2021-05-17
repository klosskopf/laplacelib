#include <string>
#include "laplace.h"
#include "Zeit.h"

#define SIMDAUER 3

double Zeit::deltat = 0.000001;
double Zeit::time = 0;

int main()
{
	Function generator;
	
	
	/*System Strecke;
	double s_b[2] = {720.196, 4.423};
	double s_a[3] = {1164.532, 19.384, 1};
	Strecke.addparam(1, 2, s_b, s_a);

	
	System Regler;
	double r_b[3] = { 1280984.833, 21321.9, 1100};
	double r_a[4] = {0, 28808, 897, 4.423};
	Regler.addparam(2, 3, r_b, r_a);*/
	
	System Strecke;
	double s_b[1] = { 1.027 };
	double s_a[5] = { 1,0.3345, 0.03138, 0.0010193, 0.00001096};
	Strecke.addparam(0, 4, s_b, s_a);

	PID Regler(4.62, 0.17829, 0.12, 0.01);
	
	Begrenzer begrenzer(-4,4);

	ADD adder;
	double adderkoef[2] = { 1,-1 };
	adder.addparam(2, adderkoef);

	Graph graphA(2,0.01, 0.15, 40);

	graphA.addinput(0, begrenzer);
	graphA.addinput(1, Strecke);
	adder.addinput(0, generator);
	adder.addinput(1, Strecke);
	Regler.addinput(0, adder);
	begrenzer.addinput(0, Regler);
	Strecke.addinput(0, begrenzer);
	
	while (Zeit::time<SIMDAUER)
	{
		graphA.magic();
		adder.magic();
		Strecke.magic();
		Regler.magic();
		begrenzer.magic();
		generator.magic();
		Zeit::tick();
	}
	getchar();
}