#ifndef LAPLACE_H
#define LAPLACE_H

#include <list>
#include <string>

//Base building block
class clBlock
{
protected:
	double out=0;
	std::list<clBlock*> inputs;
public:
	double output()const;
	virtual void magic(double dt)=0;			//this computes the block. Call it once every cycle. The order has a effect, but it should be neglectible
	virtual void addInput(clBlock& inputBlock);	//adds another input to the block. Responisibility is not transfered. You still need to delete every single one
};

//Laplace-System
class clSystem: public clBlock
{
private:
	unsigned int orderB; //number of b coeffs -1
	unsigned int orderA; //number of a coeffs -1
	double* y = NULL; //y(0) and its derivatives
	double* u = NULL; //u(0) and its derivatives
	double* b = NULL; //b-coefficients
	double* a = NULL; //a-coefficients
	void derivate(double array[], double newVal, int k, int order, double dt);
public:
	clSystem(unsigned int orderB, unsigned int orderA, const double* b, const double* a);
	virtual ~clSystem();
	void magic(double dt);
};

//Comandline-Plot
class clGraph: public clBlock
{
private:
	const double tRes;
	const double yRes;
	const int yMax;private:
	std::list<char> markers;
	double time=0;
public:
	clGraph(double tRes, double yRes, double yMax);
	void addInput(clBlock& inputblock, const char marker);
	void magic(double dt);
};

//Addition, Gain and Substraction
class clAdd : public clBlock
{
private:
	std::list<double> gains;
public:
	void addInput(clBlock& inputBlock) override;
	void addInput(clBlock& inputBlock, const double gain);
	void magic(double dt);
};

//Limit input Value
class clLimiter : public clBlock
{
private:
	double max=10;
	double min=10;
public:
	clLimiter(const double min, const double max);
	void magic(double dt);
};

//parametrize a System as a PID
class clPid : public clBlock
{
private:
	clSystem* system=NULL;
public:
	clPid(double Kp, double Ki, double Kd);
	virtual ~clPid();
	virtual void addInput(clBlock& inputBlock) override;
	void magic(double dt);
};

//Outputs a function
class clFunction : public clBlock
{
private:
	double time=0;
public:
	void magic(double dt);
};

#endif //LAPLACE_H