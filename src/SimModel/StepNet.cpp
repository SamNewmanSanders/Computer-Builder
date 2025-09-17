#include "SimModel.h"

#include "Helpers/IndexHelpers.h"

void SimModel::stepNet(NetlistInstance &netInst)
{
    auto& state = netInst.state;
    const auto& def = netInst.def;
    int outputsBaseIndex = Helpers::getOutputsBaseIndex(*def, state);

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
                    int baseIndex = Helpers::getComponentBaseIndex(*def, state, conn.fromComp);
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
                if(!state.subcircuitInstances[ci]) std::cerr<<"SubCircuit does not have a corresponding state! Index mismatch?\n";
                else
                {
                    for (int i = 0 ; i < inputValues.size() ; i++)
                    {
                        state.subcircuitInstances[ci]->state.currentValues[i] = inputValues[i];
                        state.subcircuitInstances[ci]->state.nextValues[i] = inputValues[i];
                    }
                }
                break;
            }
        }

        //  Find component base index and write the outputs to the next vector
        int compBaseIndex = Helpers::getComponentBaseIndex(*def, state, ci);
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
