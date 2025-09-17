#include "Helpers/GeneralHelpers.h"

namespace Helpers
{

ComponentType getComponentTypeFromString(const std::string& s) {
    if (s == "AND") return ComponentType::AND;
    if (s == "OR") return ComponentType::OR;
    if (s == "NOT") return ComponentType::NOT;
    if (s == "SUBCIRCUIT") return ComponentType::SUBCIRCUIT;
    else
    {
        std::cerr<<"String NOT recognized when converting to ComponentType. Returning AND defualt\n";
        return ComponentType::AND;
    }
}

std::string componentTypeToString(ComponentType type)
{
    switch(type) {
        case ComponentType::AND: return "AND";
        case ComponentType::OR: return "OR";
        case ComponentType::NOT: return "NOT";
        case ComponentType::SUBCIRCUIT: return "SUBCIRCUIT";
        default: return "UNKNOWN";
    }
}

}