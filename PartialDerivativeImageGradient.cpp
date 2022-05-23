#include "PartialDerivativeImageGradient.h"
#include "Image.h"
#include <math.h>

//void PartialDerivativeImageGradient::PartialDerivative(Image* phi, Image* refImage, const Image* templateImage, int controlPointsDim[])
//{
//	int imgWidth = refImage->getWidth();
//	int imgHeight= refImage->getHeight();
//
//	int nCtrlY = static_cast<int>(ceil(imgHeight / controlPointsDim[0])) + 3; // +3 for cubic bsplines
//	int nCtrlX = static_cast<int>(ceil(imgWidth / controlPointsDim[1])) + 3;
//
//	int d = nCtrlX * nCtrlY;
//
//	Image* diff;
//	diff->setHeight(imgHeight);
//	diff->setWidth(imgWidth);
//	
//	for (int i = 0; i < imgHeight; i++)
//	{
//		for (int j = 0; j < imgWidth; j++)
//			diff-> [i][j] = matOne[i][j] - matTwo[i][j];
//	}
//}

// https://www.youtube.com/watch?v=028GNYC32Rg
// https://www.youtube.com/watch?v=HGHbcRscFsgl
// https://www.youtube.com/watch?v=mRM5Js3VLCk
// https://www.scratchapixel.com/lessons/digital-imaging/simple-image-manipulations
// CImg https ://imkaywu.github.io/blog/2017/06/image-class/