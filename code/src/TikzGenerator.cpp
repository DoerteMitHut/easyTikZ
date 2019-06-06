#include "TikzGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>



//##### PUBLIC #####

//generates an EasyTikZ.txt based on diagramInput, alignmentOption TODO: at a specified path && AlignmentOption* results in linker error??????
int TikzGenerator::generateEasyTikZ(Diagram diagramInput, AlignmentOption* alignmentOptionInput/*, string pathOutput*/)
{
	diagramInput.alignDiagram(alignmentOptionInput);

    unpackDiagram(diagramInput);

    printEasyTikZ(m_stringDigital);

    return 0;
}



//##### UTILITY #####

//generic toString Template
template <typename T>
std::string toStringBoi(const T& input)
{
    std::ostringstream digitalStreamString;
    digitalStreamString << input;

    return digitalStreamString.str();
}

//unpacks Diagram and calls appropriate methods
int TikzGenerator::unpackDiagram(Diagram diagramInput)
{
    //for each Rectangle append the corresponding result of drawRectangle to m_stringDigital
    for(const auto& rectangle : diagramInput.getShapes(typeid(std::shared_ptr<Rectangle>)))
    {
        auto& currentRec = (std::shared_ptr<Rectangle>&)rectangle;
        m_stringDigital.append(drawRectangle(currentRec));
    }
    m_stringDigital.append("\n");
    //for each Connection append the corresponding result of drawConnection to m_stringDigital
    for(const auto& connection : diagramInput.getConnections())
    {
        // TODO pass the connection itself as parameter
        m_stringDigital.append(drawConnection(connection.getIdentifierOrigin(), connection.getIdentifierTarget(), connection.getDirectional()));
    }

    return 0;
}

//writes a given string to a file named EasyTikZ.txt TODO: at a specified path
int TikzGenerator::printEasyTikZ(std::string stringToPrint/*, std::string pathOutput*/)
{
    std::ofstream fileStreamOutput("EasyTikZ.txt");
    fileStreamOutput << stringToPrint;

    return 0;
}



//##### RECTANGLES #####

//returns TikZ code for a rectangle as string
std::string TikzGenerator::drawRectangle(std::shared_ptr<Rectangle>& rect)
{
    const float minWidth = rect->getMinWidth();
    const float minHeight = rect->getMinHeight();
    const std::string identifier = rect->getIdentifier();
    const float rootCoordX = rect->getRootCoordX();
    const float rootCoordY = rect->getRootCoordY();
    const std::string label = rect->getLabel();

    std::ostringstream methodOutput;
    methodOutput << "\\node[draw";
    methodOutput << ", rectangle, minimum width = " << toStringBoi(minWidth) << "cm, ";
    methodOutput << "minimum height = " << toStringBoi(minHeight) << "cm] ";
    methodOutput << "(" << identifier << ") ";
    methodOutput << "at (" << toStringBoi(rootCoordX) << "," << toStringBoi(rootCoordY) << ")";
    methodOutput << " {" << label << "};\n";
    return methodOutput.str();
}



//##### CONNECTIONS #####

//returns TikZ code for a connection between two nodes
std::string TikzGenerator::drawConnection(std::string identifierOrigin, std::string identifierTarget, bool directional)
{
    if(directional)
    {
        std::ostringstream methodOutput;
        //add default arrow style et cetera when cosmetic variables are available
        methodOutput << "\\draw[->,auto] (" << identifierOrigin << ") -- (" << identifierTarget << ");\n";
        return methodOutput.str();
    }
    else
    {
        std::ostringstream methodOutput;
        methodOutput << "\\draw[auto] (" << identifierOrigin << ") -- (" << identifierTarget << ");\n";
        return methodOutput.str();
    }
}
