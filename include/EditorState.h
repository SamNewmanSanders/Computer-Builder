#pragma once

#include <SFML/Graphics.hpp>

#include <optional>

#include "Netlist.h"

enum class EditorMode {
    Idle,              // Default, nothing happening
    PlacingComponent,
    PlacingInputPort,
    PlacingOutputPort,
    DrawingConnection
};

struct EditorState  // Holds the editing state that the controller changes - MAKE DEFAULT CONSTRUCTIBLE FOR NOW
{
    // When to update
    bool stepSim = false;
    bool runSim = false;

    // Keep track of what the user is doing
    EditorMode mode = EditorMode::Idle;
    
    // Toggling inputs
    bool rightMouseDown = false;
    int lastToggledInput = -1; // -1 = none, stops debouncing type thing when toggling inputs when sliding

    // Connection / wire stuff
    float wireThickness = 8.0f;

    // GUI variables
    sf::Vector2f mousePos = sf::Vector2f(0.f, 0.f);
    sf::Vector2f snappedMousePos = sf::Vector2f(0.f, 0.f);
    sf::Vector2f snappedMousePosRounded = sf::Vector2f(0.f, 0.f);


    // Render variables
    float gridSize = 25.0f;
    float padding = 0.3f;       // The fraction of the size of a grid cell that a component extends
    float bottomOffset = 60.0f;

    // Maybe add variables for button sizes and relate to bottomOffset
};