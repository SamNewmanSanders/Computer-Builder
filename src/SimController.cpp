#include "SimController.h"

#include "Helpers.h"

SimController::SimController(SimModel& model_, EditorState& editorState_, sf::RenderWindow& window_) 
: model(model_), editorState(editorState_), window(window_) {}

void SimController::handleInputs(tgui::Gui& gui)
{
    while (const std::optional event = window.pollEvent())
    {
        gui.handleEvent(*event);

        // Close window
        if (event->is<sf::Event::Closed>())
        {
            window.close();
            continue;
        }

        if (auto* kp = event->getIf<sf::Event::KeyPressed>())
            handleKeyPress(*kp);
        else if (auto* mm = event->getIf<sf::Event::MouseMoved>())
            handleMouseMove(*mm);
        else if (auto* mp = event->getIf<sf::Event::MouseButtonPressed>())
            handleMousePress(*mp);
    }
}

void SimController::handleKeyPress(const sf::Event::KeyPressed& kp)
{
    if (kp.code == sf::Keyboard::Key::S) editorState.stepSim = true;
    if (kp.code == sf::Keyboard::Key::R) editorState.runSim = !editorState.runSim;

    if (kp.code == sf::Keyboard::Key::Escape)
        {
            window.close();
        }
    if (kp.code == sf::Keyboard::Key::Backspace && editorState.placingComponent)
        editorState.placingComponent = false;

    if (kp.code == sf::Keyboard::Key::Backspace && editorState.placingInputPort)
        editorState.placingInputPort = false;
    if (kp.code == sf::Keyboard::Key::Backspace && editorState.placingOutputPort)
        editorState.placingOutputPort = false;
    if (kp.code == sf::Keyboard::Key::Backspace && editorState.currentConnectionInfo)
        {editorState.currentConnectionInfo = std::nullopt; editorState.currentConnectionVisual = std::nullopt;}
}

void SimController::handleMouseMove(const sf::Event::MouseMoved& mm)
{
    auto& def = model.mainInst.def;
    auto& state = model.mainInst.state;

    editorState.mousePos = static_cast<sf::Vector2f>(mm.position);
    editorState.snappedMousePos = snapToGrid(editorState.mousePos, editorState.gridSize);

    // Snap a component you are moving to the grid
    if (editorState.placingComponent)
    {
        if (state.componentVisuals.back().isGhost)
            state.componentVisuals.back().position = editorState.snappedMousePos;
        else std::cerr<<"Component being placed is not a ghost\n";          // Should never happen but check anyway
    }

    if (editorState.placingInputPort)
    {
        if (model.inputPorts.back().isBeingPlaced)
            model.inputPorts.back().position = editorState.snappedMousePos;
        else std::cerr<<"Input port being placed does NOT think its being placed????\n";
    }
    if (editorState.placingOutputPort)
    {
        if (model.outputPorts.back().isBeingPlaced)
            model.outputPorts.back().position = editorState.snappedMousePos;
        else std::cerr<<"Output port being placed does NOT think its being placed????\n";
    }

    if (editorState.currentConnectionInfo) 
    {
        editorState.currentConnectionVisual->tempEndPos = editorState.mousePos;
    }
}




