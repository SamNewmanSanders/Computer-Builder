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

    // Allow drawing of both port types
    void drawPort(InputPort& inputPort);
    void drawPort(OutputPort& outputPort);
    void drawPortHelper(sf::Vector2f pos, sf::Vector2f& outSize, bool isGhost, bool pinOnRight); // Avoid drawing logic repitition

    void drawPin(sf::Vector2f pos);

    EditorState& editorState;
    SimModel& model;
    SimController& controller;
    sf::RenderWindow& window;

    sf::Font font;
};