#pragma once

#include "Diagram.h"



class AlignmentOption
{
public:
    virtual int align(Diagram*) = 0;

protected:
    virtual const void readDiagram(Diagram*) = 0;
    virtual void replaceDiagramMap(Diagram*) = 0;
};
