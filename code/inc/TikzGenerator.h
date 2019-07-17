#pragma once

#include "Diagram.h"

#include <string>


class TikzGenerator
{
    public:
        int generateEasyTikZ(Diagram diagramInput, AlignmentOption* alignmentOptionInput, bool tikzEnv, bool texEnv, float gridSizeX = 1.18, float gridSizeY = 1.18/*, std::string pathOutput*/);

     private:
        std::string m_stringDigital;

        int unpackDiagram(Diagram diagramInput);
        int printEasyTikZ(std::string stringToPrint/*, std::string pathOutput*/);

        std::string drawRectangle(std::shared_ptr<Rectangle>&);
        std::string drawCircle(std::shared_ptr<Circle>&);
        std::string drawPolygon(std::shared_ptr<Polygon>&);
        std::string drawConnection(std::shared_ptr<Connection>&);

        void cosmeticOptions();

        void texEnvHead();
        void tikzEnvHead();
        void texEnvFoot();
        void tikzEnvFoot();
};

