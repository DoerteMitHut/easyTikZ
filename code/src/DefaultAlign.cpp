#include "DefaultAlign.h"
#include <cmath>


//##### PUBLIC #####

std::pair<float, float> DefaultAlign::align(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& inputMap, std::vector<std::shared_ptr<Connection>>& inputConnections, float gridSizeX, float gridSizeY)
{
    //determine m_gridSize(s) for current input
    automaticGridSize(inputMap, gridSizeX, gridSizeY);

    //align elements of Diagram
    alignNodesToGrid(inputMap);
    alignIntermediateCornersToGrid(inputConnections);

    std::pair<float, float> output;
    output.first = m_gridSizeX;
    output.second = m_gridSizeY;

    return output;
}



//##### UTILITY #####

void DefaultAlign::automaticGridSize(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input, float gridSizeX, float gridSizeY)
{
    m_gridSizeX = 0.0;
    m_gridSizeY = 0.0;
    
    int counter = 0;

    for(const auto& rectangle : input[typeid(std::shared_ptr<Rectangle>)])
    {
        auto& currentRec = (std::shared_ptr<Rectangle>&)rectangle;

        m_gridSizeX += currentRec->getMinWidth();
        m_gridSizeY += currentRec->getMinHeight();

        counter++;
    }

    for(const auto& circle : input[typeid(std::shared_ptr<Circle>)])
    {
        auto& currentCirc = (std::shared_ptr<Circle>&)circle;

        m_gridSizeX += currentCirc->getMinSize();
        m_gridSizeY += currentCirc->getMinSize();

        counter++;
    }

    for(const auto& polygon : input[typeid(std::shared_ptr<Polygon>)])
    {
        auto& currentPoly = (std::shared_ptr<Circle>&)polygon;

        m_gridSizeX += currentPoly->getMinSize();
        m_gridSizeY += currentPoly->getMinSize();

        counter++;
    }
    
    if(counter == 0)
    {
        m_gridSizeX = gridSizeX;
        m_gridSizeY = gridSizeY;
    }
    else if(m_gridSizeX > m_gridSizeY)
    {
        m_gridSizeX /= counter;
        m_gridSizeY /= counter;

        auto count = std::round(m_gridSizeX / m_gridSizeY);
        m_gridSizeX = count * m_gridSizeY;
    }
    else if(m_gridSizeX < m_gridSizeY)
    {
        m_gridSizeX /= counter;
        m_gridSizeY /= counter;

        auto count = std::round(m_gridSizeY / m_gridSizeX);
        m_gridSizeY = count * m_gridSizeX;
    }
}

void DefaultAlign::alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input)
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

void DefaultAlign::alignIntermediateCornersToGrid(std::vector<std::shared_ptr<Connection>>& input)
{
    for (const auto& con : input)
    {
        auto coords = con->getIntermediateCorners();

        for (auto& coord : coords)
        {
            auto gridCount = std::round(coord.first / m_gridSizeX);
            coord.first = gridCount * m_gridSizeX;
            gridCount = std::round(coord.second / m_gridSizeY);
            coord.second = gridCount * m_gridSizeY;
        }

        con->setIntermediateCorners(coords);
    }
}