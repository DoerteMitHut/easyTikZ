#pragma once

#include "AlignmentOption.h"
#include <utility>


class DefaultAlign : public AlignmentOption
{
    public:
		std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> alignMap(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>&) override;

    private:
		void automaticGridSize(std::vector<std::shared_ptr<Shape>>);
		void alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> input);

		float m_gridSizeX;
		float m_gridSizeY;
};
