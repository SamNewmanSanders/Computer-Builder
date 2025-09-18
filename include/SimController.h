#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "SimModel.h"
#include "EditorState.h"

// Controller can modify the model

class SimController 
{
public:

    SimController(SimModel& model_, EditorState& editorState_, sf::RenderWindow& window_) 
                    : model(model_), editorState(editorState_), window(window_) {}

    void handleInputs(tgui::Gui& gui);

    // GUI setup
    void setupButtons(tgui::Gui& gui);

private:

    void setupCircuitDropdown(tgui::Gui& gui);
    void createFinishPopup(tgui::Gui& gui);

    // Specific event handlers
    void handleKeyPress(const sf::Event::KeyPressed& kp);
    void handleMouseMove(const sf::Event::MouseMoved& mm);
    void handleMousePress(const sf::Event::MouseButtonPressed& mp);

    void toggleInputUnderMouse();

    EditorState& editorState;
    SimModel& model;
    sf::RenderWindow& window;

    std::vector<tgui::Button::Ptr> buttons; // Store the list of buttons - may be useful later

};