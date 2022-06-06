#include "PartialDerivativeImage.h"
#include<math.h>

void PartialDerivativeImage::PartialDerivative(ImageSimple* phi, ImageSimple* refImage, ImageSimple* templateImage, int controlPointsDim[])
{
	int imgWidth = refImage->getWidth();
	int imgHeight = refImage->getHeight();

	int nCtrlY = static_cast<int>(ceil(imgHeight / controlPointsDim[0])) + 3; // +3 for cubic bsplines
	int nCtrlX = static_cast<int>(ceil(imgWidth / controlPointsDim[1])) + 3;

	int D = nCtrlX * nCtrlY;
	ImageSimple diff = *templateImage - refImage;

	ImageSimple* forceX = nullptr, * forceY = nullptr, * gradX = nullptr, * gradY = nullptr;
	templateImage->gradient(gradX, gradY);
	diff.dot(forceX, gradX);
	diff.dot(forceY, gradY);

	for (int d = 0; d < D; d++)
	{
		//int x, y;
		auto [x, y] = to2DIndex(d, controlPointsDim);
		int minX = std::max((x - 4) * controlPointsDim[0] + 1, 1);
		int minY = std::max((y - 4) * controlPointsDim[1] + 1, 1);
		int maxX = std::max((x)*controlPointsDim[0], imgWidth);
		int maxY = std::max((y)*controlPointsDim[1], imgHeight);

		int pdX = 0, pdY = 0;
		ImageSimple phishifted = ImageSimple(*phi);


	}
}


std::tuple<int, int> PartialDerivativeImage::to2DIndex(int d, int dims[])
{
	int x = ceil(d / dims[0]);
	int y = d % dims[0];
	if (y == 0)
	{
		y = dims[0];
	}
	return std::make_tuple(x, y);
}