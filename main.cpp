#include "ImageRegistration.h"
#include "ImageSimple.h"
#include "PartialDerivativeImage.hpp"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	/*ImageRegistration w;
	w.show();*/
	ImageSimple test("test.jpg");
	ImageSimple newImg("test.jpg");

	// https://github.com/Yuhala/image-processing-cpp/tree/master/ImageProcessing

	//for (int i = 0; i < (test.getHeight() / 2) * test.getWidth() * test.getChannels(); i++)
	//{
	//	test.setPixelValueAt(0, i);
	//}

	//for (int i = 0; i < (newImg.getHeight() / 2); i++)
	//{
	//	for (int j = 0; j < newImg.getWidth(); j++)
	//	{
	//		newImg.setPixelValueAt(0, j, i, 0);
	//		newImg.setPixelValueAt(0, j, i, 1);
	//		newImg.setPixelValueAt(0, j, i, 2);
	//	}
	//}

	//test.write("new1.jpg");
	//newImg.write("new2.jpg");

	//ImageSimple diffed = test - &newImg;
	//diffed.write("diffed.jpg");

	//ImageSimple dotProd = test;
	//test.dot(&dotProd, &newImg);
	//dotProd.write("dotProd.jpg");

	//ImageSimple greyed = test;
	//test.greyscale(&greyed);
	//greyed.write("greyed.jpg");


	ImageSimple original("test.jpg");
	ImageSimple tmpl("test.jpg");

	dims_2D d_ctrl, n_img;
	d_ctrl.x = 40;
	d_ctrl.y = 40;
	n_img.x = original.getWidth();
	n_img.y = original.getHeight();

	ImageSimple phi = init_grid(d_ctrl, n_img);
	dims_2D n_ctrl = calculateNumberOfCtrlPts(d_ctrl, n_img);

	int n_iter = 10;
	float lambda = 5.0;

	ImageSimple warpedTmpl = tmpl;

	for (int iter = 1; iter <= n_iter; iter++)
	{
		std::printf("Iteration %d of %d.\n", iter, n_iter);
		std::printf("SSD dissimilarity: %f\n", original.ssd(warpedTmpl));

		int ctrl_dims[2] = {d_ctrl.x, d_ctrl.y};

		auto partial_derivative = PartialDerivative(&phi, &original, &tmpl, ctrl_dims);

		/* Apply update to the control points */
		//phi = phi - std::transform(partial_derivative.cbegin(), partial_derivative.cend(), partial_derivative.begin(), [lambda](unsigned char c) { return c * lambda; });
		ImageSimple incremented_partial_derivative = partial_derivative * lambda;
		phi = phi - &(incremented_partial_derivative); // TODO: Substracting and overwriting
		auto [u_x, u_y] = get_displacement(&phi, d_ctrl, n_ctrl, n_img);
		warpedTmpl = warp(tmpl, u_x, u_y, n_img);
	}

	return 0;
}
