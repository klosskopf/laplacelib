#include "laplace.h"
#include <string.h>
#include <iostream>

void Block::addinput(Block& inputblock)
{
    in.push_back(&inputblock);
}
double Block::output()const
{
    return out;
}

void System::derivate(double Array[], double newval, int bin, int bis, double dt)
{
    if (bin != bis)
    {
        derivate(Array, (newval - Array[bin]) / dt, bin + 1, bis, dt);
    }
    Array[bin] = newval;
}
System::System(unsigned int gradb, unsigned int grada, const double* b, const double* a)
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
System::~System() {
    if (a != NULL)delete a;
    if (b != NULL)delete b;
    if (y != NULL)delete y;
    if (u != NULL)delete u;
}
void System::magic(double dt)
{
    double newy;
    if (in.size())derivate(u, in.front()->output(), 0, gradb, dt);
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
        y[koefn - 1] += (y[koefn] * dt);
    }
    out=y[0];
}

Graph::Graph(double taufloesung, double yaufloesung, unsigned int ymax) : taufloesung(taufloesung), yaufloesung(yaufloesung), ymax(ymax){}
void Graph::magic(double dt)
{
    time+=dt;
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
            int eingang=0;
            for (Block* block : in)
            {
                if ((i == (int)(block->output() / yaufloesung)) && (eingang < 4))
                {
                    outputpixel = chnnllkptbl[eingang];
                }
                eingang++;
            }
            std::cout << outputpixel;
        }
        std::cout << time << " ";
        for (Block* eingang:in)
        {
            std::cout << eingang->output() << " ";
        }
        std::cout << std::endl;
    }
    else
    {
        tdelay += dt;
    }
    if (in.size())out = in.front()->output();
}

void ADD::addinput(Block& inputblock)
{
    addinput(inputblock, 1);
}
void ADD::addinput(Block& inputblock, const double gain)
{
    in.push_back(&inputblock);
    gains.push_back(gain);
}
void ADD::magic(double dt)
{
    double zwischen = 0;
    int i=0;
    std::list<Block*>::iterator blockit;
    std::list<double>::iterator gainit;
    for (blockit = in.begin(), gainit = gains.begin(); (blockit != in.end()) && (gainit != gains.end()) ; blockit++, gainit++)
    {
        zwischen += ((*gainit) * (*blockit)->output());
        if (++i==inputnr) break;
    }
    out = zwischen;
}

Begrenzer::Begrenzer(const double min, const double max) : max(max), min(min) {}
void Begrenzer::magic(double dt)
{
    double zwischen=0;
    if (in.size())zwischen = in.front()->output();
    if (zwischen > max)zwischen = max;
    if (zwischen < min)zwischen = min;
    out = zwischen;
}

PID::PID(double kp, double Tn, double Tv, double Tr)
{
    if ((Tn == 0) && (Tv == 0) && (Tr == 0))
    {
        double b[1] = { kp };
        double a[1] = { 1 };
        system = new System(0,0,b,a);
    }
    else if (Tn == 0)
    {
        double b[2] = { kp, kp*Tv };
        double a[2] = { 1, Tr };
        system = new System(1,1,b,a);
    }
    else
    {
        double b[3] = { kp, kp * Tn, kp * Tn * Tv };
        double a[3] = { 0, Tn, Tn * Tr };
        system = new System(2,2,b,a);
    }
}
PID::~PID()
{
    if(system) delete system;
    system = NULL;	
}
void PID::addinput(Block& inputblock)
{
    if (system) system->addinput(inputblock);
}
void PID::magic(double dt)
{
    system->magic(dt);
    out=system->output();
}

void Function::magic(double dt)
{
    time += dt;
    if (time > 0.3)out = 3;
    else out = 0;
}
