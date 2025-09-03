#include <algorithm>
#include <iostream>
#include <memory>
#include <vector>


#include "circle.h"
#include "square.h"

#include "shape.h"


int main()
{
	using Shapes = std::vector<Shape>;

	Shapes shapes;
	shapes.emplace_back(Circle{ 2.0 });
	shapes.emplace_back(Square{ 1.5 });
	shapes.emplace_back(Circle{ 4.2 });

	std::cout << "\nDrawing all shapes:\n";
	std::for_each(shapes.begin(), shapes.end(), [](const Shape& shape)
				  {
					  draw(shape);
				  });

	std::cout << "\nSerializing all shapes:\n";
	std::for_each(shapes.begin(), shapes.end(), [](const Shape& shape)
				  {
					  serialize(shape);
				  });
}
