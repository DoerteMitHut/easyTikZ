#ifndef TIKZ_GENERATOR_H
#define TIKZ_GENERATOR_H

#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

class TikzGenerator
{
	private:
		string stringDigital;

		string drawPolygon(int verts, float size, float rootCoordX, float rootCoordY);
		int printEasyTikZ(string stringToPrint, string pathOutput);
	public:
		int generateEasyTikZ(string diagramInput, string pathOutput);
		void debugMethod();
};
#endif