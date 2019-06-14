#pragma once

#include "Diagram.h"

#include <string>


class TikzGenerator
{
    public:
        int generateEasyTikZ(Diagram diagramInput, AlignmentOption* alignmentOptionInput, bool texEnv, bool tikzEnv/*, std::string pathOutput*/);

     private:
        std::string m_stringDigital;

        int unpackDiagram(Diagram diagramInput);
        int printEasyTikZ(std::string stringToPrint/*, std::string pathOutput*/);

        std::string drawRectangle(std::shared_ptr<Rectangle>&);
        std::string drawConnection(std::shared_ptr<Connection>&);

        void texEnvHead();
        void tikzEnvHead();
        void texEnvFoot();
        void tikzEnvFoot();
};

