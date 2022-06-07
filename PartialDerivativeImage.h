#pragma once
#include "ImageSimple.h"
#include <math.h>
#include <tuple>
#include <vector>

class PartialDerivativeImage
{
public:
	std::vector<unsigned char> PartialDerivative(ImageSimple* phi, ImageSimple* refImage, ImageSimple* templateImage, int controlPointsDim[]);
private:
	std::tuple<int, int> to2DIndex(int d, int dims[]);
	std::tuple<int, int> Txy(int x, int y, const ImageSimple* const phi, int controlPointsDims[], int controlPointsNums[]);
};

