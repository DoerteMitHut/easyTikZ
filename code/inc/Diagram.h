#pragma once

#include "Shapes.h"
#include "Connection.h"
#include "AlignmentOptionCollection.h"

#include <memory>
#include <string>
#include <vector>
#include <typeinfo>
#include <typeindex>
#include <unordered_map>



class Diagram
{
public:
    Diagram(/* args */) = default;
    ~Diagram() = default;

    void alignDiagram(AlignmentOption*);//AlignmentOption* results in linker error??????

    void insertRectangle(std::shared_ptr<Rectangle>);
    void insertConnection(Connection&);
    void insertMap(std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>>);

    Shape& getShape();

    std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> getMap();
    std::vector<std::shared_ptr<Shape>>& getShapes(std::type_index);
    std::vector<Connection>& getConnections();

private:
    std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> m_nodes;
    std::vector<Connection> m_connections;
    double m_gridSize;
};

