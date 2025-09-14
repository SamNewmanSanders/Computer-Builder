#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include "SimModel.h"

class SimRenderer
{
public:

    SimRenderer(SimModel& model_, EditorState& editorState_);

// Method which draws a component given its *index*

private:

    EditorState& editorState;
    SimModel& model;
};