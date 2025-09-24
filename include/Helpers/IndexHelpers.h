#pragma once

// This file is useful because the logic is reliant on indices. There are component indices, and indices in the state vector
// which correspond to true/false values for pin indices within components. These helpers convert, read values etc

#include "Netlist.h"

namespace Helpers
{

// Helper that can find where the [netlist outputs] section of the vector starts
int getOutputsBaseIndex(const Netlist& def, const NetlistState& state);

// Similar thing for a component base index
int getComponentBaseIndex(const Netlist& def, const NetlistState& state, int compIndex);

// This function returns the value of a component output pin
LogicState getOutputPinValue(const Netlist def, const NetlistState state, int compIndex, int pinIndex);

// This function checks if a component inputpin is already connected - stops multiple outputs connecting to one input
bool isInputPinConnected(const Netlist def, const NetlistState state, int compIndex, int pinIndex);

}