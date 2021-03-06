#include "Diagram.h"



//##### UTILITY #####

std::vector<std::pair<float, float>> Diagram::alignDiagram(AlignmentOption* option, float gridSizeX, float gridSizeY)
{
    auto& map = this->getMap();
    auto& connections = this->getConnections();

    return(option->align(map, connections, gridSizeX, gridSizeY));
}



//##### INSERTION #####

void Diagram::insertNode(std::shared_ptr<Rectangle> inRect)
{
    m_nodes[typeid(inRect)].push_back(inRect);
}
void Diagram::insertNode(std::shared_ptr<Circle> inCirc)
{
    m_nodes[typeid(inCirc)].push_back(inCirc);
}
void Diagram::insertNode(std::shared_ptr<Polygon> inPoly)
{
    m_nodes[typeid(inPoly)].push_back(inPoly);
}
void Diagram::insertConnection(std::shared_ptr<Connection> inConn)
{
    m_connections.push_back(inConn);
}
void Diagram::insertMap(std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> inMap)
{
    m_nodes.clear();
    m_nodes = inMap;
}



//##### GETTERS #####

std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>>& Diagram::getMap()
{
    return m_nodes;
}
std::vector<std::shared_ptr<Shape>>& Diagram::getShapes(std::type_index shapeType)
{
    return m_nodes[shapeType];
}
std::vector<std::shared_ptr<Connection>>& Diagram::getConnections()
{
    return m_connections;
}
