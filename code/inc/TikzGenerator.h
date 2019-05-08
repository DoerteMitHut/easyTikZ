#ifndef TIKZ_GENERATOR_H
#define TIKZ_GENERATOR_H

#include "Diagram.h"

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class TikzGenerator
{
    public:
        int generateEasyTikZ(Diagram diagramInput/*, std::string pathOutput*/);

     private:
        std::string m_stringDigital;

        int unpackDiagram(Diagram diagramInput);
        int printEasyTikZ(std::string stringToPrint/*, std::string pathOutput*/);

        std::string drawRectangle(float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY);
        std::string drawRectangle(std::string fill, float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY);
};
#endif
