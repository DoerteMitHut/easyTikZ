#include "Diagram.h"


//##### INSERTION #####

//TODO Template this
void Diagram::insertRectangle(std::shared_ptr<Rectangle> inRect)
{
    m_nodes[typeid(inRect)].push_back(inRect);
}
void Diagram::insertConnection(Connection& inConn)
{
    m_connections.push_back(inConn);
}



//##### GETTERS #####

Shape& Diagram::getShape()
{
    throw std::logic_error("not implemented");
}
std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> getShapes()
{
    throw std::logic_error("not implemented");
}
std::vector<std::shared_ptr<Shape>>& Diagram::getShapes(std::type_index shapeType)
{
    return m_nodes[shapeType];
}
std::vector<Connection>& Diagram::getConnections()
{
    return m_connections;
}