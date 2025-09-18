#include "Helpers/FileHelpers.h"

using json = nlohmann::ordered_json;

namespace Helpers
{

    
bool jsonFileExists(const std::string& path)
{
    std::filesystem::path p(path);
    return std::filesystem::exists(p) && p.extension() == ".json";
}


Netlist loadNetlistFromJson(const std::string& filename) {

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open netlist file." << filename << "\n";
    }

    json j;
    file >> j;


    auto def = Netlist();
    def.numInputs  = j.value("numInputs", 0);
    def.numOutputs = j.value("numOutputs", 0);

    for (auto& jc : j["components"]) {
        ComponentInfo ci;
        ci.type       = Helpers::getComponentTypeFromString(jc["type"]);
        ci.numInputs  = jc.value("numInputs", 0);
        ci.numOutputs = jc.value("numOutputs", 0);
        ci.name       = jc.value("name", "");

        if (ci.type == ComponentType::SUBCIRCUIT) {
            std::string subPath = "../circuits/" + ci.name + ".json";
            

            // ci.subcircuitDef = loadNetlistFromJson(subPath);
        }

        def.components.push_back(ci);
    }

    for (auto& jconn : j["connections"]) {
        def.connections.push_back({
            jconn.value("fromComp", 0),
            jconn.value("outPin", 0),
            jconn.value("toComp", 0),
            jconn.value("inPin", 0)
        });
    }

    return def;
}

}