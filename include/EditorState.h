#pragma once

#include <SFML/Graphics.hpp>

#include <optional>

#include "Netlist.h"

struct EditorState  // Holds the editing state that the controller changes - MAKE DEFAULT CONSTRUCTIBLE FOR NOW
{
    // When to update
    bool stepSim = false;
    bool runSim = false;

    // Placing things
    bool placingComponent = false;
    bool placingInputPort = false;       // Similar but for I/O ports
    bool placingOutputPort = false;
    
    // Toggling inputs
    bool rightMouseDown = false;
    int lastToggledInput = -1; // -1 = none, stops debouncing type thing when toggling inputs when sliding

    // Relevant objects
    std::optional<ConnectionInfo> currentConnectionInfo = std::nullopt;         // Logic info
    std::optional<ConnectionVisual> currentConnectionVisual = std::nullopt;     // Visual info

    // GUI variables
    sf::Vector2f mousePos = sf::Vector2f(0.f, 0.f);
    sf::Vector2f snappedMousePos = sf::Vector2f(0.f, 0.f);


    // Render variables
    float gridSize = 30.0f;
    float padding = 0.3f;       // The fraction of the size of a grid cell that a component extends
    float bottomOffset = 60.0f;
    float wireThickness = 8.0f;

    // Maybe add variables for button sizes and relate to bottomOffset
};