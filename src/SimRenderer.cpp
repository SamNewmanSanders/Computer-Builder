#include "SimRenderer.h"

SimRenderer::SimRenderer(SimModel& model_, EditorState& editorState_, sf::RenderWindow& window_) 
: model(model_), editorState(editorState), window(window_) {}

void SimRenderer::render(){}

void SimRenderer::drawComponent(ComponentInfo& info, ComponentVisual& visual)
{

}