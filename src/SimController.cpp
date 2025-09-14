#include "SimController.h"

SimController::SimController(SimModel& model_, EditorState& editorState_) : model(model_), editorState(editorState) {}

void SimController::handleInputs(sf::RenderWindow& window, tgui::Gui& gui)
{
    while (const std::optional event = window.pollEvent())
    {
        gui.handleEvent(*event);

        // Close window
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            continue;
        }

        if (auto* kp = event->getIf<sf::Event::KeyPressed>())
            handleKeyPress(*kp);
        else if (auto* mm = event->getIf<sf::Event::MouseMoved>())
            handleMouseMove(*mm);
        else if (auto* mp = event->getIf<sf::Event::MouseButtonPressed>())
            handleMousePress(*mp);
    }
}

void SimController::handleKeyPress(const sf::Event::KeyPressed& kp)
{
    if (kp.code == sf::Keyboard::Key::S)
        {
            editorState.updateSim = true;
        }
}




void SimController::handleMouseMove(const sf::Event::MouseMoved& mm){}
void SimController::handleMousePress(const sf::Event::MouseButtonPressed& mp){}