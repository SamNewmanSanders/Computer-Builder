#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "SimModel.h"

class SimRenderer
{
public:

    SimRenderer(SimModel& model_, EditorState& editorState_, sf::RenderWindow& window_);

    void render();

// Method which draws a component given its *index*

private:

    void drawComponent(ComponentInfo& info, ComponentVisual& visual);

    EditorState& editorState;
    SimModel& model;
    sf::RenderWindow& window;
};