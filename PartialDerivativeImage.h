#pragma once
#include "ImageSimple.h"
#include <math.h>
#include <tuple>
#include <vector>

#include <cmath>

std::vector<unsigned char> PartialDerivative(ImageSimple* phi, ImageSimple* refImage, ImageSimple* templateImage, int controlPointsDim[]);
std::tuple<int, int> to2DIndex(int d, int dims[]);
std::tuple<int, int> Txy(int x, int y, const ImageSimple* const phi, int controlPointsDims[], int nControlPoints[]);

double spline_basis(int i, double u);
typedef struct dims_2D { int x; int y; };
ImageSimple init_grid(dims_2D d_ctrl, dims_2D n_img);