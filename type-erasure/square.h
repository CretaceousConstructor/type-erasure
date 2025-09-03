#pragma once
#include <iostream>

class Square
{
	double width_;

public:
	explicit Square(double width) : width_(width)
	{
	}

	double width() const
	{
		return width_;
	}
};

std::ostream& operator<<(std::ostream& os, const Square& square);
void serialize(const Square& square);
void draw(const Square& square);

