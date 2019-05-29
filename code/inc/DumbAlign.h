#pragma once

#include "Diagram.h"
#include "AlignmentOption.h"

#include <vector>



class DumbAlign : AlignmentOption
{
    public:
    int align(Diagram*);

    private:
    const void readDiagram(Diagram*);
    void sortMap(std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>>);
    void replaceDiagramMap(Diagram*);

    std::unordered_map<std::type_index,std::vector<std::shared_ptr<Shape>>> m_map;
};