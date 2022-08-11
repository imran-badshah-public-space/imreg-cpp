#include "PartialDerivativeImage.h"
#include <math.h>
#include <cmath>

std::vector<unsigned char> PartialDerivative(ImageSimple* phi, ImageSimple* refImage, ImageSimple* templateImage, int controlPointsDim[])
{
	int imgWidth = refImage->getWidth();
	int imgHeight = refImage->getHeight();

	int nCtrlY = (ceil(imgHeight / controlPointsDim[0])) + 3; // +3 for cubic bsplines
	int nCtrlX = (ceil(imgWidth / controlPointsDim[1])) + 3;
	int nControlPoints[2] = { nCtrlX, nCtrlY };

	int D = nCtrlX * nCtrlY;
	ImageSimple diff = *templateImage - refImage;
	std::vector<unsigned char> gradientE(D);

	ImageSimple* forceX = nullptr, * forceY = nullptr, * gradX = templateImage, * gradY = templateImage;
	templateImage->gradient(gradX, gradY);
	diff.dot(forceX, gradX);
	diff.dot(forceY, gradY);

	for (int d = 0; d < D; d++)
	{
		auto [x, y] = to2DIndex(d, controlPointsDim);
		int minX = std::max((x - 4) * controlPointsDim[0] + 1, 1);
		int minY = std::max((y - 4) * controlPointsDim[1] + 1, 1);
		int maxX = std::max((x)*controlPointsDim[0], imgWidth);
		int maxY = std::max((y)*controlPointsDim[1], imgHeight);

		uint8_t pdX = 0, pdY = 0;
		ImageSimple phishifted = ImageSimple(*phi);
		phishifted.setPixelValueAt(phishifted.getPixelValueAt(d) + 1, d);
		phishifted.setPixelValueAt(phishifted.getPixelValueAt(d + D) + 1, d + D);

		for (int lx = minX; lx < maxX; lx++)
		{
			for (int ly = minY; ly < maxY; ly++)
			{
				auto [res_x1, res_y1] = Txy(lx, ly, phi, controlPointsDim, nControlPoints);
				auto [res_x2, res_y2] = Txy(lx, ly, &phishifted, controlPointsDim, nControlPoints);
				int res_x = res_x2 - res_x1;
				int res_y = res_y2 - res_y1;
				pdX += res_x * forceX->getPixelValueAt(lx, ly, 0);
				pdY += res_y * forceY->getPixelValueAt(lx, ly, 0);
			}
		}

		double m_pd = sqrt(pow(pdX, 2) + pow(pdY, 2));
		if (m_pd != 0)
		{
			pdX = pdX / m_pd;
			pdY = pdY / m_pd;
		}
		gradientE[d] = pdX;
		gradientE[D + d] = pdY;
	}

	return gradientE;
}


std::tuple<int, int> to2DIndex(int d, int dims[])
{
	int x = ceil(d / dims[1]);
	int y = d % dims[0];
	if (y == 0)
	{
		y = dims[0];
	}
	return std::make_tuple(x, y);
}

std::tuple<int, int> Txy(int x, int y, const ImageSimple* const phi, int controlPointsDims[], int nControlPoints[])
{
	auto a = std::floor(2.0);
	auto i_temp = x / controlPointsDims[0];
	auto j_temp = y / controlPointsDims[1];

	auto is = std::floor(i_temp);
	auto js = std::floor(j_temp);

	auto u = i_temp - std::floor(i_temp);
	auto v = j_temp - std::floor(j_temp);

	auto offset = (nControlPoints[0] * nControlPoints[1]);

	int res_x = 0;
	int res_y = 0;


	for (int l = 1; l <= 4; l++)
	{
		for (int m = 1; m <= 4; m++)
		{
			auto index = ((is + l + 1) - 1) * nControlPoints[0] + js + m + 1;
			auto s = spline_basis(l - 1, u) * spline_basis(m - 1, v);
			res_x = res_x + s * phi->getPixelValueAt(index);
			if (index + offset < phi->getHeight())
			{
				res_y = res_y + s * phi->getPixelValueAt(index + offset);
			}
		}
	}

	return std::make_tuple(res_x, res_y);
}

double spline_basis(int i, double u)
{
	// Note: S_i ^ d(u) = S_i ^ 3(u) for cubic bsplines
	switch (i)
	{
	case 0:
		return (-1 * (u * u * u) + 3 * u * u - 3 * u + 1) / 6;
	case 1:
		return (3 * u * u * u - 6 * u * u + 4) / 6;
	case 2:
		return (-3 * u * u * u + 3 * u * u + 3 * u + 1) / 6;
	case 3:
		return u * u * u / 6;
	default:
		return 0;
	}
}

typedef struct dims_2D { int x; int y; };

ImageSimple init_grid(dims_2D d_ctrl, dims_2D n_img)
{
	dims_2D n_ctrl;
	n_ctrl.x = (ceil(n_img.x / d_ctrl.x)) + 3;
	n_ctrl.y = (ceil(n_img.x /d_ctrl.x)) + 3;
	
	ImageSimple phi = ImageSimple(n_ctrl.x * n_ctrl.y * 2, 1, 1, 0);
	int offset = n_ctrl.x * n_ctrl.y;

	for (int j = 1; j <= n_ctrl.y; j++)
	{
		for (int i = 1; i <= n_ctrl.x; i++)
		{
			int lin_index_x = ((i - 1) * n_ctrl.x) + j;
			int lin_index_y = ((i - 1) * n_ctrl.x) + j + offset;

			phi.setPixelValueAt((i - 2) * d_ctrl.y, lin_index_x); // -2 to place one control point before the first pixels
			phi.setPixelValueAt((j - 2) * d_ctrl.x, lin_index_y); // first pixel has coordinates (0,0)
		}
	}
	return phi;
}