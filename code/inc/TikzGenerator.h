#pragma once

#include "Diagram.h"

#include <string>


class TikzGenerator
{
    public:
        int generateEasyTikZ(Diagram diagramInput, DefaultAlign alignmentOptionInput/*, std::string pathOutput*/);

     private:
        std::string m_stringDigital;

        int unpackDiagram(Diagram diagramInput);
        int printEasyTikZ(std::string stringToPrint/*, std::string pathOutput*/);

        std::string drawRectangle(std::shared_ptr<Rectangle>&);

        std::string drawConnection(std::string identifierOrigin, std::string identifierTarget, bool directional);
};

