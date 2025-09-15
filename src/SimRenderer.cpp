#include "SimRenderer.h"
#include "Helpers.h"

#include <cmath>

SimRenderer::SimRenderer(SimModel& model_, EditorState& editorState_, sf::RenderWindow& window_) 
: model(model_), editorState(editorState_), window(window_) {}

void SimRenderer::render()
{
    drawGrid();

    auto& def = model.mainInst.def;
    auto& state = model.mainInst.state;

    // Sanity checks
    if (def->components.size() != state.componentVisuals.size()) std::cerr<<"Component info and visuals not same size\n";
    if (def->connections.size() != state.connectionVisuals.size()) std::cerr<<"Connection info and visuals not same size\n";

    // Draw all components in the netlist (includes the one currently being placed)
    for (int ci = 0 ; ci < def->components.size() ; ci++)
    {
        drawComponent(def->components[ci], state.componentVisuals[ci]);
    }

    // Draw all i/o ports
    for (auto& i : model.inputPorts) drawInputPort(i);
    for (auto& o : model.outputPorts) drawOutputPort(o);

    // Draw all connections (wires) and ghost too
    for (int conn = 0 ; conn < def->connections.size() ; conn++)
    {
        drawConnection(def->connections[conn], state.connectionVisuals[conn]);
    }
    if (editorState.currentConnectionInfo)
        drawConnection(*editorState.currentConnectionInfo, *editorState.currentConnectionVisual);
}

void SimRenderer::drawConnection(ConnectionInfo& info, ConnectionVisual& visual)
{
    sf::Vector2f start;

    // Determine the start position
    if (info.fromComp == -1)
    {
        // Wire starts at an InputPort
        auto outputPinPositions = getInputPortPinPositions(model.inputPorts);
        start = outputPinPositions[info.outPin];
    }
    else
    {
        auto& startInfo = model.mainInst.def->components[info.fromComp];
        auto& startVis  = model.mainInst.state.componentVisuals[info.fromComp];
        auto outputPinPositions = getPinPositions(startInfo, startVis, false, editorState.gridSize);
        start = outputPinPositions[info.outPin];
    }

    sf::Vector2f end;
    if (visual.isBeingDrawn) {
        end = visual.tempEndPos;
    } else 
    {
        if (info.toComp == -1)
        {
            // Wire ends at OutputPort
            auto inputPinPositions = getOutputPortPinPositions(model.outputPorts);
            end = inputPinPositions[info.inPin];
        }
        else
        {
            auto& endInfo = model.mainInst.def->components[info.toComp];
            auto& endVisual = model.mainInst.state.componentVisuals[info.toComp];
            auto inputPinPositions = getPinPositions(endInfo, endVisual, true, editorState.gridSize);
            end = inputPinPositions[info.inPin];
        }
    }

    // Compute the vector from start to end
    sf::Vector2f delta = end - start;
    float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);

    float thickness = editorState.wireThickness;

    // Create a rectangle with the correct length and thickness
    sf::RectangleShape wireShape(sf::Vector2f(length, thickness));
    sf::Color color;
    if (visual.isBeingDrawn) color = sf::Color::Yellow;
    else
    { 
        bool value = getOutputPinValue(model.mainInst, info.fromComp, info.outPin);
        color = value ? sf::Color::Green : sf::Color::Red;
    } 
    wireShape.setFillColor(color); // or any color you like

    // Set origin to left-center so rotation works naturally
    wireShape.setOrigin(sf::Vector2f(0.f, thickness / 2.f));
    wireShape.setPosition(start);

    // Compute angle in degrees
    float angle = std::atan2(delta.y, delta.x) * 180.f / 3.14159265f;
    wireShape.setRotation(sf::radians(std::atan2(delta.y, delta.x)));

    // Draw the wire
    window.draw(wireShape);
}


void SimRenderer::drawGrid()
{

    sf::Vector2u windowSize = window.getSize();
    sf::Color gridColor(200, 200, 200, 100); // Light gray with transparency

    float thickness = 1.0f; // Line thickness

    // Vertical lines
    for (float x = 0.f; x < windowSize.x; x += editorState.gridSize)
    {
        sf::RectangleShape line(sf::Vector2f(thickness, windowSize.y - editorState.bottomOffset));
        line.setPosition(sf::Vector2f(x, 0.f));
        line.setFillColor(gridColor);
        window.draw(line);
    }

    // Horizontal lines
    for (float y = 0.f; y < windowSize.y - editorState.bottomOffset; y += editorState.gridSize)
    {
        sf::RectangleShape line(sf::Vector2f(windowSize.x, thickness));
        line.setPosition(sf::Vector2f(0.f, y));
        line.setFillColor(gridColor);
        window.draw(line);
    }
}


