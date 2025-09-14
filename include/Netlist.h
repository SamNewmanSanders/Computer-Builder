#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <optional>
#include <iostream>
#include <SFML/Graphics.hpp>

// Forward declare
struct Netlist;

enum class ComponentType { AND, OR, NOT, SUBCIRCUIT};

struct ComponentInfo
{
    ComponentType type;
    int numInputs;
    int numOutputs;

    // Store the index of the component within the net for quick access. Null if not placed yet.
    // This is important as the global pin index can simply be the CompBaseIndex + local pin index
    std::optional<int> compBaseIndex = std::nullopt;                   
};

struct ComponentVisual {
    sf::Vector2f position;
    sf::Vector2f size;
    std::string label;
    bool isGhost;       
};

struct ConnectionInfo {

    int fromComp;   // component index (-1 if external *input)
    int outPin;     // Pin index within the component (still applies if external!)
    int toComp;     // (-1 if external *output)
    int inPin;
};


struct Netlist {
    std::vector<ComponentInfo> components;
    std::vector<ConnectionInfo> connections;
    uint32_t numInputs;
    uint32_t numOutputs;
};

struct NetlistState {
    //  Vectors are structured [netlist inputs][Component outputs][netlist outputs]

    std::vector<bool> currentValues; // Stores all boolean information in the net
    std::vector<bool> nextValues;    // Use a double buffer for two step simulation

    // Also include the visual aspects of a component, tied to logic by vector index
    std::vector<ComponentVisual> componentVisuals;  
};


struct NetlistInstance {
    Netlist* def;   // The netlist itself should be shared
    NetlistState state;   // The state should be *unique* to the instance
};



// Also have this useful helper that can find where the [netlist outputs] section of the vector starts
inline int getOutputsBaseIndex(const Netlist& def, const NetlistState& state) 
{
    int i = static_cast<int>(state.currentValues.size()) - def.numOutputs;

    if (i < 0) std::cerr<<"def.NumOuputs is larger than state.currentValues\n"; return -1;
    
    return i;
}