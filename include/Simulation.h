#pragma once

#include "SimController.h"
#include "SimModel.h"
#include "SimRenderer.h"

class Simulation
{
public:
    
    Simulation(bool fullscreen);

    void run();

private:

    // Learnt that this order here matters, as this is the order they are constructed
    sf::RenderWindow window;
    tgui::Gui gui;
    
    EditorState editorState;
    SimModel model;
    SimRenderer renderer;
    SimController controller;
};