#pragma once
#include "ImageSimple.h"
#include "MatrixSimple.h"
#include <math.h>
#include <tuple>
#include <vector>
#include <cmath>

struct dims_2D { int x; int y; };

std::vector<double> PartialDerivative(std::vector<double>* phi, MatrixSimple* refImage, MatrixSimple* templateImage, dims_2D controlPointsDim);
std::tuple<int, int> to2DIndex(int d, dims_2D dims);
std::tuple<double, double> Txy(int x, int y, const std::vector<double>* const phi, dims_2D controlPointsDims, dims_2D nControlPoints);

double spline_basis(int i, double u);
std::vector<double> init_grid(dims_2D d_ctrl, dims_2D n_img);
dims_2D calculateNumberOfCtrlPts(dims_2D d_ctrl, dims_2D n_img);
std::tuple<MatrixSimple, MatrixSimple>get_displacement(const std::vector<double>* const phi, dims_2D d_ctrl, dims_2D n_ctrl, dims_2D n_img);
void warp(MatrixSimple* target, MatrixSimple const* tmpl, MatrixSimple const* disp_x, MatrixSimple const* disp_y, dims_2D n_img);
double get_bilinear(MatrixSimple const* tmpl, double x, double y);
