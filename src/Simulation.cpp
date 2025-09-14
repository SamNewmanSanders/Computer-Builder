#include "Simulation.h"

Simulation::Simulation() : editorState(), model(),
                            window(sf::VideoMode({1300, 800}), "My window"),
                            renderer(model, editorState, window), controller(model, editorState)
{
    gui.setTarget(window);
    window.setKeyRepeatEnabled(false);
}

void Simulation::run()
{
    while (window.isOpen())
    {
        controller.handleInputs(window, gui);

        if (editorState.updateSim)
        {
            model.stepNet(model.mainInst);
            editorState.updateSim = false;
        }

        window.clear();
        renderer.render();
        gui.draw();
        window.display();
    }
}

