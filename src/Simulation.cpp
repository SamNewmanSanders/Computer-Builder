#include "Simulation.h"

Simulation::Simulation() : window(sf::VideoMode({1300, 800}), "My window"), editorState(), model(),
                            renderer(model, editorState, window), controller(model, editorState, window)
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

        if (editorState.updateSim)
        {
            model.stepNet(model.mainInst);
            editorState.updateSim = false;
        }

        window.clear(sf::Color::Black);
        renderer.render();
        gui.draw();
        window.display();
    }
}

