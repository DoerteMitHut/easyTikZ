#include "DumbAlign.h"



int DumbAlign::align(Diagram* input)
{
    readDiagram(input);

    sortMap(m_map);

    replaceDiagramMap(input);

    return 0;
}

const void DumbAlign::readDiagram(Diagram* input)
{
    m_map = input->getMap();
}

void DumbAlign::sortMap(std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> input /* + gridDemension*/)
{
    for(auto& rectangle : input[typeid(std::shared_ptr<Rectangle>)])
    {
        //determine gridCellSize based on max minWidth/minHeight of Shapes (foreach check)
        //arrange Shapes based on rootCoordinates and given gridDimension
    }
}

void DumbAlign::replaceDiagramMap(Diagram* input)
{
    input->insertMap(m_map);
    //m_map.swap(input->getMap()); //is this a copy or the actual map of my Diagram? Better use a setter for the map?
}