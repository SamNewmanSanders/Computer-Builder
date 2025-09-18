#include "ConnectionBuilder.h"
#include "Helpers/IndexHelpers.h"

void ConnectionBuilder::start(int fromComp, int outPin, sf::Vector2f startPos, int gridSize)
{
    // Set the first two members of the ConnectionInfo struct
    info = ConnectionInfo{fromComp, outPin};

    // Also create a connectionVisual that is being drawn (true)
    visual = ConnectionVisual{true};   
    // Set the first point on the wire to the startPos
    visual->wirePoints.push_back(startPos);

    // Also push back another startPos position that acts as the end of the wire until it is updated
}

void ConnectionBuilder::updateEnd(sf::Vector2f pos, int gridSize)
{
    visual->wirePoints.back() = pos;
}

void ConnectionBuilder::addCorner(sf::Vector2f pos, int gridSize)
{
    visual->wirePoints.back() = pos;    // Set the end of the vector to the corner pos
    visual->wirePoints.push_back(pos);  // Set the temporary end to the same position so it can be updated
}

void ConnectionBuilder::finish(int toComp, int inPin, SimModel& model, sf::Vector2f endPos)
{
    // Set the final wire point position
    visual->wirePoints.back() = endPos;

    // Finalize
    info->toComp = toComp;
    info->inPin = inPin;
    visual->isBeingDrawn = false;
    model.addConnection(*info, *visual);
    info = std::nullopt;
    visual = std::nullopt;
}

void ConnectionBuilder::cancel()
{
    info = std::nullopt;
    visual = std::nullopt;
}