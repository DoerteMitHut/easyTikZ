#include "Diagram.h"



//##### UTILITY #####
void Diagram::alignDiagram(DumbAlign option)
{
    option.align(this); //shared_pointer? Just noticed this doesn't work properly.
}



//##### INSERTION #####

void Diagram::insertRectangle(std::shared_ptr<Rectangle> inRect)
{
    m_nodes[typeid(inRect)].push_back(inRect);
}
void Diagram::insertConnection(Connection& inConn)
{
    m_connections.push_back(inConn);
}
void Diagram::insertMap(std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> inMap)
{
    m_nodes.clear();
    m_nodes = inMap;
}



//##### GETTERS #####

Shape& Diagram::getShape()
{
    throw std::logic_error("not implemented");
}
std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> Diagram::getMap()
{
    return m_nodes;
}
std::vector<std::shared_ptr<Shape>>& Diagram::getShapes(std::type_index shapeType)
{
    return m_nodes[shapeType];
}
std::vector<Connection>& Diagram::getConnections()
{
    return m_connections;
}