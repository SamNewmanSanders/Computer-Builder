#pragma once

#include <SFML/Graphics.hpp>
#include "Netlist.h"

namespace Helpers
{

sf::Vector2f snapToGrid(sf::Vector2f pos, float gridSize);

bool isMouseOverPoint(const sf::Vector2f& mousePos, const sf::Vector2f& thingPos, float halfSize);

bool isMouseOverBox(const sf::Vector2f& mousePos, const sf::Vector2f& pos, const sf::Vector2f& size);

// Returns a vector of a given components pin postitions - so they can be compared to a mouse position
std::vector<sf::Vector2f> getPinPositions(const ComponentInfo& info, const ComponentVisual& visual,
                                                 bool isInputPins, float gridSize);

// Similar to above - returns a vector of positions of all input ports
std::vector<sf::Vector2f> getInputPortPinPositions(const std::vector<InputPort>& inputs);

// And for outputs
std::vector<sf::Vector2f> getOutputPortPinPositions(const std::vector<OutputPort>& outputs);

}