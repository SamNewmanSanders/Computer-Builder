#include "SimModel.h"

#include "Helpers/FileHelpers.h"

SimModel::SimModel()
{
    def = Netlist();
}

void SimModel::addConnection(ConnectionInfo info, ConnectionVisual vis)
{
    def.connections.push_back(info);
    state.connectionVisuals.push_back(vis);
    
    // Sanity check
    if (def.connections.size() != state.connectionVisuals.size())
        std::cerr<<"Connection logic and visual vectors are NOT same size!";
}

// Have def and state as arguements here so it can be called recursively
void SimModel::addComponent(Netlist& def, NetlistState& state, ComponentType type, std::string name)  // Name only required for subcircuits
{

    ComponentVisual compVisual;
    ComponentInfo compInfo{ type, 0, 0 }; // placeholder, set properly below
    
    switch (type)
    {
        case ComponentType::AND:{
            compVisual.label = "AND";
            compInfo = ComponentInfo{ type, 2, 1 };
            compInfo.name = "AND";
            // Also add nullopt to the subcircuits vector to maintain parallel indexes
            state.subcircuitStates.push_back(std::nullopt);
            break;}

        case ComponentType::OR:{
            compVisual.label = "OR";
            compInfo = ComponentInfo{ type, 2, 1 };
            compInfo.name = "OR";
            state.subcircuitStates.push_back(std::nullopt);
            break;}

        case ComponentType::NOT:{
            compVisual.label = "NOT";
            compInfo = ComponentInfo{ type, 1, 1 };
            compInfo.name = "NOT";
            state.subcircuitStates.push_back(std::nullopt);
            break;}   
            
        case ComponentType::SUBCIRCUIT:{ 

            compVisual.label = name;
            compInfo.name = name;
            compInfo.type = type;

            std::string subPath = "../circuits/" + name + ".json";
            auto [subDef, subState] = Helpers::loadNetlistFromJson(subPath);
            compInfo.subDef = std::make_shared<Netlist>(subDef);

            compInfo.numInputs  = compInfo.subDef->numInputs;
            compInfo.numOutputs = compInfo.subDef->numOutputs;

            state.subcircuitStates.push_back(subState);
            
            break;
        }

        default:{
            std::cerr << "Trying to add a non existent component type\n";
            return;}
    }

    compVisual.position = {-100.f, -100.f};     // So it is off screen and not initially visible at origin
    compVisual.isGhost = true;

    def.components.push_back(compInfo);
    state.componentVisuals.push_back(compVisual);
    state.currentValues.resize(state.currentValues.size() + compInfo.numOutputs);
    state.nextValues.resize(state.nextValues.size() + compInfo.numOutputs);
}



void SimModel::addInputPort(InputPort inputPort)
{
    inputPorts.push_back(inputPort);

    // Also resize the state vectors as there is an external inputs section

    state.currentValues.resize(state.currentValues.size() + 1);
    state.nextValues.resize(state.nextValues.size() + 1);
    def.numInputs++;
}

void SimModel::addOutputPort(OutputPort outputPort)
{
    outputPorts.push_back(outputPort);

    // Also resize the state vectors as there is an external inputs section

    state.currentValues.resize(state.currentValues.size() + 1);
    state.nextValues.resize(state.nextValues.size() + 1);
    def.numOutputs++;
}

// Cancel the placement of a component (assuming there is one being placed)
void SimModel::cancelPlacement(EditorMode& editorMode)
{
    if (editorMode == EditorMode::PlacingComponent && state.componentVisuals.back().isGhost)
    {
        state.currentValues.resize(state.currentValues.size() - def.components.back().numOutputs);
        state.nextValues.resize(state.nextValues.size() - def.components.back().numOutputs);
        state.subcircuitStates.pop_back();
        def.components.pop_back();  // Remove last element
        state.componentVisuals.pop_back();
    }

    else if (editorMode == EditorMode::PlacingInputPort && inputPorts.back().isBeingPlaced)
    {
        state.currentValues.pop_back();
        state.nextValues.pop_back();
        def.numInputs--;
        inputPorts.pop_back();
    }
    else if (editorMode == EditorMode::PlacingOutputPort && outputPorts.back().isBeingPlaced)
    {
        state.currentValues.pop_back();
        state.nextValues.pop_back();
        def.numOutputs--;
        outputPorts.pop_back();
    }

    editorMode = EditorMode::Idle;
}