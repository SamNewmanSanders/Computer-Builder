#pragma once

// This file deals with loading the json data into useful forms

#include "Netlist.h"
#include "SimModel.h"
#include "GeneralHelpers.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <memory>

namespace Helpers
{

std::shared_ptr<Netlist> loadNetlistFromJson(const std::string& filename);

bool jsonFileExists(const std::string& path);

}