#pragma once

#include "AlignmentOption.h"
#include <utility>


class DefaultAlign : public AlignmentOption
{
    public:
		std::pair<float, float> align(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>&, std::vector<std::shared_ptr<Connection>>&, float, float) override;

    private:
		void automaticGridSize(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input, float, float);
		void alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input);
		void alignIntermediateCornersToGrid(std::vector<std::shared_ptr<Connection>>& input);

		float m_gridSizeX;
		float m_gridSizeY;
};
