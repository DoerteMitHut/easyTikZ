#include "Diagram.h"



//##### INSERTION #####

void Diagram::insertNode(Shape &inShape)
{
    m_nodes[typeid(inShape)].push_back(std::unique_ptr<Shape>(&inShape));
}
void Diagram::insertConnection(Connection &inConn)
{
    m_connections.push_back(inConn);
}



//##### GETTERS #####

Shape& Diagram::getShape()
{
    throw std::logic_error("not implemented");
}
std::unordered_map<std::type_index,std::vector<std::unique_ptr<Shape>>> getShapes()
{
    throw std::logic_error("not implemented");
}
std::vector<std::unique_ptr<Shape>>& Diagram::getShapes(std::type_index shapeType)
{
    return m_nodes[shapeType];
}
std::vector<Connection>& Diagram::getConnections()
{
    return m_connections;
}