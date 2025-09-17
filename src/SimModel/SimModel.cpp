#include "SimModel.h"

#include "Helpers/FileHelpers.h"

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

void SimModel::addComponent(ComponentType type, std::string name)  // Name only required for subcircuits
{
    auto& def = mainInst.def;
    auto& state = mainInst.state;

    ComponentVisual compVisual;
    ComponentInfo compInfo{ type, 0, 0 }; // placeholder, set properly below
    
    switch (type)
    {
        case ComponentType::AND:{
            compVisual.label = "AND";
            compInfo = ComponentInfo{ type, 2, 1 };
            compInfo.name = "AND";
            // Also add nullopt to the subcircuits vector to maintain parallel indexes
            state.subcircuitInstances.push_back(std::nullopt);
            break;}

        case ComponentType::OR:{
            compVisual.label = "OR";
            compInfo = ComponentInfo{ type, 2, 1 };
            compInfo.name = "OR";
            state.subcircuitInstances.push_back(std::nullopt);
            break;}

        case ComponentType::NOT:{
            compVisual.label = "NOT";
            compInfo = ComponentInfo{ type, 1, 1 };
            compInfo.name = "NOT";
            state.subcircuitInstances.push_back(std::nullopt);
            break;}   
            
        case ComponentType::SUBCIRCUIT:{ 
            
            compVisual.label = name;
            compInfo.name = name;
            compInfo.type = type;

            // Load the definition from JSON
            std::string subPath = "../circuits/" + name + ".json";
            auto subDef = Helpers::loadNetlistFromJson(subPath);

            if (subDef) {
                std::cerr << "Failed to load subcircuit definition: " << name << "\n";
            } else {
                // Copy the port counts from the subcircuit definition
                compInfo.numInputs  = compInfo.subcircuitDef->numInputs;
                compInfo.numOutputs = compInfo.subcircuitDef->numOutputs;
            }
            break;
        }

        default:{
            std::cerr << "Trying to add a non existent component type\n";
            return;}
    }

    compVisual.position = {-100.f, -100.f};     // So it is off screen and not initially visible at origin
    compVisual.isGhost = true;

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