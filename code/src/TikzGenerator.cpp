#include "TikzGenerator.h"



//##### PUBLIC #####

//generates an EasyTikZ.txt based on diagramInput TODO: at a specified path
int TikzGenerator::generateEasyTikZ(Diagram diagramInput/*, string pathOutput*/)
{
    unpackDiagram(diagramInput);

    printEasyTikZ(m_stringDigital);

    return 0;
}



//##### UTILITY #####

//generic toString Template
template <typename T> std::string toStringBoi(const T& input)
{
    ostringstream digitalStreamString;
    digitalStreamString << input;

    return digitalStreamString.str();
}

//unpacks Diagram and calls appropriate methods
int TikzGenerator::unpackDiagram(Diagram diagramInput)
{
    //for each Rectangle append the corresponding result of drawRectangle to m_stringDigital
    for(std::unique_ptr<Shape> &rectangle : diagramInput.getShapes(typeid(Rectangle)))
    {
        std::unique_ptr<Rectangle> &currentRec = (std::unique_ptr<Rectangle>&)rectangle;
        m_stringDigital.append(drawRectangle(currentRec->getMinWidth, currentRec->getMinHeight, currentRec->getIdentifier, currentRec->getRootCoordX, currentRec->getRootCoordY));
    }
    m_stringDigital.append("\n");
    //for each Connection append the corresponding result of drawConnection to m_stringDigital
    for(Connection &connection : diagramInput.getConnections())
    {
        m_stringDigital.append(drawConnection(connection.getIdentifierOrigin, connection.getIdentifierTarget, connection.getDirectional));
    }

    return 0;
}

//writes a given string to a file named EasyTikZ.txt TODO: at a specified path
int TikzGenerator::printEasyTikZ(std::string stringToPrint/*, std::string pathOutput*/)
{
    std::ofstream fileStreamOutput("EasyTikZ.txt");
    fileStreamOutput << stringToPrint;
    fileStreamOutput.close();

    return 0;
}



//##### RECTANGLES #####

//returns TikZ code for a rectangle as string
std::string TikzGenerator::drawRectangle(float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY)
{
    //QUESTION: bad style?
    return("\\node[draw, rectangle, minimum width = "+ toStringBoi(minWidth) + "cm, minimum height = " + toStringBoi(minHeight) + "cm] (" + identifier + ") at (" + toStringBoi(rootCoordX) + "," + toStringBoi(rootCoordY) + ")\n");
}

//returns TikZ code for a filled rectangle as string //Edit: I now realize this probably is pretty useless
std::string TikzGenerator::drawRectangle(std::string fill, float minWidth, float minHeight, std::string identifier, float rootCoordX, float rootCoordY)
{
    //QUESTION: bad style?
    return("\\node[draw, fill=" + fill + ", rectangle, minimum width = "+ toStringBoi(minWidth) + "cm, minimum height = " + toStringBoi(minHeight) + "cm] (" + identifier + ") at (" + toStringBoi(rootCoordX) + "," + toStringBoi(rootCoordY) + ")\n");
}



//##### CONNECTIONS #####

//returns TikZ code for a connection between two nodes
std::string TikzGenerator::drawConnection(std::string identifierOrigin, std::string identifierTarget, bool directional)
{
    if(directional)
    {
        return("\\draw[->,auto] (" + identifierOrigin + ") -- (" + identifierTarget + ");\n");//add default arrow style et cetera when cosmetic variables are available
    }
    else
    {
        return("\\draw[auto] (" + identifierOrigin + ") -- (" + identifierTarget + ");\n");
    }
}