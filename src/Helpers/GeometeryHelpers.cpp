#include "Helpers/GeometeryHelpers.h"

namespace Helpers
{

sf::Vector2f snapToGrid(sf::Vector2f pos, float gridSize)
{
    float snappedPosX = static_cast<int>(pos.x / gridSize) * gridSize;
    float snappedPosY = static_cast<int>(pos.y  / gridSize) * gridSize;

    return sf::Vector2f(snappedPosX, snappedPosY);
}

sf::Vector2f snapToGridRounded(sf::Vector2f pos, float gridSize)
{
    float snappedPosX = static_cast<int>(pos.x / gridSize + 0.5f) * gridSize;
    float snappedPosY = static_cast<int>(pos.y / gridSize + 0.5f) * gridSize;

    return sf::Vector2f(snappedPosX, snappedPosY);
}

bool isMouseOverPoint(const sf::Vector2f& mousePos, const sf::Vector2f& thingPos, float halfSize) {
    return mousePos.x >= thingPos.x - halfSize && mousePos.x <= thingPos.x + halfSize &&
           mousePos.y >= thingPos.y - halfSize && mousePos.y <= thingPos.y + halfSize;
}

bool isMouseOverBox(const sf::Vector2f& mousePos, const sf::Vector2f& pos, const sf::Vector2f& size)
{
    return mousePos.x >= pos.x &&
           mousePos.x <= pos.x + size.x &&
           mousePos.y >= pos.y &&
           mousePos.y <= pos.y + size.y;
}

sf::Vector2f positionToTopLeft(float gridSize, float padding, sf::Vector2f compPos)   
{
    return compPos + sf::Vector2f(0.0f, -gridSize * padding);
}


// Returns a vector of a given components pin postitions - so they can be compared to a mouse position
std::vector<sf::Vector2f> getPinPositions(const ComponentInfo& info, const ComponentVisual& visual,
                                                 bool isInputPins, float gridSize)
{
    std::vector<sf::Vector2f> pinVector;

    if (isInputPins)
    {
        for (int pi = 0 ; pi < info.numInputs ; pi++)
        {
            sf::Vector2f pinPos;
            float relY = pi * gridSize;
            float relX = 0.0f;
            pinPos = visual.position + sf::Vector2f(relX, relY);
            pinVector.push_back(pinPos);
        } 
    }

    else
    {
        for (int po = 0 ; po < info.numOutputs ; po++)
        {
            sf::Vector2f pinPos;
            float relY = po * gridSize;
            float relX = visual.size.x;
            pinPos = visual.position + sf::Vector2f(relX, relY);
            pinVector.push_back(pinPos);
        }
    }

    return pinVector;
}

// Similar to above - returns a vector of positions of all input ports
std::vector<sf::Vector2f> getInputPortPinPositions(const std::vector<InputPort>& inputs)
{
    std::vector<sf::Vector2f> pinVector;

    for (int ii = 0 ; ii < inputs.size() ; ii++)
    {
        sf::Vector2f pinPos;
        float relY = 0.0f;
        float relX = inputs[ii].size.x;
        pinPos = inputs[ii].position + sf::Vector2f(relX, relY);
        pinVector.push_back(pinPos);
    } 
    return pinVector;
}


// And for outputs
std::vector<sf::Vector2f> getOutputPortPinPositions(const std::vector<OutputPort>& outputs)
{
    std::vector<sf::Vector2f> pinVector;

    for (int oi = 0 ; oi < outputs.size() ; oi++)
    {
        sf::Vector2f pinPos;
        float relY = 0.0f;      // Both zero here as the output port pin is an input pin, which is at origin
        float relX = 0.0f;  
        pinPos = outputs[oi].position + sf::Vector2f(relX, relY);
        pinVector.push_back(pinPos);
    } 
    return pinVector;
}

}