void SimRenderer::drawComponent(ComponentInfo& info, ComponentVisual& visual)
{
    sf::Vector2f componentPos = visual.position; 
    int maxPins = std::max(info.numInputs, info.numOutputs);

    float padding = editorState.gridSize * editorState.padding;
    float height = (maxPins - 1) * editorState.gridSize + padding * 2;
    float width  = 2.f * editorState.gridSize;

    sf::RectangleShape compShape;
    compShape.setSize(sf::Vector2f(width, height));
    compShape.setOutlineColor(sf::Color::White);
    compShape.setOutlineThickness(2.f);
    compShape.setOrigin(sf::Vector2f(0.f, padding));    // Set origin at top leftmost input pin
    compShape.setPosition(componentPos);
    visual.size = sf::Vector2f(width, height);

    if (visual.isGhost)
        compShape.setFillColor(sf::Color(200, 200, 200, 100)); // transparent ghost
    else
        compShape.setFillColor(sf::Color(200, 200, 200));      // solid component

    window.draw(compShape);

    // Pin radius
    float radius = editorState.gridSize / 4.f;

    // Draw input pins
    for (int ipi = 0; ipi < info.numInputs; ipi++)
    {
        sf::CircleShape pinShape(radius);
        pinShape.setOrigin(sf::Vector2f(radius, radius));
        pinShape.setFillColor(sf::Color::White);

        float relY = ipi * editorState.gridSize;
        float relX = 0.0f;
        sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
        pinShape.setPosition(pinPos);
        window.draw(pinShape);
    }

    // Draw output pins
    for (int opi = 0; opi < info.numOutputs; opi++)
    {
        sf::CircleShape pinShape(radius);
        pinShape.setOrigin(sf::Vector2f(radius, radius));
        pinShape.setFillColor(sf::Color::White);

        float relY = opi * editorState.gridSize;
        float relX = width;
        sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
        pinShape.setPosition(pinPos);
        window.draw(pinShape);
    }
}

void SimRenderer::drawInputPort(InputPort& inputPort)
{
    sf::Vector2f componentPos = inputPort.position;

    float padding = editorState.gridSize * editorState.padding;
    float height = padding * 2;
    float width  = 2.f * editorState.gridSize;

    sf::RectangleShape compShape;
    compShape.setSize(sf::Vector2f(width, height));
    compShape.setOutlineColor(sf::Color::White);
    compShape.setOutlineThickness(2.f);
    compShape.setOrigin(sf::Vector2f(0.f, padding));    // Set origin at top leftmost input pin
    compShape.setPosition(componentPos);
    inputPort.size = sf::Vector2f(width, height);

    if (inputPort.isBeingPlaced)
        compShape.setFillColor(sf::Color(200, 200, 200, 100)); // transparent ghost
    else
        compShape.setFillColor(sf::Color(200, 200, 200));      // solid component

    window.draw(compShape);

    // Pin radius
    float radius = editorState.gridSize / 4.f;

    // Draw singular output pin
    sf::CircleShape pinShape(radius);
    pinShape.setOrigin(sf::Vector2f(radius, radius));
    pinShape.setFillColor(sf::Color::White);

    float relY = 0.0f;
    float relX = width;
    sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
    pinShape.setPosition(pinPos);
    window.draw(pinShape);
}


void SimRenderer::drawOutputPort(OutputPort& outputPort)
{
    sf::Vector2f componentPos = outputPort.position;

    float padding = editorState.gridSize * editorState.padding;
    float height = padding * 2;
    float width  = 2.f * editorState.gridSize;

    sf::RectangleShape compShape;
    compShape.setSize(sf::Vector2f(width, height));
    compShape.setOutlineColor(sf::Color::White);
    compShape.setOutlineThickness(2.f);
    compShape.setOrigin(sf::Vector2f(0.f, padding));    // Set origin at top leftmost input pin
    compShape.setPosition(componentPos);
    outputPort.size = sf::Vector2f(width, height);

    if (outputPort.isBeingPlaced)
        compShape.setFillColor(sf::Color(200, 200, 200, 100)); // transparent ghost
    else
        compShape.setFillColor(sf::Color(200, 200, 200));      // solid component

    window.draw(compShape);

    // Pin radius
    float radius = editorState.gridSize / 4.f;

    // Draw singular input pin
    sf::CircleShape pinShape(radius);
    pinShape.setOrigin(sf::Vector2f(radius, radius));
    pinShape.setFillColor(sf::Color::White);

    float relY = 0.0f;
    float relX = 0.0f;
    sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
    pinShape.setPosition(pinPos);
    window.draw(pinShape);
}