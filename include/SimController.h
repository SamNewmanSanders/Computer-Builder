#pragma once

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include <TGUI/Backend/SFML-Graphics.hpp>

#include "SimModel.h"
#include "EditorState.h"
#include "ConnectionBuilder.h"

// Controller can modify the model

class SimController 
{
public:

    SimController(SimModel& model_, EditorState& editorState_, sf::RenderWindow& window_) 
                    : model(model_), editorState(editorState_), window(window_) {}

    void handleInputs(tgui::Gui& gui);

    // GUI setup
    void setupButtons(tgui::Gui& gui);


    ConnectionBuilder connectionBuilder;    // Let this be public so the connection being drawn can be accessed easily

private:

    void setupCircuitDropdown(tgui::Gui& gui);
    void refreshCircuitDropdown(tgui::ComboBox::Ptr circuitDropdown);
    void createFinishPopup(tgui::Gui& gui);

    // Specific event handlers
    void handleKeyPress(const sf::Event::KeyPressed& kp);
    void handleKeyRelease(const sf::Event::KeyReleased& kr);
    void handleMouseMove(const sf::Event::MouseMoved& mm);
    void handleMousePress(const sf::Event::MouseButtonPressed& mp);
    void handleMouseRelease(const sf::Event::MouseButtonReleased& mr);

    void toggleInputUnderMouse();

    EditorState& editorState;
    SimModel& model;
    sf::RenderWindow& window;

    tgui::ComboBox::Ptr circuitDropdown;     // Have this member as it needs to be updated as we add components
};