#pragma once

#include "Netlist.h"

// Forward declare
struct EditorState;

class SimModel  // This is the "truth" - everything else just reads / writes to this
{
public:

    // These members would usually be within an instance but cleaner if seperated here
    NetlistInstance mainInst;

    void stepNet(NetlistInstance& netInst);

    // Some api like functions i guess
    //void setInput();
    //void readOutput();

private:
};


struct EditorState  // Holds the editing state that the controller changes - MAKE DEFAULT CONSTRUCTIBLE FOR NOW
{
    // Sim state variables
    bool updateSim = false;

    // std::optional<Component> ghostComponent;
    // std::optional<Wire> ghostWire;
    // std::optional<std::pair<int,int>> highlightedInput;
    // std::optional<std::pair<int,int>> highlightedOutput;
};