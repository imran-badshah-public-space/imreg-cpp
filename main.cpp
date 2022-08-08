#include "ImageRegistration.h"
#include "ImageSimple.h"
#include <QtWidgets/QApplication>

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);
	/*ImageRegistration w;
	w.show();*/
	ImageSimple test("test.jpg");
	ImageSimple newImg("test.jpg");


	// https://github.com/Yuhala/image-processing-cpp/tree/master/ImageProcessing



	for (int i = 0; i < (test.getHeight() / 2) * test.getWidth() * test.getChannels(); i++)
	{
		test.setPixelValueAt(0, i);
	}

	for (int i = 0; i < (newImg.getHeight() / 2); i++)
	{
		for (int j = 0; j < newImg.getWidth(); j++)
		{
			newImg.setPixelValueAt(0, j, i, 0);
			newImg.setPixelValueAt(0, j, i, 1);
			newImg.setPixelValueAt(0, j, i, 2);
		}
	}

	test.write("new1.jpg");
	newImg.write("new2.jpg");

	ImageSimple diffed = test - &newImg;
	diffed.write("diffed.jpg");

	ImageSimple dotProd = test;
	test.dot(&dotProd, &newImg);
	dotProd.write("dotProd.jpg");

	ImageSimple greyed = test;
	test.greyscale(&greyed);
	greyed.write("greyed.jpg");
	return 0;
}
