#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include "SimModel.h"

// Controller can modify the model

class SimController 
{
public:

    SimController(SimModel& model_, EditorState& editorState_);

    void handleInputs(sf::RenderWindow& window, SimModel& model);   // 

private:

    void handleKeyPress();
    void handleMouseMove();
    void handleMousePress();

    EditorState& editorState;
    SimModel& model;
};