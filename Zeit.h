#pragma once
class Zeit
{
public:
	static double deltat;
	static double time;
	static void tick() { time += deltat; }
};