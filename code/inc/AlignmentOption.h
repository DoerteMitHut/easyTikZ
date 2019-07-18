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
	//should return vector with 3 pairs (push_back m_gridSize, then m_gridBottomLeft, then m_gridTopRight)
	virtual std::vector<std::pair<float, float>> align(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>&, std::vector<std::shared_ptr<Connection>>&, float, float) = 0;

protected:
		std::pair<float, float> m_gridSize;
		std::pair<float, float> m_gridBottomLeft;
		std::pair<float, float> m_gridTopRight;
};
