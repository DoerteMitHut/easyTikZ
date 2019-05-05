#ifndef TIKZ_GENERATOR_H
#define TIKZ_GENERATOR_H

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

class TikzGenerator
{
	private:
		std::string stringDigital;

		std::string drawPolygon(int verts, float size, float rootCoordX, float rootCoordY);
		int printEasyTikZ(std::string stringToPrint, std::string pathOutput);
	public:
		int generateEasyTikZ(std::string diagramInput, std::string pathOutput);
		void debugMethod();
};
#endif
