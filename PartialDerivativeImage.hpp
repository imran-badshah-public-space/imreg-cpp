#include "PartialDerivativeImage.h"

ImageSimple PartialDerivative(ImageSimple* phi, ImageSimple* refImage, ImageSimple* templateImage, dims_2D controlPointsDim)
{
	dims_2D n_img;
	n_img.x = refImage->getWidth();
	n_img.y = refImage->getHeight();
	dims_2D nControlPoints = calculateNumberOfCtrlPts(controlPointsDim, n_img);
	int D = nControlPoints.x * nControlPoints.y;

	ImageSimple diff = *templateImage - refImage;
	ImageSimple gradientE = *phi;

	ImageSimple forceX = *templateImage, forceY = *templateImage, gradX = *templateImage, gradY = *templateImage;
	templateImage->gradient(&gradX, &gradY);
	diff.dot(&forceX, &gradX);
	diff.dot(&forceY, &gradY);
	gradX.write("20221020_gradX.jpg");
	gradY.write("20221020_gradY.jpg");

	for (int d = 0; d < D; d++)
	{
		if (d % 20 == 0)
		{
			std::printf("PD iter: %d of %d.\n", d, D);
		}
		auto [x, y] = to2DIndex(d, nControlPoints);
		// int minX = std::max((x - 4) * controlPointsDim.x + 1, 1);
		// int minY = std::max((y - 4) * controlPointsDim.y + 1, 1);
		int minX = std::max((x - 4) * controlPointsDim.x, 0);
		int minY = std::max((y - 4) * controlPointsDim.y, 0);
		int maxX = std::min((x)*controlPointsDim.x, n_img.x - 1);
		int maxY = std::min((y)*controlPointsDim.y, n_img.y - 1);

		uint8_t pdX = 0, pdY = 0;
		ImageSimple phishifted = ImageSimple(*phi);
		phishifted.setPixelValueAt(phishifted.getPixelValueAt(d) + 1, d);
		phishifted.setPixelValueAt(phishifted.getPixelValueAt(d + D) + 1, d + D);

		// TODO: for (int lx = minX; lx <= maxX; lx++)
		for (int lx = minX; lx < maxX; lx++)
		{
			for (int ly = minY; ly < maxY; ly++)
			{
				auto [res_x1, res_y1] = Txy(lx, ly, phi, controlPointsDim, nControlPoints);
				auto [res_x2, res_y2] = Txy(lx, ly, &phishifted, controlPointsDim, nControlPoints);
				int res_x = res_x2 - res_x1;
				int res_y = res_y2 - res_y1;
				pdX += res_x * forceX.getPixelValueAt(lx, ly, 0);
				pdY += res_y * forceY.getPixelValueAt(lx, ly, 0);
			}
		}

		double m_pd = sqrt(pow(pdX, 2) + pow(pdY, 2));
		if (m_pd != 0)
		{
			pdX = pdX / m_pd;
			pdY = pdY / m_pd;
		}
		gradientE.setPixelValueAt(pdX, d);
		gradientE.setPixelValueAt(pdY, D + d); // TODO: `D+d` bigger than than the initialised dims
	}

	return gradientE;
}


std::tuple<int, int> to2DIndex(int d, dims_2D dims)
{
	int x = ceil(d / dims.y);
	int y = d % dims.y;
	/*if (y == 0)
	{
		y = dims.y;
	}*/
	return std::make_tuple(x, y);
}

