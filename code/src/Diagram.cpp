#include "Diagram.h"
#include <string>

using namespace std;

void Diagram::insertNode(Shape &inShape)
{
    m_nodes[typeid(inShape)].push_back(std::unique_ptr<Shape>(&inShape));
}

Shape& Diagram::getShape()
{
    throw std::logic_error("not implemented");
}

std::vector<std::unique_ptr<Shape>>& Diagram::getShapes(std::type_index shapeType)
{
    return m_nodes[shapeType];
}

void Diagram::insertConnection(Connection &inConn)
{
    m_connections.push_back(inConn);
}

std::vector<Connection>& Diagram::getConnections()
{
    return m_connections;
}

// Rectangle

