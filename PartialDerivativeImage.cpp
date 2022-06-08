#include "PartialDerivativeImage.h"

std::vector<unsigned char> PartialDerivativeImage::PartialDerivative(ImageSimple* phi, ImageSimple* refImage, ImageSimple* templateImage, int controlPointsDim[])
{
	int imgWidth = refImage->getWidth();
	int imgHeight = refImage->getHeight();

	int nCtrlY = static_cast<int>(ceil(imgHeight / controlPointsDim[0])) + 3; // +3 for cubic bsplines
	int nCtrlX = static_cast<int>(ceil(imgWidth / controlPointsDim[1])) + 3;
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
		int maxX = std::max((x) * controlPointsDim[0], imgWidth);
		int maxY = std::max((y) * controlPointsDim[1], imgHeight);

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


std::tuple<int, int> PartialDerivativeImage::to2DIndex(int d, int dims[])
{
	int x = ceil(d / dims[1]);
	int y = d % dims[0];
	if (y == 0)
	{
		y = dims[0];
	}
	return std::make_tuple(x, y);
}


std::tuple<int, int> PartialDerivativeImage::Txy(int x, int y, const ImageSimple* const phi, int controlPointsDims[], int nControlPoints[])
{
	return std::make_tuple(2, 2); // TODO
}
