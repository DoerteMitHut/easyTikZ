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
    for(auto& rectangle : input->getShapes(typeid(std::shared_ptr<Rectangle>)))
    {
        
    }
}

void DumbAlign::sortMap(std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> input)
{

}

void DumbAlign::replaceDiagramMap(Diagram* input)
{
    std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> diaMap = input->getMap();
    m_map.swap(diaMap); //is this a copy or the actual map of my Diagram? Better use a setter for the map?
}