#pragma once

// This file is just for simple general useful functions that don't fit into the other files

#include "Netlist.h"
#include <iostream>

namespace Helpers
{

ComponentType getComponentTypeFromString(const std::string& s);

std::string componentTypeToString(ComponentType type);

}