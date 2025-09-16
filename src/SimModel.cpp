#include "SimModel.h"

#include "Helpers.h"
#include <iostream>
#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;


SimModel::SimModel()
{
    mainInst.def = std::make_shared<Netlist>();
}

void SimModel::addConnection(ConnectionInfo info, ConnectionVisual vis)
{
    mainInst.def->connections.push_back(info);
    mainInst.state.connectionVisuals.push_back(vis);
    
    // Sanity check
    if (mainInst.def->connections.size() != mainInst.state.connectionVisuals.size())
        std::cerr<<"Connection logic and visual vectors are NOT same size!";
}


void SimModel::stepNet(NetlistInstance &netInst)
{
    auto& state = netInst.state;
    const auto& def = netInst.def;
    int outputsBaseIndex = getOutputsBaseIndex(*def, state);

    // Copy in just in case currentVals has been externally updated - means the logic runs faster sometimes
    state.nextValues = state.currentValues;

    // Loop over components and update
    for (int ci = 0; ci < def->components.size() ; ci++)
    {
        const auto& c = def->components[ci];

        std::vector<bool> inputValues;
        std::vector<bool> outputValues(c.numOutputs, false);    // Size the vector

        // Find out indices of connecting components
        for (const auto& conn : def->connections) 
        {
            if (conn.toComp == ci)  
            {
                int nodeIndex;
                if (conn.fromComp == -1)
                    nodeIndex = conn.outPin; // external input
                else
                {
                    int baseIndex = getComponentBaseIndex(*def, state, conn.fromComp);
                    nodeIndex = baseIndex + conn.outPin;         // Global pin index
                }

                inputValues.push_back(state.currentValues[nodeIndex]);
            }
        }
        
        switch (c.type)
        {
            case ComponentType::AND:
            {
                if (inputValues.size() != 2) std::cerr<<"AND doesn't have 2 inputs\n";

                outputValues[0] = inputValues[0] && inputValues[1];
                break;
            }
            case ComponentType::OR:
            {
                if (inputValues.size() != 2) std::cerr<<"OR doesn't have 2 inputs\n";

                outputValues[0] = inputValues[0] || inputValues[1];
                break;
            }
            case ComponentType::NOT:
            {
                if (inputValues.size() != 1) std::cerr<<"NOT doesn't have 1 inputs\n";

                outputValues[0] = !inputValues[0];
                break;
            }
            case ComponentType::SUBCIRCUIT:
            {
                // IMPLEMENT FUTURE LOGIC HERE
                break;
            }
        }

        //  Find component base index and write the outputs to the next vector
        int compBaseIndex = getComponentBaseIndex(*def, state, ci);
        for (int oi = 0; oi < outputValues.size() ; oi++)
        {
            bool val = outputValues[oi];
            state.nextValues[compBaseIndex + oi] = val; 
        }

        //  Update the external outputs too
        for (const auto& conn : def->connections)
        {
            if (conn.fromComp == ci)
            {
                if (conn.toComp == -1)
                    state.nextValues[outputsBaseIndex + conn.inPin] = state.nextValues[compBaseIndex + conn.outPin]; 

                else continue;  // Otherwise output exclusively corresponds to component position, don't care
            }
        } 
    }

    state.currentValues = state.nextValues;
}


void SimModel::addComponent(ComponentType type)
{
    ComponentVisual compVisual;
    ComponentInfo compInfo{ type, 0, 0 }; // placeholder, set properly below
    
    switch (type)
    {
        case ComponentType::AND:
            compVisual.label = "AND";
            compInfo = ComponentInfo{ type, 2, 1 };
            compInfo.name = "AND";
            break;

        case ComponentType::OR:
            compVisual.label = "OR";
            compInfo = ComponentInfo{ type, 2, 1 };
            compInfo.name = "OR";
            break;

        case ComponentType::NOT:
            compVisual.label = "NOT";
            compInfo = ComponentInfo{ type, 1, 1 };
            compInfo.name = "NOT";
            break;   
            
        case ComponentType::SUBCIRCUIT:
            // ADD LOGIC HERE - MAYBE IN ANOTHER FUNCTION
            break;

        default:
            std::cerr << "Trying to add a non existent component type\n";
            return;
    }

    compVisual.position = {-100.f, -100.f};     // So it is off screen and not initially visible at origin
    compVisual.isGhost = true;

    auto& def = mainInst.def;
    auto& state = mainInst.state;

    def->components.push_back(compInfo);
    state.componentVisuals.push_back(compVisual);
    state.currentValues.resize(state.currentValues.size() + compInfo.numOutputs);
    state.nextValues.resize(state.nextValues.size() + compInfo.numOutputs);
}

void SimModel::addInputPort(InputPort inputPort)
{
    inputPorts.push_back(inputPort);

    // Also resize the state vectors as there is an external inputs section

    auto& state = mainInst.state;
    state.currentValues.resize(state.currentValues.size() + 1);
    state.nextValues.resize(state.nextValues.size() + 1);
    mainInst.def->numInputs++;
}

void SimModel::addOutputPort(OutputPort outputPort)
{
    outputPorts.push_back(outputPort);

    // Also resize the state vectors as there is an external inputs section

    auto& state = mainInst.state;
    state.currentValues.resize(state.currentValues.size() + 1);
    state.nextValues.resize(state.nextValues.size() + 1);
    mainInst.def->numOutputs++;
}


void SimModel::finishCircuit(std::string name)
{
    nlohmann::json j;
    j["name"] = name;
    j["numInputs"] = inputPorts.size();
    j["numOutputs"] = outputPorts.size();

    // Serialize components
    for (const auto& c : mainInst.def->components) {
        nlohmann::json cj;
        cj["type"] = c.name;
        cj["numInputs"] = c.numInputs;
        cj["numOutputs"] = c.numOutputs;
        j["components"].push_back(cj);
    }

    // Serialize connections
    for (const auto& conn : mainInst.def->connections) {
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
}




