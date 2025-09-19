#pragma once

#include <cstdint>
#include <string>
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

    std::string name = "";   // This is neccesary for subcircuits to identify them, also set for gates why not

    std::shared_ptr<Netlist> subDef = nullptr;   // FOR NOW EACH COMP STILL POINTS TO ITS OWN UNIQUE DEFINITION
};

struct ComponentVisual {
    sf::Vector2f position;
    sf::Vector2f size;
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
    std::vector<sf::Vector2f> wirePoints;   // The wire may weave to avoid components so have a vector of "corner points" per say
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

    // Have a vector that also represents the state of any subcomponents. If not a subcircuit just nullopt.
    std::vector<std::optional<NetlistState>> subcircuitStates;
};




// Also define structs that represents external i/o's controlled by user - KEEP THIS SIMPLE
struct InputPort{
    sf::Vector2f position;
    sf::Vector2f size;
    bool isBeingPlaced = false;
};

struct OutputPort{
    sf::Vector2f position;
    sf::Vector2f size;
    bool isBeingPlaced = false;
};