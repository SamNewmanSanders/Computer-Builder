#pragma once

#include <SFML/Graphics.hpp>

#include "Netlist.h"

inline sf::Vector2f snapToGrid(sf::Vector2f pos, float gridSize)
{
    float snappedPosX = static_cast<int>(pos.x / gridSize) * gridSize;
    float snappedPosY = static_cast<int>(pos.y  / gridSize) * gridSize;

    return sf::Vector2f(snappedPosX, snappedPosY);
}


inline std::vector<sf::Vector2f> getPinPositions(ComponentInfo& info, ComponentVisual& visual, bool isInputPins, float gridSize)
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

inline bool isMouseOverPoint(const sf::Vector2f& mousePos, const sf::Vector2f& thingPos, float halfSize) {
    return mousePos.x >= thingPos.x - halfSize && mousePos.x <= thingPos.x + halfSize &&
           mousePos.y >= thingPos.y - halfSize && mousePos.y <= thingPos.y + halfSize;
}

// Helper that can find where the [netlist outputs] section of the vector starts
inline int getOutputsBaseIndex(const Netlist& def, const NetlistState& state) 
{
    int i = static_cast<int>(state.currentValues.size()) - def.numOutputs;

    if (i < 0) std::cerr<<"def.NumOuputs is larger than state.currentValues\n"; return -1;
    
    return i;
}

// Similar thing for a component base index
inline int getComponentBaseIndex(const Netlist& def, const NetlistState& state, int compIndex)
{
    if (compIndex != -1)    // Case where it is an actual component
    {
        int base = def.numInputs;
        for (int i = 0; i < compIndex; i++)
            base += def.components[i].numOutputs;
        return base;
    }
    else
    {
        return 0;       
    }
}


inline bool getOutputPinValue(NetlistInstance inst, int compIndex, int pinIndex)
{
    auto& def = inst.def;
    auto& state = inst.state;

    int index = getComponentBaseIndex(*def, state, compIndex) + pinIndex;

    bool value = state.currentValues[index];

    return value;
    // This function also should work if compIndex is -1 as getComponentBaseIndex works for the -1 case too
}

inline bool isInputPinConnected(NetlistInstance& inst, int compIndex, int pinIndex)
{
    bool connected = false;
    // Loop over connections and check
    for (auto& conn : inst.def->connections)
    {
        if (conn.toComp == compIndex && conn.inPin == pinIndex) connected = true;
    }
    return connected;
}
