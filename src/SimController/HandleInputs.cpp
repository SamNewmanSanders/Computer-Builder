#include "SimController.h"

#include "Helpers/GeometeryHelpers.h"
#include "Helpers/IndexHelpers.h"


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

        // Also stop running sim if any key is pressed - otherwise weird behaviour
        
        if (auto* kp = event->getIf<sf::Event::KeyPressed>()) handleKeyPress(*kp);
        else if (auto* kr = event->getIf<sf::Event::KeyReleased>()) handleKeyRelease(*kr);
        else if (auto* mm = event->getIf<sf::Event::MouseMoved>()) handleMouseMove(*mm);
        else if (auto* mp = event->getIf<sf::Event::MouseButtonPressed>()) handleMousePress(*mp);
        else if (auto* mr = event->getIf<sf::Event::MouseButtonReleased>()) handleMouseRelease(*mr);
    }
}


void SimController::handleKeyPress(const sf::Event::KeyPressed& kp)
{
    if (kp.code == sf::Keyboard::Key::S) editorState.stepSim = true;

    if (kp.code == sf::Keyboard::Key::R) editorState.runSim = !editorState.runSim;

    if (kp.code == sf::Keyboard::Key::Escape) window.close();

    if (kp.code == sf::Keyboard::Key::Backspace && editorState.mode == EditorMode::PlacingComponent) 
        model.cancelPlacement(editorState.mode);
    if (kp.code == sf::Keyboard::Key::Backspace && editorState.mode == EditorMode::PlacingInputPort) 
        model.cancelPlacement(editorState.mode);
    if (kp.code == sf::Keyboard::Key::Backspace && editorState.mode == EditorMode::PlacingOutputPort)
        model.cancelPlacement(editorState.mode);
    if (kp.code == sf::Keyboard::Key::Backspace && connectionBuilder.isActive()) 
        {connectionBuilder.cancel(); editorState.mode = EditorMode::Idle;}
}

void SimController::handleKeyRelease(const sf::Event::KeyReleased& kr)
{
}

void SimController::handleMouseMove(const sf::Event::MouseMoved& mm)
{
    auto& def = model.def;
    auto& state = model.state;

    editorState.mousePos = static_cast<sf::Vector2f>(mm.position);
    editorState.snappedMousePos = Helpers::snapToGrid(editorState.mousePos, editorState.gridSize);
    editorState.snappedMousePosRounded = Helpers::snapToGridRounded(editorState.mousePos, editorState.gridSize);

    // Snap a component you are moving to the grid
    if (editorState.mode == EditorMode::PlacingComponent)
    {
        if (state.componentVisuals.back().isGhost)
            state.componentVisuals.back().position = editorState.snappedMousePos;
        else std::cerr<<"Component being placed is not a ghost\n";          // Should never happen but check anyway
    }

    if (editorState.mode == EditorMode::PlacingInputPort)
    {
        if (model.inputPorts.back().isBeingPlaced)
            model.inputPorts.back().position = editorState.snappedMousePos;
        else std::cerr<<"Input port being placed does NOT think its being placed????\n";
    }
    if (editorState.mode == EditorMode::PlacingOutputPort)
    {
        if (model.outputPorts.back().isBeingPlaced)
            model.outputPorts.back().position = editorState.snappedMousePos;
        else std::cerr<<"Output port being placed does NOT think its being placed????\n";
    }

    if (connectionBuilder.isActive())   // Update the position of the end of the wire to where the mouse is
    {
        connectionBuilder.updateEnd(editorState.snappedMousePosRounded, editorState.gridSize);
    }

    // Allow inputs to change by dragging mouse while right clicking
    if (editorState.rightMouseDown)
    {
        toggleInputUnderMouse();
    }
}



