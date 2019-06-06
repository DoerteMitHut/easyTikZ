#include "DefaultAlign.h"



//##### PUBLIC #####

std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> DefaultAlign::alignMap(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> input)
{
	automaticGridSize(input[typeid(std::shared_ptr<Rectangle>)]);

	//alignNodesToGrid(input);

	return input;
}



//##### UTILITY #####

void DefaultAlign::automaticGridSize(std::vector<std::shared_ptr<Shape>> input) //currently only works for rectangles
{
	for(const auto& rectangle : input)
    {
        auto& currentRec = (std::shared_ptr<Rectangle>&)rectangle;

		if ((currentRec->getMinWidth()) < (m_gridSizeX*2))
		{
			m_gridSizeX = (currentRec->getMinWidth() / 2);
		}
		if ((currentRec->getMinHeight()) < (m_gridSizeY * 2))
		{
			m_gridSizeY = (currentRec->getMinHeight() / 2);
		}
    }
}

//struggling because of nodes being in vectors in unordered_map, just want them all in succession
void DefaultAlign::alignNodesToGrid(std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>> input)
{
	//iterator pointing to beginning of map 
	std::unordered_map<std::type_index, std::vector<std::shared_ptr<Shape>>>::iterator it = input.begin();

	//while loop for all elements; use "it->second" to access vector
	while (it != input.end())
	{
		//extract coordinates of all elements of vector
		for(std::shared_ptr<Shape> node : it->second)
		{
			m_coordinateMap.insert({node->getRootCoordX(),node->getRootCoordY()});
		}
	}

	//TODO: sort m_coordinateMap appropriately based on m_gridSizeX & m_gridSizeY

	//while loop for all elements again, replacing coordinates in the same order 
	while (it != input.end())
	{
		//replace coordinates of Shape with associated coords in m_coordinateMap
		for (std::shared_ptr<Shape> node : it->second)
		{
			//probably gonna need a way of iteration for m_coordinateMap when inserting/extracting
			node->setRootCoordX(0.0); //placeholder
			node->setRootCoordY(0.0); //placeholder
		}
	}
}