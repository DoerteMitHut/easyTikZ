#include "SizeAlign.h"
#include <cmath>


//##### PUBLIC #####

std::vector<std::pair<float, float>> SizeAlign::align(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& inputMap, std::vector<std::shared_ptr<Connection>>& inputConnections, float gridSizeX, float gridSizeY)
{
    //determine m_gridSize(s) for current input
    automaticGridSize(inputMap, gridSizeX, gridSizeY);

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

void SizeAlign::automaticGridSize(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input, float gridSizeX, float gridSizeY)
{
    m_gridSize.first = 0.0;
    m_gridSize.second = 0.0;
    
    int counter = 0;

    for(const auto& rectangle : input[typeid(std::shared_ptr<Rectangle>)])
    {
        auto& currentRec = (std::shared_ptr<Rectangle>&)rectangle;

        m_gridSize.first += currentRec->getMinWidth();
        m_gridSize.second += currentRec->getMinHeight();

        counter++;
    }

    for(const auto& circle : input[typeid(std::shared_ptr<Circle>)])
    {
        auto& currentCirc = (std::shared_ptr<Circle>&)circle;

        m_gridSize.first += currentCirc->getMinSize();
        m_gridSize.second += currentCirc->getMinSize();

        counter++;
    }

    for(const auto& polygon : input[typeid(std::shared_ptr<Polygon>)])
    {
        auto& currentPoly = (std::shared_ptr<Circle>&)polygon;

        m_gridSize.first += currentPoly->getMinSize();
        m_gridSize.second += currentPoly->getMinSize();

        counter++;
    }
    
    if(counter == 0)
    {
        m_gridSize.first = gridSizeX;
        m_gridSize.second = gridSizeY;
    }
    else if(m_gridSize.first > m_gridSize.second)
    {
        m_gridSize.first /= counter;
        m_gridSize.second /= counter;

        auto count = std::round(m_gridSize.first / m_gridSize.second);
        m_gridSize.first = count * m_gridSize.second;
    }
    else if(m_gridSize.first < m_gridSize.second)
    {
        m_gridSize.first /= counter;
        m_gridSize.second /= counter;

        auto count = std::round(m_gridSize.second / m_gridSize.first);
        m_gridSize.second = count * m_gridSize.first;
    }
}

void SizeAlign::alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>& input)
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

            //size(s)
            if(it.first == typeid(std::shared_ptr<Rectangle>&))
            {
                auto& currentNode = (std::shared_ptr<Rectangle>&)node;
                auto width = currentNode->getMinWidth();
                auto height = currentNode->getMinHeight();

                auto gridCount = std::round(width/m_gridSize.first);
                width = gridCount * m_gridSize.first;
                gridCount = std::round(height/m_gridSize.second);
                height = gridCount * m_gridSize.second;

                currentNode->setMinWidth(width);
                currentNode->setMinHeight(height);
            }
            else if(it.first == typeid(std::shared_ptr<Circle>&))
            {
                auto& currentNode = (std::shared_ptr<Circle>&)node;
                auto size = currentNode->getMinSize();
                auto gridSizeSmaller = (m_gridSize.first <= m_gridSize.second ? m_gridSize.first : m_gridSize.second);

                auto gridCount = std::round(size/gridSizeSmaller);
                size = gridCount * gridSizeSmaller;

                currentNode->setMinSize(size);
            }
            else if(it.first == typeid(std::shared_ptr<Polygon>&))
            {
                auto& currentNode = (std::shared_ptr<Polygon>&)node;
                auto size = currentNode->getMinSize();
                auto gridSizeSmaller = (m_gridSize.first <= m_gridSize.second ? m_gridSize.first : m_gridSize.second);

                auto gridCount = std::round(size/gridSizeSmaller);
                size = gridCount * gridSizeSmaller;

                currentNode->setMinSize(size);
            }

            //check if coords expand current grid
            if(!gridReset)
            {
                m_gridBottomLeft.first = x;
                m_gridBottomLeft.second = y;
                m_gridTopRight.first = x;
                m_gridTopRight.second = y;

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
                    m_gridTopRight.second = y;
                }
            }
        }
    }
}

void SizeAlign::alignIntermediateCornersToGrid(std::vector<std::shared_ptr<Connection>>& input)
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