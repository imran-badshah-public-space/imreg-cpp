#include "ImageRegistration.h"
#include "ImageSimple.h"
#include "PartialDerivativeImage.h"
#include <QtWidgets/QApplication>
#include <chrono>
#include <cstring>

int main(int argc, char* argv[])
{
	//QApplication a(argc, argv);
	/*ImageRegistration w;
	w.show();*/

	//ImageSimple test("test3.jpg");
	//ImageSimple newImg = test;

	//// https://github.com/Yuhala/image-processing-cpp/tree/master/ImageProcessing

	//for (int i = 0; i < (test.getHeight() / 2) * test.getWidth() * test.getChannels(); i++)
	//{
	//	test.setPixelValueAt(test.getPixelValueAt(i), i);
	//}

	//for (int i = 0; i < (newImg.getWidth()); i++)
	//{
	//	for (int j = 0; j < newImg.getHeight() / 2; j++)
	//	{
	//		for (int k = 0; k < newImg.getChannels(); k++)
	//		{
	//			newImg.setPixelValueAt(newImg.getPixelValueAt(i, j, k), i, j, k);
	//		}
	//	}
	//}

	//test.write("new1.jpg");
	//newImg.write("new2.jpg");

	//ImageSimple diffed = test - &newImg;
	//diffed.write("diffed.jpg");

	//ImageSimple dotProd = test;
	//test.dot(&dotProd, &newImg);
	//dotProd.write("dotProd.jpg");

	//dims_2D n_img_t;
	//n_img_t.x = test.getWidth();
	//n_img_t.y = test.getHeight();
	//ImageSimple greyed = test;
	//test.greyscale(&greyed);
	//greyed.write("greyed.jpg");

	//return 0;


	ImageSimple original("img_r.jpg");
	ImageSimple tmpl("img_t.jpg");

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
	ImageSimple greyedTmpl = tmpl;
	ImageSimple greyedOriginal = original;

	tmpl.greyscale(&warpedTmpl);
	tmpl.greyscale(&greyedTmpl);
	original.greyscale(&greyedOriginal);

	auto timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char* timeNowCharPtr = std::ctime(&timeNow);
	timeNowCharPtr[strlen(timeNowCharPtr) - 1] = '\0';

	for (int iter = 1; iter <= n_iter; iter++)
	{
		std::printf("Iteration %d of %d.\n", iter, n_iter);
		std::printf("SSD dissimilarity: %f\n", greyedOriginal.ssd(&warpedTmpl));

		dims_2D ctrl_dims = {.x=d_ctrl.x, .y=d_ctrl.y};

		auto partial_derivative = PartialDerivative(&phi, &greyedOriginal, &greyedTmpl, ctrl_dims);

		/* Apply update to the control points */
		//phi = phi - std::transform(partial_derivative.cbegin(), partial_derivative.cend(), partial_derivative.begin(), [lambda](unsigned char c) { return c * lambda; });
		lambda *= 0.5;
		ImageSimple incremented_partial_derivative = partial_derivative * lambda;
		phi.write("20220913_midnight_phi_1.jpg");
		phi = phi - &(incremented_partial_derivative); // TODO: Substracting and overwriting
		phi.write("20220913_midnight_phi_2.jpg");
		auto [u_x, u_y] = get_displacement(&phi, d_ctrl, n_ctrl, n_img);
		warp(warpedTmpl, &greyedTmpl, &u_x, &u_y, n_img);
		//warpedTmpl.write("warpedTmpl.jpg");
		warpedTmpl.write("20220913_midnight_warpedTmpl.jpg");
		if (iter != n_iter)
		{
			std::printf("Writing files.\n");
			partial_derivative.write("20220913_midnight_partial_derivative.jpg");
			u_x.write("20220913_midnight_u_x.jpg");
			u_y.write("20220913_midnight_u_y.jpg");
			std::printf("Written files.\n");
			//return 0;
		}
		
		//warpedTmpl.write(strcat(timeNowCharPtr, "_warpedTmpl.jpg"));
	}

	std::printf("Final Writing files.\n");
	phi.write("20220913_midnight_phi.jpg");
	warpedTmpl.write("20220913_midnight_warpedTmpl.jpg");

	return 0;
}
