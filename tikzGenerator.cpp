#include "tikzGenerator.h"

using namespace std;

//Generic toString Template
template <typename T> string toStringBoi(const T& input)
{
	ostringstream digitalStreamString;
	digitalStreamString << input;
	return digitalStreamString.str();
}

//generates an EasyTikZ.txt at pathOutput location, based on diagramInput
int TikzGenerator::generateEasyTikZ(/*Diagram*/string diagramInput, string pathOutput)
{
	//unpackDiagram(diagramInput);

	//printEasyTikZ(

	return 0;
}

//int unpackDiagram(Diagram diagramInput){each shape -> drawShape(shape); / drawConnections(data); / drawText(data); return 0;} /*also unpacks shape specific information and converts them appropriately for drawShape*/

//returns TikZ code for a polygon as string (currently supports 4-4 verts)
string TikzGenerator::drawPolygon(int verts, float size, float rootCoordX, float rootCoordY) /*add parameter for connection hardpoints - array?*/
{
	cout << "DEBUG: Drawing polygon with " << verts << " verts in size " << size << " with the root coordinate " << rootCoordX << "|" << rootCoordY << endl;
	string stringMethodOutput;

	if (verts < 3)
	{
		cerr << "ERROR: A polygon has to have at least 3 verts" << endl;
		return "";	
	}
	else if (verts == 4)
	{
		stringMethodOutput.append("\\draw ");
		stringMethodOutput.append("(" + toStringBoi(rootCoordX) + "," + toStringBoi(rootCoordY) + ") ");
		stringMethodOutput.append("--(" + toStringBoi(rootCoordX + size) + "," + toStringBoi(rootCoordY) + ") ");
		stringMethodOutput.append("--(" + toStringBoi(rootCoordX + size) + "," + toStringBoi(rootCoordY + size) + ") ");
		stringMethodOutput.append("--(" + toStringBoi(rootCoordX) + "," + toStringBoi(rootCoordY + size) + ") ");
		stringMethodOutput.append("--cycle;\n");
	}
	else if (verts > 4)
	{
		cerr << "ERROR: Polygons with " << verts << " verts have yet to be implemented";
		return "";
	}

	return stringMethodOutput;
}

//writes a given string to a file named EasyTikZ.txt at a specified path
int TikzGenerator::printEasyTikZ(string stringToPrint, string pathOutput)//yet to add adjustable path of output
{
	ofstream fileStreamOutput("EasyTikZ.txt");
	fileStreamOutput << stringToPrint;
	fileStreamOutput.close();

	return 0;
}

//Only here for testing. Did you know that most toilets flush in E flat?
void TikzGenerator::debugMethod()
{
	stringDigital.append(drawPolygon(4, 2, 0, 0));
	stringDigital.append(drawPolygon(4, 2, 2, 2));
	printEasyTikZ(stringDigital, "");
}

//Super useful main
int main(int argc, char** argv)
{
	TikzGenerator gen;

	gen.debugMethod();

	return 0;
}