#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "SimModel.h"

// Controller can modify the model

class SimController 
{
public:

    SimController(SimModel& model_, EditorState& editorState_);

    void handleInputs(sf::RenderWindow& window, tgui::Gui& gui);

private:

    void handleKeyPress(const sf::Event::KeyPressed& kp);
    void handleMouseMove(const sf::Event::MouseMoved& mm);
    void handleMousePress(const sf::Event::MouseButtonPressed& mp);

    EditorState& editorState;
    SimModel& model;
};