void SimController::handleMousePress(const sf::Event::MouseButtonPressed& mp)
{
    auto& def = model.def;
    auto& state = model.state;

    sf::Vector2f pressPos = static_cast<sf::Vector2f>(mp.position);

    if (mp.button == sf::Mouse::Button::Right)
    {
        editorState.rightMouseDown = true;
        editorState.lastToggledInput = -1; // reset for new drag

        // Also toggle immediately at click location
        editorState.mousePos = static_cast<sf::Vector2f>(mp.position);
        toggleInputUnderMouse();
    }

    // Finish placing a component
    if (mp.button == sf::Mouse::Button::Left && editorState.mode == EditorMode::PlacingComponent) 
    {
        state.componentVisuals.back().isGhost = false;
        editorState.mode = EditorMode::Idle;
    }

    // Finish placing an inputPort / outputPort
    if(mp.button == sf::Mouse::Button::Left && editorState.mode == EditorMode::PlacingInputPort)
    {
        model.inputPorts.back().isBeingPlaced = false;
        editorState.mode = EditorMode::Idle;
    }
    if(mp.button == sf::Mouse::Button::Left && editorState.mode == EditorMode::PlacingOutputPort)
    {
        model.outputPorts.back().isBeingPlaced = false;
        editorState.mode = EditorMode::Idle;
    }    


    // If NOT drawing a wire, start drawing on pin mouse is hovering over
    if (mp.button == sf::Mouse::Button::Left && !connectionBuilder.isActive())
    {   
        for (int ci = 0 ; ci < def.components.size() ; ci++)
        {
            // Search output pins for wire start
            auto outputPinPositions = Helpers::getPinPositions(def.components[ci], state.componentVisuals[ci], false, editorState.gridSize);
            for (int pi = 0 ; pi < outputPinPositions.size() ; pi++) 
            {
                if (Helpers::isMouseOverPoint(pressPos, outputPinPositions[pi], 10.0f))
                {
                    connectionBuilder.start(ci, pi, outputPinPositions[pi], editorState.gridSize);
                    editorState.mode = EditorMode::DrawingConnection;
                }
            }
        }
        // Loop over input ports aswell 
        auto inputPortPinPositions = Helpers::getInputPortPinPositions(model.inputPorts);
        for (int pi = 0 ; pi < inputPortPinPositions.size() ; pi++)
        {
            if (Helpers::isMouseOverPoint(pressPos, inputPortPinPositions[pi], 10.0f))
            {
                connectionBuilder.start(-1, pi, inputPortPinPositions[pi], editorState.gridSize);
                editorState.mode = EditorMode::DrawingConnection;
            }
        }
    }

    // IF drawing a wire, add a corner or finish the wire
    if (mp.button == sf::Mouse::Button::Left && connectionBuilder.isActive())
    {   
        bool finishedWire = false;
        for (int ci = 0 ; ci < def.components.size() ; ci++)
        {
            // Search input pins for wire end
            auto inputPinPositions = Helpers::getPinPositions(def.components[ci], state.componentVisuals[ci], true, editorState.gridSize);
            for (int pi = 0 ; pi < inputPinPositions.size() ; pi++) 
            {
                if (Helpers::isMouseOverPoint(pressPos, inputPinPositions[pi], editorState.gridSize / 2))
                {
                    // Make sure input pin isn't already connected
                    if (Helpers::isInputPinConnected(def, state, ci, pi)) continue;
                    // Finish the connection
                    connectionBuilder.finish(ci, pi, model, inputPinPositions[pi]);
                    finishedWire = true;
                    editorState.mode = EditorMode::Idle;
                }
            }
        }
        // Also search external output ports
        auto outputPortPinPositions = Helpers::getOutputPortPinPositions(model.outputPorts);
        for (int pi = 0 ; pi < outputPortPinPositions.size() ; pi++)
        {
            if (Helpers::isMouseOverPoint(pressPos, outputPortPinPositions[pi], editorState.gridSize / 2))
            {
                if (Helpers::isInputPinConnected(def, state, -1, pi)) continue;
                // Finish the connection
                connectionBuilder.finish(-1 , pi, model, outputPortPinPositions[pi]);
                finishedWire = true;
                editorState.mode = EditorMode::Idle;
            }
        }
        
        // If the wire wasn't finished, the user is trying to add a corner
        if (!finishedWire)
            connectionBuilder.addCorner(editorState.snappedMousePosRounded, editorState.gridSize);
    } 
}

void SimController::handleMouseRelease(const sf::Event::MouseButtonReleased& mr)
{
    if (mr.button == sf::Mouse::Button::Right)
    {
        editorState.rightMouseDown = false;
        editorState.lastToggledInput = -1; // reset
    }
}


void SimController::toggleInputUnderMouse()
{
    auto& state = model.state;

    for (int ip = 0; ip < model.inputPorts.size(); ip++)
    {
        auto& inputPort = model.inputPorts[ip];

        // Quirk of rendering, the origin (position) is the top left most pin. Adjust so helper can work
        sf::Vector2f topLeftPoint = Helpers::positionToTopLeft(editorState.gridSize, editorState.padding, inputPort.position);
        if (Helpers::isMouseOverBox(editorState.mousePos, topLeftPoint, inputPort.size))
        {
            // only toggle if it's not the same as last toggled
            if (editorState.lastToggledInput != ip)
            {
                state.currentValues[ip] = !state.currentValues[ip];
                editorState.lastToggledInput = ip;
            }
        }
    }
}