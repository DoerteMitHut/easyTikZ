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
	virtual std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> alignMap(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>&) = 0;
};
