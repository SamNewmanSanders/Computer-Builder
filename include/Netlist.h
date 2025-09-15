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
    // Note the order of declaration here is important for how I construct later
};

// Parallel struct to connection which contains information on how to draw it. Related by index
struct ConnectionVisual
{
    bool isBeingDrawn = false;
    sf::Vector2f tempEndPos;    // When the end of the wire doesn't have an index yet
    // Could have colour, etc etc
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
    std::vector<ConnectionVisual> connectionVisuals;    // Similar 
};


struct NetlistInstance {
    std::shared_ptr<Netlist> def;   // The netlist itself should be shared
    NetlistState state;   // The state should be *unique* to the instance
};


// Also define a struct that 