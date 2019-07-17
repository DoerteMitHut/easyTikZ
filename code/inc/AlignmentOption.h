#pragma once

#include "Shapes.h"
#include "Connection.h"

#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>

class AlignmentOption
{
public:
	virtual std::pair<float, float> align(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>&, std::vector<std::shared_ptr<Connection>>&, float, float) = 0;
};
