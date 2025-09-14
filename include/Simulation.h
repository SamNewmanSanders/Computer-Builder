#pragma once

#include "SimController.h"
#include "SimModel.h"
#include "SimRenderer.h"

class Simulation
{
public:
    
    Simulation();

    void run();

private:

    // Learnt that this order here matters, as this is the order they are constructed
    EditorState editorState;
    SimModel model;
    SimRenderer renderer;
    SimController controller;

    sf::RenderWindow window;
    tgui::Gui gui;
};