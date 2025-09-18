#include "Helpers/IndexHelpers.h"

namespace Helpers
{

// Helper that can find where the [netlist outputs] section of the vector starts
int getOutputsBaseIndex(const Netlist& def, const NetlistState& state) 
{
    int i = static_cast<int>(state.currentValues.size()) - def.numOutputs;

    if (i < 0) {std::cerr<<"def.NumOuputs is larger than state.currentValues\n"; return -1;}
    
    return i;
}

// Similar thing for a component base index
int getComponentBaseIndex(const Netlist& def, const NetlistState& state, int compIndex)
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

// This function returns the value of a component output pin
bool getOutputPinValue(const Netlist def, const NetlistState state, int compIndex, int pinIndex)
{

    int index;

    if(compIndex == -1){     // For the case where it is an input port
        index = pinIndex;   
    }
    else index = getComponentBaseIndex(def, state, compIndex) + pinIndex;

    bool value = state.currentValues[index];

    return value;
    // This function also should work if compIndex is -1 as getComponentBaseIndex works for the -1 case too
}

// This function checks if a component inputpin is already connected - stops multiple outputs connecting to one input
bool isInputPinConnected(const Netlist def, const NetlistState state, int compIndex, int pinIndex)
{
    bool connected = false;
    // Loop over connections and check
    for (auto& conn : def.connections)
    {
        if (conn.toComp == compIndex && conn.inPin == pinIndex) connected = true;
    }
    return connected;
}


}