#include "DefaultAlign.h"
#include <cmath>


//##### PUBLIC #####

std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> DefaultAlign::alignMap(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input)
{
    automaticGridSize(input);

    alignNodesToGrid(input);

    return input;
}



//##### UTILITY #####

void DefaultAlign::automaticGridSize(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> input)
{
    m_gridSizeX = 1.0;
    m_gridSizeY = 0.5;

    for(const auto& rectangle : input[typeid(std::shared_ptr<Rectangle>)])
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

    for(const auto& circle : input[typeid(std::shared_ptr<Circle>)])
    {
        auto& currentCirc = (std::shared_ptr<Circle>&)circle;

        if ((currentCirc->getMinSize()) < (m_gridSizeX * 2))
        {
            m_gridSizeX = (currentCirc->getMinSize() / 2);
        }
        if ((currentCirc->getMinSize()) < (m_gridSizeY * 2))
        {
            m_gridSizeY = (currentCirc->getMinSize() / 2);
        }
    }

    for(const auto& polygon : input[typeid(std::shared_ptr<Polygon>)])
    {
        auto& currentPoly = (std::shared_ptr<Circle>&)polygon;

        if ((currentPoly->getMinSize()) < (m_gridSizeX * 2))
        {
            m_gridSizeX = (currentPoly->getMinSize() / 2);
        }
        if ((currentPoly->getMinSize()) < (m_gridSizeY * 2))
        {
            m_gridSizeY = (currentPoly->getMinSize() / 2);
        }
    }
}

void DefaultAlign::alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> input)
{
    for (const auto& it : input)
    {
        //adjust coordinates of all elements of vector
        for (const auto& node : it.second)
        {
            auto x = node->getRootCoordX();
            auto y = node->getRootCoordY();

            //adjust x and y
            auto position = std::round(x / m_gridSizeX);
            x = position * m_gridSizeX;
            position = std::round(y / m_gridSizeY);
            y = position * m_gridSizeY;

            node->setRootCoordX(x);
            node->setRootCoordY(y);
        }
    }
}
