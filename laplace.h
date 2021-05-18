#ifndef LAPLACE_H
#define LAPLACE_H

#include <list>
#include <string>

class Block
{
protected:
	double out=0;
	std::list<Block*> in;
public:
	virtual void magic(double dt)=0;
	virtual void addinput(Block& inputblock);
	double output()const;
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
	void derivate(double Array[], double newval, int bin, int bis, double dt);
public:
	System(unsigned int gradb, unsigned int grada, const double* b, const double* a);
	virtual ~System();
	void magic(double dt);
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
	double time=0;
public:
	Graph(unsigned int chnr, double taufloesung, double yaufloesung, unsigned int ymax);
	void magic(double dt);
};

class ADD : public Block
{
private:
	std::list<double> gains;
	double inputnr;
public:
	void addinput(Block& inputblock, const double gain);
	void magic(double dt);
};

class Begrenzer : public Block
{
private:
	double max=10;
	double min=10;
public:
	Begrenzer(const double min, const double max);
	void magic(double dt);
};

class PID : public Block
{
private:
	System* system=NULL;
public:
	PID(double kp, double Tn, double Tv, double Tr);
	virtual ~PID();
	virtual void addinput(Block& inputblock) override;
	void magic(double dt);
};

class Function : public Block
{
private:
	double time=0;
public:
	void magic(double dt);
};

#endif //LAPLACE_H