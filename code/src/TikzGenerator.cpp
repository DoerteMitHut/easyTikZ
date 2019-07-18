#include "TikzGenerator.h"

#include <iostream>
#include <fstream>
#include <sstream>



//##### PUBLIC #####

//generates an EasyTikZ.txt based on diagramInput, alignmentOption and flags
int TikzGenerator::generateEasyTikZ(Diagram diagramInput, AlignmentOption* alignmentOptionInput,bool tikzEnv, bool texEnv, bool cosVar, float gridSizeX, float gridSizeY)
{
    m_stringDigital.clear();
    m_cosmeticGrid.clear();
    m_cosVarEnabled = cosVar;

    //alignDiagram and get resulting info on grid
    m_cosmeticGrid = diagramInput.alignDiagram(alignmentOptionInput, gridSizeX, gridSizeY);

    //append output to m_stringDigital
    if(texEnv)texEnvHead();
    if(tikzEnv)tikzEnvHead();
    if(cosVar)cosmeticOptions();
    unpackDiagram(diagramInput);
    if(tikzEnv)tikzEnvFoot();
    if(texEnv)texEnvFoot();

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
    //for each Circle append the corresponding result of drawCircle to m_stringDigital
    for(const auto& circle : diagramInput.getShapes(typeid(std::shared_ptr<Circle>)))
    {
        auto& currentCirc = (std::shared_ptr<Circle>&)circle;
        m_stringDigital.append(drawCircle(currentCirc));
    }
    //for each Polygon append the corresponding result of drawPolygon to m_stringDigital
    for(const auto& polygon : diagramInput.getShapes(typeid(std::shared_ptr<Polygon>)))
    {
        auto& currentPoly = (std::shared_ptr<Polygon>&)polygon;
        m_stringDigital.append(drawPolygon(currentPoly));
    }
    //for each Connection append the corresponding result of drawConnection to m_stringDigital
    for(const auto& connection : diagramInput.getConnections())
    {
        auto& currentConn = (std::shared_ptr<Connection>&)connection;
        m_stringDigital.append(drawConnection(currentConn));
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



//##### SHAPES #####

//returns TikZ code for a rectangle as string
std::string TikzGenerator::drawRectangle(std::shared_ptr<Rectangle>& rect)
{
    const float minWidth = rect->getMinWidth();
    const float minHeight = rect->getMinHeight();
    const bool rotated = rect->getRotated();
    const std::string identifier = rect->getIdentifier();
    const float rootCoordX = rect->getRootCoordX();
    const float rootCoordY = rect->getRootCoordY();
    const std::string label = rect->getLabel();

    std::ostringstream methodOutput;
    methodOutput << "\\node[draw, ";
    if(!rotated)
    {
        methodOutput << "rectangle, ";
        if(m_cosVarEnabled) methodOutput << "style = easytikzRectangle, ";
        methodOutput << "minimum width = " << toStringBoi(minWidth) << "cm, ";
        methodOutput << "minimum height = " << toStringBoi(minHeight) << "cm] ";
    }
    else
    {
        methodOutput << "diamond, ";
        if(m_cosVarEnabled) methodOutput << "style = easytikzDiamond, ";
        methodOutput << "minimum width = " << toStringBoi(minWidth) << "cm, ";
        methodOutput << "minimum height = " << toStringBoi(minHeight) << "cm] ";
    }
    methodOutput << "(" << identifier << ") ";
    methodOutput << "at (" << toStringBoi(rootCoordX) << "," << toStringBoi(rootCoordY) << ") ";
    methodOutput << "{" << label << "};\n";
    return methodOutput.str();
}

//returns TikZ code for a circle as string
std::string TikzGenerator::drawCircle(std::shared_ptr<Circle>& circ)
{
    const float minSize = circ->getMinSize();
    const std::string identifier = circ->getIdentifier();
    const float rootCoordX = circ->getRootCoordX();
    const float rootCoordY = circ->getRootCoordY();
    const std::string label = circ->getLabel();

    std::ostringstream methodOutput;
    methodOutput << "\\node[draw, ";
    methodOutput << "circle, ";
    if(m_cosVarEnabled) methodOutput << "style = easytikzCircle, ";
    methodOutput << "minimum size = " << toStringBoi(minSize) << "cm] ";
    methodOutput << "(" << identifier << ") ";
    methodOutput << "at (" << toStringBoi(rootCoordX) << "," << toStringBoi(rootCoordY) << ") ";
    methodOutput << "{" << label << "};\n";
    return methodOutput.str();
}

//returns TikZ code for a polygon as string
std::string TikzGenerator::drawPolygon(std::shared_ptr<Polygon>& poly)
{
    const float minSize = poly->getMinSize();
    const int polySides = poly->getPolySides();
    const std::string identifier = poly->getIdentifier();
    const float rootCoordX = poly->getRootCoordX();
    const float rootCoordY = poly->getRootCoordY();
    const std::string label = poly->getLabel();

    std::ostringstream methodOutput;
    methodOutput << "\\node[draw, ";
    methodOutput << "regular polygon, ";
    if(m_cosVarEnabled) methodOutput << "style = easytikzPolygon, ";
    methodOutput << "minimum size = " << toStringBoi(minSize) << "cm, regular polygon sides = " << toStringBoi(polySides) << "] ";
    methodOutput << "(" << identifier << ") ";
    methodOutput << "at (" << toStringBoi(rootCoordX) << "," << toStringBoi(rootCoordY) << ") ";
    methodOutput << "{" << label << "};\n";
    return methodOutput.str();
}



//##### CONNECTIONS #####

//returns TikZ code for a connection between two nodes
std::string TikzGenerator::drawConnection(std::shared_ptr<Connection>& conn)
{
    const std::string identifierOrigin = conn->getIdentifierOrigin();
    const std::string identifierTarget = conn->getIdentifierTarget();
    const std::vector<std::pair<float,float>> intermediateCorners = conn->getIntermediateCorners();
    const bool directional = conn->getDirectional();

    std::ostringstream methodOutput;
    //add default arrow style et cetera when cosmetic variables are available
    methodOutput << "\\draw[";
    if(directional) methodOutput << "->,";
    if(m_cosVarEnabled) methodOutput << "style = easytikzConnection, ";
    methodOutput << "auto] (" << identifierOrigin << ") -- ";
    for(std::pair<float,float> iCoords : intermediateCorners)
    {
        methodOutput << "(" << iCoords.first << "," << iCoords.second << ") -- ";
    }
    methodOutput << "(" << identifierTarget << ");\n";
    
    return methodOutput.str();
}



//##### ENVIRONMENTS #####

//cosmetic variables
void TikzGenerator::cosmeticOptions()
{
    m_stringDigital.append("\\tikzstyle{easytikzRectangle} = [black, line width = 1.0, solid, text = black]\n");
    m_stringDigital.append("\\tikzstyle{easytikzDiamond} = [black, line width = 1.0, solid, text = black]\n");
    m_stringDigital.append("\\tikzstyle{easytikzPolygon} = [black, line width = 1.0, solid, text = black]\n");
    m_stringDigital.append("\\tikzstyle{easytikzCircle} = [black, line width = 1.0, solid, text = black]\n");
    m_stringDigital.append("\\tikzstyle{easytikzConnection} = [black, line width = 1.0, solid, text = black]\n");
    
    m_stringDigital.append("\\tikzstyle{easytikzGrid} = [gray, line width = 0.5, dashed]\n");
    std::ostringstream gridString;
    gridString << "%\\draw[style = easytikzGrid, xstep = " << toStringBoi(m_cosmeticGrid.at(0).first) << ", ";
    gridString << "ystep = " << toStringBoi(m_cosmeticGrid.at(0).second) << "] ";
    gridString << "(" << toStringBoi(m_cosmeticGrid.at(1).first) << "," << toStringBoi(m_cosmeticGrid.at(1).second) << ") ";
    gridString << "grid (" << toStringBoi(m_cosmeticGrid.at(2).first) << "," << toStringBoi(m_cosmeticGrid.at(2).second) << ");\n\n";
    m_stringDigital.append(gridString.str());
}

//tex environment generation
void TikzGenerator::texEnvHead()
{
    m_stringDigital.append("\\documentclass{article}\n\\usepackage{tikz}\n\\usetikzlibrary{arrows}\n\\usetikzlibrary{shapes.geometric}\n\n\\begin{document}\n\n");
}
void TikzGenerator::texEnvFoot()
{
    m_stringDigital.append("\n\\end{document}");
}

//tikz environment generation
void TikzGenerator::tikzEnvHead()
{
    m_stringDigital.append("%##### BEGIN TIKZ #####\n\\begin{tikzpicture}\n\n");
}
void TikzGenerator::tikzEnvFoot()
{
    m_stringDigital.append("\n\\end{tikzpicture}\n%##### ");
    m_stringDigital.append("END TIKZ #####\n");
}