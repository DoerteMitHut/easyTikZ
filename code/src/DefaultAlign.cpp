#include "DefaultAlign.h"
#include <cmath>


//##### PUBLIC #####

std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> DefaultAlign::alignMap(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input)
{
    automaticGridSize(input[typeid(std::shared_ptr<Rectangle>)]);

    //alignNodesToGrid(input);

    return input;
}



//##### UTILITY #####

void DefaultAlign::automaticGridSize(std::vector<std::shared_ptr<Shape>> input) //currently only works for rectangles
{
    for (const auto& rectangle : input)
    {
        auto& currentRec = (std::shared_ptr<Rectangle>&)rectangle;

        if ((currentRec->getMinWidth()) < (m_gridSizeX * 2))
        {
            m_gridSizeX = (currentRec->getMinWidth() / 2);
        }
        if ((currentRec->getMinHeight()) < (m_gridSizeY * 2))
        {
            m_gridSizeY = (currentRec->getMinHeight() / 2);
        }
    }
}

//struggling because of nodes being in vectors in unordered_map, just want them all in succession
void DefaultAlign::alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> input)
{
    for (const auto& it : input)
    {
        // adjust coordinates of all elements of vector
        for (const auto& node : it.second)
        {
            // get current coords
            auto x = node->getRootCoordX();
            auto y = node->getRootCoordY();

            // TODO: test this
            // adjust x and y
            auto position = std::round(x / m_gridSizeX);
            x = position * m_gridSizeX;

            position = std::round(y / m_gridSizeY);
            y = position * m_gridSizeY;

            // write back adjusted coords
            node->setRootCoordX(x);
            node->setRootCoordY(y);
        }
    }
}
