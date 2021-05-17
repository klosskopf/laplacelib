#pragma once
#include <vector>
#include <iostream>
#include <string.h>
#include "Zeit.h"
class Defaultinblock;
class Block
{
protected:
	double out=0;
	std::vector<Block*> in;
	static Defaultinblock Defaultin;
public:
	virtual ~Block(){}
	virtual void magic()=0;
	virtual void addinput(int inputnr, Block& inputblock)
	{
		in[inputnr] = &inputblock;
	};
	double output()const { return out; };
};

class Defaultinblock :public Block
{
public:
	void magic() {};
};

class System: public Block
{
private:
	unsigned int gradb;
	unsigned int grada;
	double* y = NULL;//y(t) und seine Ableitungen
	double* u = NULL;//u(t) und seine Ableitungen
	double* b = NULL;
	double* a = NULL;
	void calculated(double Array[], double old, int bin, int bis) //Keine Ahnung, warum ich das damals old genannt hab. Eigentlich ist das der neue Wert
	{
		if (bin != bis)
		{
			calculated(Array, (old - Array[bin]) / Zeit::deltat, bin + 1, bis);
		}
		Array[bin] = old;
	};

public:
	System() { in.push_back(&Defaultin); }
	virtual ~System() {
		if (a != NULL)delete a;
		if (b != NULL)delete b;
		if (y != NULL)delete y;
		if (u != NULL)delete u;
	}

	void addparam(unsigned int gradb, unsigned int grada, const double* b, const double* a)
	{
		for (int i = gradb; (i > 0) && (b[i] == 0); i--)
		{
			gradb = i - 1;
		}
		for (int i = grada; (i > 0)&&(a[i] == 0); i--)
		{
			grada = i-1;
		}
		this->grada = grada;
		this->gradb = gradb;
		this->b = new double[gradb + 1];
		this->a = new double[grada + 1];
		this->u = new double[gradb + 1];
		this->y = new double[grada + 1];
		memcpy(this->b, b, (gradb + 1) * 8);
		memcpy(this->a, a, (grada + 1) * 8);
		memset(this->u, 0, (gradb + 1) * 8);
		memset(this->y, 0, (grada + 1) * 8);
	}

	void magic()
	{
		double newy;
		calculated(u, in[0]->output(), 0, gradb);
		newy = 0;
		for (unsigned int koefb = 0; koefb < (gradb + 1); koefb++)
		{
			newy += (b[koefb] * u[koefb]);
		}
		for (unsigned int koefa = 0; koefa < grada; koefa++)
		{
			newy -= (a[koefa] * y[koefa]);
		}
		y[grada] = newy / a[grada];
		for (unsigned int koefn = grada; koefn > 0; koefn--)
		{
			y[koefn - 1] += (y[koefn] * Zeit::deltat);
		}
		out=y[0];
	}
};

//taufloesung, yaufloesung, ymax
class Graph: public Block
{
private:
	const double taufloesung;
	const double yaufloesung;
	const int ymax;
	unsigned int chnr;
	const std::string chnnllkptbl[4] = {".","x","o","*"};
	
public:
	Graph(unsigned int chnr, double taufloesung, double yaufloesung, unsigned int ymax) : taufloesung(taufloesung), yaufloesung(yaufloesung), ymax(ymax)
	{ 
		if (chnr > 4)chnr = 4;
		this->chnr = chnr;
		for(unsigned int i=0; i<chnr; i++)in.push_back(&Defaultin);
	};
	void magic()
	{
		std::cout.setf(std::ios::fixed, std::ios::floatfield);
		std::cout.precision(4);
		std::string outputpixel;
		static double tdelay = 0;
		if (tdelay > taufloesung)
		{
			tdelay = 0;
			for (int i = -ymax; i < ymax; i++)
			{
				if (i == 0)
				{
					outputpixel = "|";
				}
				else
				{
					outputpixel = " ";
				}
				for (unsigned int eingang=0; eingang<chnr;eingang++)
				{
					if (i == (int)(in.at(eingang)->output() / yaufloesung))
					{
						outputpixel = chnnllkptbl[eingang];
					}
				}
				std::cout << outputpixel;
			}
			std::cout << Zeit::time << " ";
			for (Block* eingang:in)
			{
				std::cout << eingang->output() << " ";
			}
			std::cout << std::endl;
		}
		else
		{
			tdelay += Zeit::deltat;
		}
		out = in[0]->output();
	}
};

class ADD : public Block
{
private:
	double* parameter = NULL;
	double inputnr;
public:
	virtual ~ADD()
	{
		if (parameter!=NULL)delete parameter; 
	}
	void addparam(const unsigned int inputnr, const double* parameter)
	{
		in.clear();
		for (unsigned int i = 0; i < inputnr; i++)
		{
			in.push_back(&Defaultin);
		}
		this->inputnr = inputnr;
		this->parameter = new double[inputnr];
		memcpy(this->parameter, parameter, (inputnr) * 8);
	}
	void magic()
	{
		double zwischen = 0;
		for (int i = 0; i < inputnr; i++)
		{
			zwischen += (parameter[i]*in[i]->output());
		}
		out = zwischen;
	}

};


class Begrenzer : public Block
{
private:
	double max=10;
	double min=10;
public:
	Begrenzer(const double min, const double max) : max(max), min(min) { in.push_back(&Defaultin); }
	virtual ~Begrenzer() {}
	void magic()
	{
		double zwischen;
		zwischen = in[0]->output();
		if (zwischen > max)zwischen = max;
		if (zwischen < min)zwischen = min;
		out = zwischen;
	}
};

class PID : public System
{
private:
	using System::addparam;
public:
	PID(double kp, double Tn, double Tv, double Tr)
	{
		if ((Tn == 0) && (Tv == 0) && (Tr == 0))
		{
			double b[1] = { kp };
			double a[1] = { 1 };
			this->addparam(0, 0, b, a);
		}
		else if (Tn == 0)
		{
			double b[2] = { kp, kp*Tv };
			double a[2] = { 1, Tr };
			this->addparam(1, 1, b, a);
		}
		else
		{
			double b[3] = { kp, kp * Tn, kp * Tn * Tv };
			double a[3] = { 0, Tn, Tn * Tr };
			this->addparam(2, 2, b, a);
		}
	}
};

class Function : public Block
{
public:
	void magic()
	{
		if (Zeit::time > 0.3)out = 3;
		else out = 0;
	}
};