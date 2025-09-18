#include "Simulation.h"

Simulation::Simulation() : window(sf::VideoMode({1300, 800}), "My window"), editorState(), model(),
                            controller(model, editorState, window), renderer(model, controller, editorState, window)
{
    gui.setTarget(window);
    window.setKeyRepeatEnabled(false);
    controller.setupButtons(gui);
}

void Simulation::run()
{
    while (window.isOpen())
    {

        controller.handleInputs(gui);

        if (editorState.stepSim)
        {
            model.stepNet(model.def, model.state);
            editorState.stepSim = false;
        }
        if (editorState.runSim)
        {
            model.stepNet(model.def, model.state);
        }

        window.clear(sf::Color::Black);
        renderer.render();
        gui.draw();
        window.display();
    }
}

