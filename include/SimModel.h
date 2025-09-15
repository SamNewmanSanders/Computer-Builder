#pragma once

#include "Netlist.h"
#include "EditorState.h"

// Forward declare
struct EditorState;

class SimModel  // This is the "truth" - everything else just reads / writes to this
{
public:

    SimModel();

    // These members would usually be within an instance but cleaner if seperated here
    NetlistInstance mainInst;

    void stepNet(NetlistInstance& netInst);

    void addComponent(ComponentType type);
    void addConnection(ConnectionInfo info, ConnectionVisual vis);
    
    
    // Some api like functions i guess
    //void setInput();
    //void readOutput();

private:
};


