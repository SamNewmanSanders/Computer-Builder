#include "Simulation.h"

Simulation::Simulation() : editorState(), model(), renderer(model, editorState), controller(model, editorState)
{
    // Dummy circuit set up here etc
}

void Simulation::run()
{
    //stuff
}