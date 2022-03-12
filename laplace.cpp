#include "laplace.h"
#include <string.h>
#include <iostream>

/**
 * @brief add a input block, but not all blocks use all inputs. Most only one
 * 
 * @param inputBlock the new inputBlock
 */
void clBlock::addInput(clBlock& inputBlock)
{
    inputs.push_back(&inputBlock);
}

/**
 * @brief return the current output
 * 
 * @return double value
 */
double clBlock::output()const
{
    return out;
}

/**
 * @brief calculate the derivative upto order
 * 
 * @param array array with old derivative values
 * @param newVal The new value
 * @param k The current level. Start with 0
 * @param order number of deviations to calculate
 * @param dt delta t
 */
void clSystem::derivate(double array[], double newVal, int k, int order, double dt)
{
    if (k != order)
    {
        derivate(array, (newVal - array[k]) / dt, k + 1, order, dt);
    }
    array[k] = newVal;
}

/**
 * @brief Instantiate a new Laplace-System
 * 
 * @param orderB order of the dividend-polynomial
 * @param orderA order of the divisor-polynomial
 * @param b coeffs (b0 is b[0])
 * @param a coeffs (a0 is a[0])
 */
clSystem::clSystem(unsigned int orderB, unsigned int orderA, const double* b, const double* a)
{
    for (int i = orderB; (i > 0) && (b[i] == 0); i--)
    {
        orderB = i - 1;
    }
    for (int i = orderA; (i > 0)&&(a[i] == 0); i--)
    {
        orderA = i-1;
    }
    this->orderA = orderA;
    this->orderB = orderB;
    this->b = new double[orderB + 1];
    this->a = new double[orderA + 1];
    this->u = new double[orderB + 1];
    this->y = new double[orderA + 1];
    memcpy(this->b, b, (orderB + 1) * 8);
    memcpy(this->a, a, (orderA + 1) * 8);
    memset(this->u, 0, (orderB + 1) * 8);
    memset(this->y, 0, (orderA + 1) * 8);
}

/**
 * @brief Destroy the cl System::cl System object
 * 
 */
clSystem::~clSystem() {
    if (a != NULL)delete a;
    if (b != NULL)delete b;
    if (y != NULL)delete y;
    if (u != NULL)delete u;
}

/**
 * @brief calculate a new system value
 * 
 * @param dt deltat
 */
void clSystem::magic(double dt)
{
    double newy;
    if (inputs.size()) derivate(u, inputs.front()->output(), 0, orderB, dt);
    newy = 0;
    for (unsigned int koefb = 0; koefb < (orderB + 1); koefb++)
    {
        newy += (b[koefb] * u[koefb]);
    }
    for (unsigned int koefa = 0; koefa < orderA; koefa++)
    {
        newy -= (a[koefa] * y[koefa]);
    }
    y[orderA] = newy / a[orderA];
    for (unsigned int koefn = orderA; koefn > 0; koefn--)
    {
        y[koefn - 1] += (y[koefn] * dt);
    }
    out=y[0];
}

/**
 * @brief Instantiate a new Graph (only ever use one)
 * 
 * @param tRes vertical resolution of one char (time flows down)
 * @param yRes horizontal resolution of one char
 * @param yMax Max expected horizontal value
 */
clGraph::clGraph(double tRes, double yRes, double yMax) : tRes(tRes), yRes(yRes), yMax(yMax/yRes){}

/**
 * @brief add a new input to graph
 * 
 * @param inputBlock the input block
 * @param marker the marker used in graph
 */
void clGraph::addInput(clBlock& inputBlock, const char marker)
{
    inputs.push_back(&inputBlock);
    markers.push_back(marker);
}

/**
 * @brief draw graph
 * 
 * @param dt 
 */
