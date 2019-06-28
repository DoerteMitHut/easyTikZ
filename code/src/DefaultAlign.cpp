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
    
    if(m_gridSizeX == m_gridSizeY)
    {
        //that's cool
    }
    else if(m_gridSizeX > m_gridSizeY)
    {
        auto count = std::round(m_gridSizeX / m_gridSizeY);
        m_gridSizeX = count * m_gridSizeY;
    }
    else if(m_gridSizeX < m_gridSizeY)
    {
        auto count = std::round(m_gridSizeY / m_gridSizeX);
        m_gridSizeY = count * m_gridSizeX;
    }
}

void DefaultAlign::alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> input)
{
    for (const auto& it : input)
    {
        //adjust coordinates and minSize(s) of all elements of vector
        for (const auto& node : it.second)
        {
            //coordinates
            auto x = node->getRootCoordX();
            auto y = node->getRootCoordY();

            auto position = std::round(x / m_gridSizeX);
            x = position * m_gridSizeX;
            position = std::round(y / m_gridSizeY);
            y = position * m_gridSizeY;

            node->setRootCoordX(x);
            node->setRootCoordY(y);

            //size(s)
            if(it.first == typeid(std::__shared_ptr<Rectangle>&))
            {
                auto& currentNode = (std::shared_ptr<Rectangle>&)node;
                auto width = currentNode->getMinWidth();
                auto height = currentNode->getMinHeight();

                auto gridCount = std::round(width/m_gridSizeX);
                width = gridCount * m_gridSizeX;
                gridCount = std::round(height/m_gridSizeY);
                height = gridCount * m_gridSizeY;

                currentNode->setMinWidth(width);
                currentNode->setMinHeight(height);
            }
            else if(it.first == typeid(std::__shared_ptr<Circle>&))
            {
                auto& currentNode = (std::shared_ptr<Circle>&)node;
                auto size = currentNode->getMinSize();
                auto gridSizeSmaller = (m_gridSizeX <= m_gridSizeY ? m_gridSizeX : m_gridSizeY);

                auto gridCount = std::round(size/gridSizeSmaller);
                size = gridCount * gridSizeSmaller;

                currentNode->setMinSize(size);
            }
            else if(it.first == typeid(std::__shared_ptr<Polygon>&))
            {
                auto& currentNode = (std::shared_ptr<Polygon>&)node;
                auto size = currentNode->getMinSize();
                auto gridSizeSmaller = (m_gridSizeX <= m_gridSizeY ? m_gridSizeX : m_gridSizeY);

                auto gridCount = std::round(size/gridSizeSmaller);
                size = gridCount * gridSizeSmaller;

                currentNode->setMinSize(size);
            }
        }
    }
}
