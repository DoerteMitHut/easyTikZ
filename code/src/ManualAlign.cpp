#include "ManualAlign.h"
#include <cmath>


//##### PUBLIC #####

std::vector<std::pair<float, float>> ManualAlign::align(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& inputMap, std::vector<std::shared_ptr<Connection>>& inputConnections, float gridSizeX, float gridSizeY)
{
    m_gridSize.first = gridSizeX;
    m_gridSize.second = gridSizeY;
    
    //align elements of Diagram
    alignNodesToGrid(inputMap);
    alignIntermediateCornersToGrid(inputConnections);

    std::vector<std::pair<float, float>> output;
    output.push_back(m_gridSize);
    output.push_back(m_gridBottomLeft);
    output.push_back(m_gridTopRight);

    return output;
}



//##### UTILITY #####

void ManualAlign::alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input)
{
    bool gridReset = false;

    for (const auto& it : input)
    {
        //adjust coordinates and minSize(s) of all elements of vector
        for (const auto& node : it.second)
        {
            //coordinates
            auto x = node->getRootCoordX();
            auto y = node->getRootCoordY();

            auto position = std::round(x / m_gridSize.first);
            x = position * m_gridSize.first;
            position = std::round(y / m_gridSize.second);
            y = position * m_gridSize.second;

            node->setRootCoordX(x);
            node->setRootCoordY(y);

            //check if coords expand current grid
            if(!gridReset)
            {
                m_gridBottomLeft.first = x;
                m_gridBottomLeft.second = y;
                m_gridTopRight.first = x;
                m_gridTopRight. second = y;

                gridReset = true;
            }
            else
            {
                if(x < m_gridBottomLeft.first)
                {
                    m_gridBottomLeft.first = x;
                }
                else if(x > m_gridTopRight.first)
                {
                    m_gridTopRight.first = x;
                }

                if(y < m_gridBottomLeft.second)
                {
                    m_gridBottomLeft.second = y;
                }
                else if(y > m_gridTopRight.second)
                {
                    m_gridBottomLeft.second = y;
                }
            }
        }
    }
}

void ManualAlign::alignIntermediateCornersToGrid(std::vector<std::shared_ptr<Connection>>& input)
{
    for (const auto& con : input)
    {
        auto coords = con->getIntermediateCorners();

        for (auto& coord : coords)
        {
            auto gridCount = std::round(coord.first / m_gridSize.first);
            coord.first = gridCount * m_gridSize.first;
            gridCount = std::round(coord.second / m_gridSize.second);
            coord.second = gridCount * m_gridSize.second;
        }

        con->setIntermediateCorners(coords);
    }
}