std::tuple<int, int> Txy(int x, int y, const ImageSimple* const phi, dims_2D controlPointsDims, dims_2D nControlPoints)
{
	auto i_temp = x / controlPointsDims.x;
	auto j_temp = y / controlPointsDims.y;

	auto is = std::floor(i_temp);
	auto js = std::floor(j_temp);

	auto u = i_temp - std::floor(i_temp);
	auto v = j_temp - std::floor(j_temp);

	auto offset = (nControlPoints.x * nControlPoints.y);

	int res_x = 0;
	int res_y = 0;


	for (int l = 1; l <= 4; l++)
	{
		for (int m = 1; m <= 4; m++)
		{
			auto index = ((is + l + 1) - 1) * nControlPoints.y + js + m + 1;
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

ImageSimple init_grid(dims_2D d_ctrl, dims_2D n_img)
{
	dims_2D n_ctrl = calculateNumberOfCtrlPts(d_ctrl, n_img);

	ImageSimple phi = ImageSimple(n_ctrl.x * n_ctrl.y * 2, 1, 1, 0);
	int offset = n_ctrl.x * n_ctrl.y;

	for (int j = 0; j < n_ctrl.y; j++)
	{
		for (int i = 0; i < n_ctrl.x; i++)
		{
			// int lin_index_x = ((i - 1)*n_ctrl.x) + j;
			// int lin_index_y = ((i - 1)*n_ctrl.x) + j + offset;
			int lin_index_x = ((i)*n_ctrl.y) + j;
			int lin_index_y = ((i)*n_ctrl.y) + j + offset;

			phi.setPixelValueAt((i - 1) * d_ctrl.x, lin_index_x); // -1 to place one control point before the first pixels
			phi.setPixelValueAt((j - 1) * d_ctrl.y, lin_index_y); // first pixel has coordinates (0,0)
		}
	}
	return phi;
}

dims_2D calculateNumberOfCtrlPts(dims_2D d_ctrl, dims_2D n_img)
{
	dims_2D n_ctrl;
	n_ctrl.x = (ceil(n_img.x / d_ctrl.x)) + 3;
	n_ctrl.y = (ceil(n_img.y / d_ctrl.y)) + 3;
	return n_ctrl;
}

std::tuple<ImageSimple, ImageSimple> get_displacement(const ImageSimple* const phi, dims_2D d_ctrl, dims_2D n_ctrl, dims_2D n_img)
{
	ImageSimple u_x = ImageSimple(n_img.x, n_img.y, 1, 0);
	ImageSimple u_y = u_x;

	for (int x = 0; x < n_img.x; x++)
	{
		for (int y = 0; y < n_img.y; y++)
		{
			auto [res_x, res_y] = Txy(x, y, phi, d_ctrl, n_ctrl);
			u_x.setPixelValueAt(res_x, x, y, 0);
			u_y.setPixelValueAt(res_y, x, y, 0);
		}
	}
	return std::make_tuple(u_x, u_y);
}

void warp(ImageSimple& target, const ImageSimple const* tmpl, const ImageSimple const* disp_x, const ImageSimple const* disp_y, dims_2D n_img)
{
	for (int x = 0; x < n_img.x; x++)
	{
		for (int y = 0; y < n_img.y; y++)
		{
			auto a = get_bilinear(tmpl, disp_x->getPixelValueAt(x, y, 0) + x, y + disp_y->getPixelValueAt(x, y, 0));
			target.setPixelValueAt(
				a,
				x,
				y,
				0
			);
		}
	}
}

stbi_uc get_bilinear(const ImageSimple const* tmpl, stbi_uc x, stbi_uc y)
{
	int intx = std::floor(x);
	int inty = std::floor(y);
	stbi_uc dx = x - intx;
	stbi_uc dy = y - inty;
	stbi_uc res = 0;

	if (intx >= 0 && intx + 1 < tmpl->getWidth() && inty >= 0 && inty + 1 < tmpl->getHeight())
	{
		res = tmpl->getPixelValueAt(intx, inty, 0) * (1 - dx) * (1 - dy)
			+ tmpl->getPixelValueAt(intx, inty + 1, 0) * (1 - dx) * dy
			+ tmpl->getPixelValueAt(intx + 1, inty, 0) * dx * (1 - dy)
			+ tmpl->getPixelValueAt(intx + 1, inty + 1, 0) * dx * dy;
	}
	return res;
}