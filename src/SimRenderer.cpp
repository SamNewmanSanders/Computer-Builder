#include "SimRenderer.h"

#include "Helpers/GeometeryHelpers.h"
#include "Helpers/IndexHelpers.h"
#include <cmath>

SimRenderer::SimRenderer(SimModel& model_, SimController& controller_, EditorState& editorState_, sf::RenderWindow& window_) 
: model(model_), controller(controller_), editorState(editorState_), window(window_) 
{
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font\n";
    }
}

void SimRenderer::render()
{
    drawGrid();

    auto& def = model.def;
    auto& state = model.state;

    // Sanity checks
    if (def.components.size() != state.componentVisuals.size()) std::cerr<<"Component info and visuals not same size\n";
    if (def.connections.size() != state.connectionVisuals.size()) std::cerr<<"Connection info and visuals not same size\n";

    // Draw all components in the netlist (includes the one currently being placed)
    for (int ci = 0 ; ci < def.components.size() ; ci++)
    {
        drawComponent(def.components[ci], state.componentVisuals[ci]);
    }

    // Draw all i/o ports
    for (auto& i : model.inputPorts) drawInputPort(i);
    for (auto& o : model.outputPorts) drawOutputPort(o);

    // Draw all connections (wires) and ghost too
    for (int conn = 0 ; conn < def.connections.size() ; conn++)
    {
        drawConnection(def.connections[conn], state.connectionVisuals[conn]);
    }
    if (editorState.mode == EditorMode::DrawingConnection)
        drawConnection(*controller.connectionBuilder.info, *controller.connectionBuilder.visual);
}


void SimRenderer::drawConnection(ConnectionInfo& info, ConnectionVisual& visual)
{
    // Pick wire color
    sf::Color color;
    if (visual.isBeingDrawn) {
        color = sf::Color::Yellow;
    } else {
        bool value = Helpers::getOutputPinValue(model.def, model.state, info.fromComp, info.outPin);
        color = value ? sf::Color::Green : sf::Color::Red;
    }

    float thickness = editorState.wireThickness;

    // Draw each segment in the polyline
    for (size_t i = 0; i + 1 < visual.wirePoints.size(); ++i)
    {
        sf::Vector2f start = visual.wirePoints[i];
        sf::Vector2f end   = visual.wirePoints[i + 1];

        sf::Vector2f delta = end - start;
        float length = std::sqrt(delta.x * delta.x + delta.y * delta.y);

        // Rectangle for this segment
        sf::RectangleShape seg(sf::Vector2f(length, thickness));
        seg.setFillColor(color);

        // origin at left-center
        seg.setOrigin(sf::Vector2f(0.f, thickness / 2.f));
        seg.setPosition(start);

        // rotation
        float angle = std::atan2(delta.y, delta.x) * 180.f / 3.14159265f;
        seg.setRotation(sf::radians(std::atan2(delta.y, delta.x)));

        window.draw(seg);
    }
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

    // Size the width with increasing maxPins so it doesn't look long and thin
    float width  = ((maxPins - 1) / 3 + 2) * editorState.gridSize;

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
        pinShape.setFillColor(sf::Color::Yellow);

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
        pinShape.setFillColor(sf::Color::Yellow);

        float relY = opi * editorState.gridSize;
        float relX = width;
        sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
        pinShape.setPosition(pinPos);
        window.draw(pinShape);
    }


    // Draw text
    sf::Text labelText(font);
    labelText.setString(info.name); 
    labelText.setCharacterSize(14);  // starting size
    labelText.setFillColor(sf::Color::Blue);

    // Measure text width
    sf::FloatRect bounds = labelText.getLocalBounds();
    float textWidth = bounds.size.x;

    // Scale down if wider than component
    float maxTextWidth = width - 4.f; // small padding so it doesn't touch edges
    float scale = 1.0f;
    if (textWidth > maxTextWidth)
        scale = maxTextWidth / textWidth;
    labelText.setScale(sf::Vector2f(scale, scale));

    // Position it centered on the component
    bounds = labelText.getLocalBounds();
    labelText.setOrigin(sf::Vector2f(bounds.size.x / 2.f, bounds.size.y));
    sf::Vector2f compTopLeft = Helpers::positionToTopLeft(editorState.gridSize, editorState.padding, componentPos);
    labelText.setPosition(compTopLeft + sf::Vector2f(width / 2.0f, height / 2.0f));

    window.draw(labelText);
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
    pinShape.setFillColor(sf::Color::Yellow);

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
    pinShape.setFillColor(sf::Color::Yellow);

    float relY = 0.0f;
    float relX = 0.0f;
    sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
    pinShape.setPosition(pinPos);
    window.draw(pinShape);
}