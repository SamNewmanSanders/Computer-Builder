#include "SimModel.h"
#include "Helpers/GeneralHelpers.h"

#include <fstream>
#include <filesystem>
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;


void SimModel::finishCircuit(std::string name)
{
    nlohmann::json j;
    j["name"] = name;
    j["numInputs"] = inputPorts.size();
    j["numOutputs"] = outputPorts.size();

    // Serialize components
    for (const auto& c : def.components) {
        nlohmann::json cj;
        cj["type"] = Helpers::componentTypeToString(c.type);
        cj["name"] = c.name;
        cj["numInputs"] = c.numInputs;
        cj["numOutputs"] = c.numOutputs;
        j["components"].push_back(cj);
    }

    // Serialize connections
    for (const auto& conn : def.connections) {
        nlohmann::json cj;
        cj["fromComp"] = conn.fromComp;
        cj["outPin"]   = conn.outPin;
        cj["toComp"]   = conn.toComp;
        cj["inPin"]    = conn.inPin;
        j["connections"].push_back(cj);
    }

    std::filesystem::path circuitsDir = "../circuits";
    std::filesystem::create_directories(circuitsDir);

    std::filesystem::path filename = circuitsDir / (name + ".json");
    std::ofstream file(filename);
    file << j.dump(4);


    reset();    // Reset the model
}
