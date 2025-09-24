#include "SimRenderer.h"

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
    for (auto& i : model.inputPorts) drawPort(i);
    for (auto& o : model.outputPorts) drawPort(o);

    // Draw all connections (wires) and ghost too
    for (int conn = 0 ; conn < def.connections.size() ; conn++)
    {
        drawConnection(def.connections[conn], state.connectionVisuals[conn]);
    }
    if (editorState.mode == EditorMode::DrawingConnection)
        drawConnection(*controller.connectionBuilder.info, *controller.connectionBuilder.visual);
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

void SimRenderer::drawPin(sf::Vector2f pos)
{
    float r = editorState.gridSize / 4.f;
    sf::CircleShape pin(r);
    pin.setOrigin(sf::Vector2f(r, r));
    pin.setFillColor(sf::Color::Yellow);
    pin.setPosition(pos);
    window.draw(pin);
}