void clGraph::magic(double dt)
{
    time+=dt;
    std::cout.setf(std::ios::fixed, std::ios::floatfield);
    std::cout.precision(4);
    std::string outputPixel;
    static double tDelay = 0;
    if (tDelay > tRes)
    {
        tDelay = 0;
        for (int i = -yMax; i < yMax; i++)
        {
            if (i == 0)
            {
                outputPixel = "|";
            }
            else
            {
                outputPixel = " ";
            }
            int input=0;

            std::list<clBlock*>::iterator blockIt;
            std::list<char>::iterator markerIt;
            for (blockIt = inputs.begin(), markerIt = markers.begin(); (blockIt != inputs.end()) && (markerIt != markers.end()) ; blockIt++, markerIt++)
            {
                if ((i == (int)((*blockIt)->output() / yRes)) && (input < 4))
                {
                    outputPixel = (*markerIt);
                }
                input++;
            }
            std::cout << outputPixel;
        }
        std::cout << "time:" << time << " ";

        std::list<clBlock*>::iterator blockIt;
        std::list<char>::iterator markerIt;
        for (blockIt = inputs.begin(), markerIt = markers.begin(); (blockIt != inputs.end()) && (markerIt != markers.end()) ; blockIt++, markerIt++)
        {
            std::cout << (*markerIt) << ": " << (*blockIt)->output() << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        tDelay += dt;
    }
    if (inputs.size())out = inputs.front()->output();
}

/**
 * @brief add a input with gain 1
 * 
 * @param inputBlock input block
 */
void clAdd::addInput(clBlock& inputBlock)
{
    addInput(inputBlock, 1);
}

/**
 * @brief add a input with custom gain
 * 
 * @param inputBlock input block
 * @param gain 
 */
void clAdd::addInput(clBlock& inputBlock, const double gain)
{
    inputs.push_back(&inputBlock);
    gains.push_back(gain);
}

/**
 * @brief perform additions
 * 
 * @param dt deltat
 */
void clAdd::magic(double dt)
{
    double temp = 0;
    std::list<clBlock*>::iterator blockIt;
    std::list<double>::iterator gainIt;
    for (blockIt = inputs.begin(), gainIt = gains.begin(); (blockIt != inputs.end()) && (gainIt != gains.end()) ; blockIt++, gainIt++)
    {
        temp += ((*gainIt) * (*blockIt)->output());
    }
    out = temp;
}

/**
 * @brief Instantiate a new limiter
 * 
 * @param min Lower limit
 * @param max Upper limit
 */
clLimiter::clLimiter(const double min, const double max) : max(max), min(min) {}

/**
 * @brief perform limit
 * 
 * @param dt deltat
 */
void clLimiter::magic(double dt)
{
    double temp=0;
    if (inputs.size())temp = inputs.front()->output();
    if (temp > max)temp = max;
    if (temp < min)temp = min;
    out = temp;
}

/**
 * @brief initialize a PID System
 * 
 * @param Kp proportional coeff
 * @param Ki integral coeff
 * @param Kd derivative coeff
 */
clPid::clPid(double Kp, double Ki, double Kd)
{
    if ((Kp != 0) && (Ki == 0))
    {
        double b[2] = {Kp, Kd};
        double a[1] = { 1 };
        system = new clSystem(1,0,b,a);
    }
    else
    {
        double b[3] = {Ki, Kp, Kd};
        double a[2] = { 0, 1 };
        system = new clSystem(2,1,b,a);
    }
}

/**
 * @brief Destroy the cl Pid::cl Pid object
 * 
 */
clPid::~clPid()
{
    if(system) delete system;
    system = NULL;	
}

/**
 * @brief add a new input
 * 
 * @param inputBlock new input block
 */
void clPid::addInput(clBlock& inputBlock)
{
    if (system) system->addInput(inputBlock);
}

/**
 * @brief perform a pid-calculation
 * 
 * @param dt deltat
 */
void clPid::magic(double dt)
{
    system->magic(dt);
    out=system->output();
}

/**
 * @brief defines the function
 * 
 * @param dt deltat
 */
void clFunction::magic(double dt)
{
    time += dt;
    if (time > 0.3)out = 1;
    else out = 0;
}
