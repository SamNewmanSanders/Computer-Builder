#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "EditorState.h"
#include "SimModel.h"
#include "SimController.h"

class SimRenderer
{
public:

    SimRenderer(SimModel& model_, SimController& controller_, EditorState& editorState_, sf::RenderWindow& window_);

    void render();

private:

    void drawComponent(ComponentInfo& info, ComponentVisual& visual);
    void drawGrid();
    void drawConnection(ConnectionInfo& info, ConnectionVisual& visual);
    void drawInputPort(InputPort& inputPort);
    void drawOutputPort(OutputPort& outputPort);

    EditorState& editorState;
    SimModel& model;
    SimController& controller;
    sf::RenderWindow& window;

    sf::Font font;
};