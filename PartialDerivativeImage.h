#pragma once
#include "ImageSimple.h"
#include <math.h>
#include <tuple>

class PartialDerivativeImage
{
public:
	void PartialDerivative(ImageSimple* phi, ImageSimple* refImage, ImageSimple* templateImage, int controlPointsDim[]);
private:
	std::tuple<int, int> to2DIndex(int d, int dims[]);
};

