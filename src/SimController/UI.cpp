#include "SimController.h"
#include "EditorState.h"
#include "Helpers/FileHelpers.h"

#include <filesystem>
#include <fstream>

void SimController::setupButtons(tgui::Gui& gui)
{
    const int buttonWidth = 80;
    const int buttonHeight = 40;
    const int padding = 10;

    std::vector<std::string> buttonNames = { "Input", "Output", "And", "Or", "Not", "Finish", "Restart" };

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
                
                model.addComponent(model.def, model.state, ComponentType::AND);
                editorState.mode = EditorMode::PlacingComponent;
                editorState.runSim = false; // Strange behaviour otherwise
            });
        }
        if (buttonNames[i] == "Or")
        {
            button->onPress([this]() {
                
                model.addComponent(model.def, model.state, ComponentType::OR);
                editorState.mode = EditorMode::PlacingComponent;
                editorState.runSim = false; // Strange behaviour otherwise
            });
        }
        if (buttonNames[i] == "Not")
        {
            button->onPress([this]() {
                
                model.addComponent(model.def, model.state, ComponentType::NOT);
                editorState.mode = EditorMode::PlacingComponent;
                editorState.runSim = false; // Strange behaviour otherwise
            });
        }
        if (buttonNames[i] == "Input")
        {
            button->onPress([this]() {
                
                auto newInputPort = InputPort();
                newInputPort.isBeingPlaced = true;
                model.addInputPort(newInputPort);
                editorState.mode = EditorMode::PlacingInputPort;
                editorState.runSim = false; // Strange behaviour otherwise
            });
        }
        if (buttonNames[i] == "Output")
        {
            button->onPress([this]() {
                
                auto newOutputPort = OutputPort();
                newOutputPort.isBeingPlaced = true;
                model.addOutputPort(newOutputPort);
                editorState.mode = EditorMode::PlacingOutputPort;
                editorState.runSim = false; // Strange behaviour otherwise
            });
        }        
        if (buttonNames[i] == "Finish")
        {
            button->onPress([this, &gui]() {
                createFinishPopup(gui);
            });
        }   

        if (buttonNames[i] == "Restart")
        {
            button->onPress([this, &gui]() {
                model.reset();
                connectionBuilder.cancel();
            });
        }   

        // Add button to GUI
        gui.add(button);
    }

    setupCircuitDropdown(gui);
}


void SimController::setupCircuitDropdown(tgui::Gui& gui)
{
    // Create a ComboBox
    circuitDropdown = tgui::ComboBox::create();
    circuitDropdown->setSize(200, 30);

    // Position it in the bottom-right corner
    float xPos = window.getSize().x - circuitDropdown->getSize().x - 10; // 10px padding from right edge
    float yPos = window.getSize().y - circuitDropdown->getSize().y - 10; // 10px padding from bottom edge
    circuitDropdown->setPosition(xPos, yPos);

    // Clear and populate the dropdown with .json files in "circuits" folder
    refreshCircuitDropdown(circuitDropdown);

    // Handle selection
    circuitDropdown->onItemSelect([this, &gui](const tgui::String& selected) {
        
        std::cout << "Selected circuit: " << selected << std::endl;

        std::string path = "../circuits/" + selected.toStdString() + ".json";
        
        if (Helpers::jsonFileExists(path))
        {
            std::cout << "Circuit JSON exists\n";
            model.addComponent(model.def, model.state, ComponentType::SUBCIRCUIT, selected.toStdString());
            editorState.mode = EditorMode::PlacingComponent;
            circuitDropdown->deselectItem();
        }
        else
        {
            std::cerr << "JSON file missing: " << path << std::endl;
        }
    });

    gui.add(circuitDropdown);
}


void SimController::refreshCircuitDropdown(tgui::ComboBox::Ptr circuitDropdown)
{
    namespace fs = std::filesystem;

    circuitDropdown->removeAllItems(); // clear it
    const std::string folderPath = "../circuits";

    if (fs::exists(folderPath) && fs::is_directory(folderPath)) {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                std::string filename = entry.path().stem().string();
                circuitDropdown->addItem(filename);
            }
        }
    }
}


void SimController::createFinishPopup(tgui::Gui& gui) {
    auto windowPopup = tgui::ChildWindow::create("Enter Circuit Name");
    windowPopup->setSize(300, 150);
    windowPopup->setPosition(
        (window.getSize().x - 300) / 2.f,
        (window.getSize().y - 150) / 2.f
    );
    windowPopup->setResizable(false);

    auto nameBox = tgui::EditBox::create();
    nameBox->setSize(250, 30);
    nameBox->setPosition(25, 40);
    nameBox->setDefaultText("Enter name...");

    auto submitButton = tgui::Button::create("OK");
    submitButton->setSize(100, 30);
    submitButton->setPosition(100, 90);

    submitButton->onPress([this, nameBox, windowPopup, &gui]() {
        std::string circuitName = nameBox->getText().toStdString();
        model.finishCircuit(circuitName);
        gui.remove(windowPopup);
        refreshCircuitDropdown(circuitDropdown);
    });

    windowPopup->add(nameBox);
    windowPopup->add(submitButton);
    gui.add(windowPopup);
}

