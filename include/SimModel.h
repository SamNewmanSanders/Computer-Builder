#pragma once

#include "Netlist.h"
#include "EditorState.h"

// Forward declare
struct EditorState;

class SimModel  // This is the "truth" - everything else just reads / writes to this
{
public:

    SimModel();

    NetlistInstance mainInst;

    void stepNet(NetlistInstance& netInst);

    void addComponent(ComponentType type, std::string name = "");
    void addConnection(ConnectionInfo info, ConnectionVisual vis);

    // Define the user ports here as I want a clean seperation between user inputs and the netlist itself (black box)
    std::vector<InputPort> inputPorts;
    std::vector<OutputPort> outputPorts;

    void addInputPort(InputPort inputPort);
    void addOutputPort(OutputPort outputPort);

    void finishCircuit(std::string name);
};