void SimController::handleMousePress(const sf::Event::MouseButtonPressed& mp)
{
    auto& def = model.mainInst.def;
    auto& state = model.mainInst.state;

    sf::Vector2f pressPos = static_cast<sf::Vector2f>(mp.position);

    // Finish placing a component
    if (mp.button == sf::Mouse::Button::Left && editorState.placingComponent) 
    {
        state.componentVisuals.back().isGhost = false;
        editorState.placingComponent = false;
    }

    // Finish placing an inputPort / outputPort
    if(mp.button == sf::Mouse::Button::Left && editorState.placingInputPort)
    {
        model.inputPorts.back().isBeingPlaced = false;
        editorState.placingInputPort = false;
    }
    if(mp.button == sf::Mouse::Button::Left && editorState.placingOutputPort)
    {
        model.outputPorts.back().isBeingPlaced = false;
        editorState.placingOutputPort = false;
    }    

    // If NOT drawing a wire, start drawing on pin mouse is hovering over
    if (mp.button == sf::Mouse::Button::Left && !editorState.currentConnectionInfo)
    {   
        for (int ci = 0 ; ci < def->components.size() ; ci++)
        {
            // Search output pins for wire start
            auto outputPinPositions = getPinPositions(def->components[ci], state.componentVisuals[ci], false, editorState.gridSize);
            for (int pi = 0 ; pi < outputPinPositions.size() ; pi++) 
            {
                if (isMouseOverPoint(pressPos, outputPinPositions[pi], 10.0f))
                {
                    editorState.currentConnectionInfo = ConnectionInfo{ci, pi}; // Can do this as they are the first two members
                    editorState.currentConnectionVisual = ConnectionVisual{true, pressPos};   
                }
            }
        }

        // Loop over input ports aswell 
        auto inputPortPinPositions = getInputPortPinPositions(model.inputPorts);
        for (int pi = 0 ; pi < inputPortPinPositions.size() ; pi++)
        {
            if (isMouseOverPoint(pressPos, inputPortPinPositions[pi], 10.0f))
            {
                editorState.currentConnectionInfo = ConnectionInfo{-1, pi}; // -1 means external input
                editorState.currentConnectionVisual = ConnectionVisual{true, pressPos};
            }
        }
    } 

    else if (mp.button == sf::Mouse::Button::Left && editorState.currentConnectionInfo)
    {   
        for (int ci = 0 ; ci < def->components.size() ; ci++)
        {
            // Search input pins for wire end
            auto inputPinPositions = getPinPositions(def->components[ci], state.componentVisuals[ci], true, editorState.gridSize);
            for (int pi = 0 ; pi < inputPinPositions.size() ; pi++) 
            {
                if (isMouseOverPoint(pressPos, inputPinPositions[pi], 10.0f))
                {
                    // Make sure it isn't already connected
                    if (isInputPinConnected(model.mainInst, ci, pi)) continue;

                    editorState.currentConnectionInfo->toComp = ci;
                    editorState.currentConnectionInfo->inPin = pi;
                    editorState.currentConnectionVisual->isBeingDrawn = false;
                    model.addConnection(*editorState.currentConnectionInfo, *editorState.currentConnectionVisual);
                    editorState.currentConnectionInfo = std::nullopt;
                    editorState.currentConnectionVisual = std::nullopt;
                }
            }

            // Also search external output ports
            auto outputPortPinPositions = getOutputPortPinPositions(model.outputPorts);
            for (int pi = 0 ; pi < outputPortPinPositions.size() ; pi++)
            {
                if (isMouseOverPoint(pressPos, outputPortPinPositions[pi], 10.0f))
                {

                    if (isInputPinConnected(model.mainInst, -1, pi)) continue;

                    editorState.currentConnectionInfo->toComp = -1;
                    editorState.currentConnectionInfo->inPin = pi;
                    editorState.currentConnectionVisual->isBeingDrawn = false;
                    model.addConnection(*editorState.currentConnectionInfo, *editorState.currentConnectionVisual);
                    editorState.currentConnectionInfo = std::nullopt;
                    editorState.currentConnectionVisual = std::nullopt;
                }
            }
        }
    } 
    
    // Toggle inputport value
    if (mp.button == sf::Mouse::Button::Right)
    {
        for (int ip = 0 ; ip < model.inputPorts.size() ; ip++)
        {
            auto& inputPort = model.inputPorts[ip];
            sf::Vector2f topLeftPoint = inputPort.position + sf::Vector2f(0.0f, -editorState.padding*editorState.gridSize); // Quirk of how I draw
            if (isMouseOverBox(pressPos, inputPort.position, inputPort.size))
            {
                state.currentValues[ip] = !state.currentValues[ip];
                //std::cout<<"Input Toggled\n";
            }
        }
    }

    
}




void SimController::setupButtons(tgui::Gui& gui)
{
    const int buttonWidth = 80;
    const int buttonHeight = 40;
    const int padding = 10;

    std::vector<std::string> buttonNames = { "And", "Or", "Not", "Input", "Output", "Finish" };

    for (size_t i = 0; i < buttonNames.size(); ++i)
    {
        // Create button
        auto button = tgui::Button::create(buttonNames[i]);
        button->setSize(buttonWidth, buttonHeight);
        button->setPosition(
            padding + i * (buttonWidth + padding),
            window.getSize().y - padding - buttonHeight
        );


        if (buttonNames[i] == "And")
        {
            button->onPress([this]() {
                
                model.addComponent(ComponentType::AND);
                editorState.placingComponent = true;
            });
        }
        if (buttonNames[i] == "Or")
        {
            button->onPress([this]() {
                
                model.addComponent(ComponentType::OR);
                editorState.placingComponent = true;
            });
        }
        if (buttonNames[i] == "Not")
        {
            button->onPress([this]() {
                
                model.addComponent(ComponentType::NOT);
                editorState.placingComponent = true;
            });
        }
        if (buttonNames[i] == "Input")
        {
            button->onPress([this]() {
                
                auto newInputPort = InputPort();
                newInputPort.isBeingPlaced = true;
                model.addInputPort(newInputPort);
                editorState.placingInputPort = true;
            });
        }
        if (buttonNames[i] == "Output")
        {
            button->onPress([this]() {
                
                auto newOutputPort = OutputPort();
                newOutputPort.isBeingPlaced = true;
                model.addOutputPort(newOutputPort);
                editorState.placingOutputPort = true;
            });
        }             
        // Add button to GUI
        gui.add(button);

        // Store it for later access (optional)
        buttons.push_back(button);
    }
}