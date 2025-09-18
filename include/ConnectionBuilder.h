#pragma once

#include "Netlist.h"
#include "SimModel.h"

struct ConnectionBuilder
{
    std::optional<ConnectionInfo> info = std::nullopt;
    std::optional<ConnectionVisual> visual = std::nullopt;

    void start(int fromComp, int outPin, sf::Vector2f startPos, int gridSize);
    void updateEnd(sf::Vector2f pos, int gridSize);
    void addCorner(sf::Vector2f pos, int gridSize);
    void finish(int toComp, int inPin, SimModel& model, sf::Vector2f endPos);
    void cancel();

    
    bool isActive() const { return info.has_value() && visual.has_value(); }
};