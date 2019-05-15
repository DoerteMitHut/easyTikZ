#pragma once

#include "Diagram.h"

#include <string>


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

        std::string drawConnection(std::string identifierOrigin, std::string identifierTarget, bool directional);
};

