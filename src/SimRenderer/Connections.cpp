#include "SimRenderer.h"

#include "Helpers/GeometeryHelpers.h"
#include "Helpers/IndexHelpers.h"
#include <cmath>


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