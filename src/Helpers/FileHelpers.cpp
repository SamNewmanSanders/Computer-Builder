#include "Helpers/FileHelpers.h"

using json = nlohmann::ordered_json;

namespace Helpers
{

    
bool jsonFileExists(const std::string& path)
{
    std::filesystem::path p(path);
    return std::filesystem::exists(p) && p.extension() == ".json";
}


std::pair<Netlist, NetlistState> loadNetlistFromJson(const std::string& filename) 
{

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open netlist file." << filename << "\n";
    }

    json j;
    file >> j;

    auto def = Netlist();
    auto state = NetlistState();
    def.numInputs  = j.value("numInputs", 0);
    def.numOutputs = j.value("numOutputs", 0);

    int stateVectorsSize = def.numInputs + def.numOutputs;  // Track to resize the state vector

    for (auto& jc : j["components"]) {
        ComponentInfo c;
        c.type       = Helpers::getComponentTypeFromString(jc["type"]);
        c.numInputs  = jc.value("numInputs", 0);
        c.numOutputs = jc.value("numOutputs", 0);
        c.name       = jc.value("name", "");

        stateVectorsSize += c.numOutputs;   // Add the correct number of slots for each component

        // If the component itself is a subcircuit, recursively call
        if (c.type == ComponentType::SUBCIRCUIT) {
            std::string subPath = "../circuits/" + c.name + ".json";

            auto [subDef, subState] = Helpers::loadNetlistFromJson(subPath);

            c.subDef = std::make_shared<Netlist>(subDef);
            state.subcircuitStates.push_back(subState);
        }
        else state.subcircuitStates.push_back(std::nullopt);    // Make sure to keep the substate vector indexes correct

        def.components.push_back(c);
    }

    for (auto& jconn : j["connections"]) {
        def.connections.push_back({
            jconn.value("fromComp", 0),
            jconn.value("outPin", 0),
            jconn.value("toComp", 0),
            jconn.value("inPin", 0)
        });
    }

    state.currentValues.resize(stateVectorsSize, LogicState::OFF);
    state.nextValues.resize(stateVectorsSize, LogicState::OFF);

    return { std::move(def), std::move(state) };
}

}