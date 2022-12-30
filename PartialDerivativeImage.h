#pragma once
#include "ImageSimple.h"
#include <math.h>
#include <tuple>
#include <vector>
#include <cmath>

struct dims_2D { int x; int y; };

std::vector<float> PartialDerivative(std::vector<float>* phi, ImageSimple* refImage, ImageSimple* templateImage, dims_2D controlPointsDim);
std::tuple<int, int> to2DIndex(int d, dims_2D dims);
std::tuple<int, int> Txy(int x, int y, const std::vector<float>* const phi, dims_2D controlPointsDims, dims_2D nControlPoints);

double spline_basis(int i, double u);
std::vector<float> init_grid(dims_2D d_ctrl, dims_2D n_img);
dims_2D calculateNumberOfCtrlPts(dims_2D d_ctrl, dims_2D n_img);
std::tuple<ImageSimple, ImageSimple>get_displacement(const std::vector<float>* const phi, dims_2D d_ctrl, dims_2D n_ctrl, dims_2D n_img);
void warp(ImageSimple& target, ImageSimple const* tmpl, ImageSimple const* disp_x, ImageSimple const* disp_y, dims_2D n_img);
stbi_uc get_bilinear(ImageSimple const* tmpl, stbi_uc x, stbi_uc y);
