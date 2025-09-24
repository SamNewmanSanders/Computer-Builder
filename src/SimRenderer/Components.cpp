#include "SimRenderer.h"
#include "Helpers/GeometeryHelpers.h"


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
        float relY = ipi * editorState.gridSize;
        float relX = 0.0f;
        sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
        drawPin(pinPos);
    }

    // Draw output pins
    for (int opi = 0; opi < info.numOutputs; opi++)
    {
        float relY = opi * editorState.gridSize;
        float relX = width;
        sf::Vector2f pinPos = componentPos + sf::Vector2f(relX, relY);
        drawPin(pinPos);
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



void SimRenderer::drawPort(InputPort& inputPort)
{
    drawPortHelper(inputPort.position, inputPort.size, inputPort.isBeingPlaced, /*pinOnRight=*/true);
}

void SimRenderer::drawPort(OutputPort& outputPort)
{
    drawPortHelper(outputPort.position, outputPort.size, outputPort.isBeingPlaced, /*pinOnRight=*/false);
}

void SimRenderer::drawPortHelper(sf::Vector2f pos, sf::Vector2f& outSize, bool isGhost, bool pinOnRight)
{
    float padding = editorState.gridSize * editorState.padding;
    float height = padding * 2;
    float width  = 2.f * editorState.gridSize;

    // Rectangle
    sf::RectangleShape rect({width, height});
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(2.f);
    rect.setOrigin(sf::Vector2f(0.f, padding));
    rect.setPosition(pos);
    rect.setFillColor(isGhost ? sf::Color(200,200,200,100) : sf::Color(200,200,200));
    window.draw(rect);

    outSize = {width, height};

    // Pin position (right side for input port, left side for output port)
    sf::Vector2f pinPos = pos + sf::Vector2f(pinOnRight ? width : 0.f, 0.f);
    drawPin(pinPos);
}