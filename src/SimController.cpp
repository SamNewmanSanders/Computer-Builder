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
    if (kp.code == sf::Keyboard::Key::S)
        {
            editorState.updateSim = true;
        }
    if (kp.code == sf::Keyboard::Key::Escape)
        {
            window.close();
        }
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

    if (mp.button == sf::Mouse::Button::Left && editorState.placingComponent) 
    {
        state.componentVisuals.back().isGhost = false;
        editorState.placingComponent = false;
    }

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

        // TODO: FUNCTION THAT RETURNS THE OUTPUTPIN POSITIONS OF ALL INPUTPORTS
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
        // Add button to GUI
        gui.add(button);

        // Store it for later access (optional)
        buttons.push_back(button);
    }
}