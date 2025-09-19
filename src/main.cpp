#include "Simulation.h"

#include <iostream>


int main()
{
    Simulation sim(true);   // Bool fullscreen
    sim.run();
    std::cout << "Simulation exited succesfully." << std::endl;
    return 0;
}