#include "ImageRegistration.h"
#include "ImageSimple.h"
#include "MatrixSimple.h"
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

	////const int w = newImg.getWidth();
	////const int h = newImg.getHeight();
	  //
	////for (int i = 0; i < w; i++)
	////{
	////	for (int j = 0; j < h; j++)
	////	{
	////		for (int k = 0; k < newImg.getChannels(); k++)
	////		{
	////			newImg.setPixelValueAt(test.getPixelValueAt(w - 1 - i, h - 1 - j, k), i, j, k);
	////		}
	////	}
	////}

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
	//d_ctrl.x = 20;
	//d_ctrl.y = 20;
	d_ctrl.x = 3;
	d_ctrl.y = 3;
	n_img.x = original.getWidth();
	n_img.y = original.getHeight();

	std::vector<double> phi = init_grid(d_ctrl, n_img);
	dims_2D n_ctrl = calculateNumberOfCtrlPts(d_ctrl, n_img);

	//int n_iter = 20;
	//double lambda = 5.0;
	//int n_iter = 30;
	int n_iter = 10;
	double lambda = 2.0;

	//ImageSimple warpedTmplG = tmpl;
	//ImageSimple greyedTmpl = tmpl;
	//ImageSimple greyedOriginal = original;

	//tmpl.greyscale(&warpedTmplG);
	//tmpl.greyscale(&greyedTmpl);
	//original.greyscale(&greyedOriginal);

	MatrixSimple greyedOriginal = original.transformToMatrix().getMinMaxVersion();
	MatrixSimple greyedTmpl = tmpl.transformToMatrix().getMinMaxVersion();
	MatrixSimple warpedTmpl = greyedTmpl;

	auto timeNow = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
	char* timeNowCharPtr = std::ctime(&timeNow);
	timeNowCharPtr[strlen(timeNowCharPtr) - 1] = '\0';
	double ssd_prev = 100;

	for (int iter = 1; iter <= n_iter; iter++)
	{
		double ssd_current = greyedOriginal.ssd(&warpedTmpl);
		std::printf("\n\nIteration %d of %d.\n", iter, n_iter);
		std::printf("SSD dissimilarity: %f with Lambda: %f\n", ssd_current, lambda);
	/*	printf("\nphi: \n");
		for (auto p : phi)
			printf("%f ", p);
		printf("\n");*/

		auto partial_derivative = PartialDerivative(&phi, &greyedOriginal, &warpedTmpl, d_ctrl);
	/*	printf("\npartial_derivative: \n");
		for (auto p : partial_derivative)
			printf("%f ", p);
		printf("\n");*/

		/* Apply update to the control points */
		if (ssd_prev < 1.5 * ssd_current)
		{
			lambda *= 0.5;
			//lambda *= 0.8;
		}
		std::vector<double> incremented_partial_derivative(phi);
		std::transform(partial_derivative.begin(), partial_derivative.end(), incremented_partial_derivative.begin(), [lambda](double c) { return c * lambda; });
		std::transform(phi.begin(), phi.end(), incremented_partial_derivative.begin(), phi.begin(), std::minus<double>());
		auto [u_x, u_y] = get_displacement(&phi, d_ctrl, n_ctrl, n_img);
		warp(&warpedTmpl, &greyedTmpl, &u_x, &u_y, n_img);
		ssd_prev = ssd_current;

		ImageSimple(u_x).write("20220913_midnight_u_x.jpg");
		ImageSimple(u_y).write("20220913_midnight_u_y.jpg");

		/*printf("\nphi: \n");
		for (auto p : phi)
			printf("%f ", p);
		printf("\n");*/

		//if (iter == n_iter)
		//{
		//	std::printf("Writing files.\n");
		//	printf("partial_derivative 000: %f", partial_derivative.at(0));
		//	/*u_x.write("20220913_midnight_u_x.jpg");
		//	u_y.write("20220913_midnight_u_y.jpg");*/
		//	std::printf("Written files.\n");
		//}
		//warpedTmpl.prettyPrint();
		ImageSimple(warpedTmpl).write("20220913_midnight_warpedTmpl.jpg");
		MatrixSimple diff = greyedTmpl - &warpedTmpl;
		ImageSimple(diff).write("20220913_midnight_diff_templ.jpg");
		//warpedTmpl.write(strcat(timeNowCharPtr, "_warpedTmpl.jpg"));
	}

	/*std::printf("Final Writing files.\n");
	printf("phi 000: %f", phi.at(0));
	warpedTmpl.write("20220913_midnight_warpedTmpl.jpg");*/

	return 0;
}
