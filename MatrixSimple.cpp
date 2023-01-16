#include "MatrixSimple.h"
#include <stdexcept>
#include <iostream>

MatrixSimple::MatrixSimple(int x, int y, double defaultVal)
{
	dims[0] = (x), dims[1] = (y);
	int length = x * y;
	data = new double[length];
	for (int i = 0; i < length; i++)
	{
		data[i] = 0;
	}
}

MatrixSimple::MatrixSimple(const MatrixSimple& mat)
{
	for (int i = 0; i < 2; i++)
	{
		dims[i] = mat.dims[i];
	}
	int length = getLength();
	data = new double[length];
	for (int i = 0; i < length; i++)
	{
		data[i] = mat.data[i];
	}
}

MatrixSimple::~MatrixSimple()
{
	delete[] data;
}

void MatrixSimple::prettyPrint()
{
	for (int i = 0; i < getLength(); i++)
	{
		if (i % dims[0] == 0)
		{
			std::cout << "\n";
		}
		std::cout << getValAt(i) << "\t";
	}
}

void MatrixSimple::setValAt(double val, int d)
{
	if (d >= getLength())
	{
		throw std::invalid_argument("Given dims exceed that of matrix's.");
	}
	data[d] = val;
}

void MatrixSimple::setValAt(double val, int x, int y)
{
	if (x >= dims[0] || y >= dims[1])
	{
		throw std::invalid_argument("Given dims exceed that of matrix's.");
	}
	data[(dims[0] * y) + x] = val;
}

double MatrixSimple::getValAt(int d) const
{
	if (d >= getLength())
	{
		throw std::invalid_argument("Given dims exceed that of matrix's.");
	}
	return data[d];
}
double MatrixSimple::getValAt(int x, int y) const
{
	if (x >= dims[0] || y >= dims[1])
	{
		throw std::invalid_argument("Given dims exceed that of matrix's.");
	}
	return data[(dims[0] * y) + x];
}

double MatrixSimple::getValAt(int d)
{
	if (d >= getLength())
	{
		throw std::invalid_argument("Given dims exceed that of matrix's.");
	}
	return data[d];
}

double MatrixSimple::getValAt(int x, int y)
{
	if (x >= dims[0] || y >= dims[1])
	{
		throw std::invalid_argument("Given dims exceed that of matrix's.");
	}
	return data[(dims[0] * y) + x];
}

int MatrixSimple::getLength() const
{
	return dims[0] * dims[1];
}


int MatrixSimple::getLength()
{
	return dims[0] * dims[1];
}

//ImageSimple MatrixSimple::toImageSimpleMinMax()
//{
//	ImageSimple img = ImageSimple(dims[0], dims[1]);
//	double min = findMin();
//	double max = findMax();
//	if (min < 0 || max > 1)
//	{
//		for (int d = 0; d < getLength(); d++)
//		{
//			img.setPixelValueAt(((data[d] - min) / (max - min)), d);
//		}
//		return img;
//	}
//
//	for (int d = 0; d < getLength(); d++)
//	{
//		img.setPixelValueAt(data[d], d);
//	}
//	
//	return img;
//}

MatrixSimple MatrixSimple::getMinMaxVersion() const
{
	MatrixSimple resMat(*this);
	double min = resMat.findMin();
	double max = resMat.findMax();
	for (int d = 0; d < getLength(); d++)
	{
		resMat.setValAt(((resMat.data[d] - min) / (max - min)), d);
	}
	return resMat;
}

double MatrixSimple::findMin()
{
	double res = data[0];
	for (int d = 0; d < getLength(); d++)
	{
		if (data[d] < res)
		{
			res = data[d];
		}
	}
	return res;
}

double MatrixSimple::findMax()
{
	double res = data[0];
	for (int d = 0; d < getLength(); d++)
	{
		if (data[d] > res)
		{
			res = data[d];
		}
	}
	return res;
}

std::tuple<int, int> MatrixSimple::getDims()
{
	return std::make_tuple(dims[0], dims[1]);
}

std::tuple<int, int> MatrixSimple::getDims() const
{
	return std::make_tuple(dims[0], dims[1]);
}

MatrixSimple MatrixSimple::operator-(const MatrixSimple* const subtrahend) const
{
	if (subtrahend->dims[0] != dims[0] || subtrahend->dims[1] != dims[1])
	{
		throw std::invalid_argument("Dimensions of subtrahend do not match that of minuend");
	}

	MatrixSimple diff = MatrixSimple(dims[0], dims[1]);
	for (int d = 0; d < getLength(); d++)
	{
		diff.setValAt(getValAt(d) - subtrahend->getValAt(d), d);
	}

	return diff;
}

void MatrixSimple::dot(MatrixSimple* prod, MatrixSimple* mat2)
{
	if (
		mat2->dims[0] != dims[0] || mat2->dims[1] != dims[1] ||
		prod->dims[0] != dims[0] || prod->dims[1] != dims[1]
		)
	{
		throw std::invalid_argument("Dimensions do not match.");
	}
	for (int i = 0; i < getLength(); i++)
	{
		prod->setValAt(this->getValAt(i) * mat2->getValAt(i), i);
	}
}

double MatrixSimple::ssd(MatrixSimple const* trg)
{
	double sim = 0;
	int len = getLength();
	MatrixSimple diff = *this - trg;
	for (uint64_t k = 0; k < len; ++k)
	{
		diff.data[k] *= diff.data[k];
	}
	for (uint64_t k = 0; k < len; ++k)
	{
		sim += diff.data[k];
	}
	return sim / len;
}

void MatrixSimple::gradient(MatrixSimple* gradX, MatrixSimple* gradY)
{
	//greyscale(gradX);
	//greyscale(gradY);
	//ImageSimple greyed = *this;
	//this->greyscale(&greyed);


	//ImageSimple blurImg(gradX->width, gradX->height, 1); // 20221018
	// TODO: Create profiles
	/*double gauss[9] = {
		1 / 16., 2 / 16., 1 / 16.,
		2 / 16., 4 / 16., 2 / 16.,
		1 / 16., 2 / 16., 1 / 16.
	};*/
	//gradX->convolveLinear(0, 3, 3, gauss, 1, 1);
	/*for (uint64_t k = 0; k < width * height; ++k)
	{
		blurImg.pixels[k] = gradX->pixels[k];
	}*/
	/*double sobel_x[9] = {
		1, 0, -1,
		2, 0, -2,
		1, 0, -1
	};
	double sobel_y[9] = {
		1, 2, 1,
		0, 0, 0,
		-1, -2, -1
	};*/
	//for (uint64_t i = 1; i < gradX->getWidth() - 1; i++)
	//{
	//	for (uint64_t j = 1; j < gradX->getHeight() - 1; j++)
	//	{
	//		auto valX = sobel_x[0] * greyed.getPixelValueAt(i - 1, j - 1, 0) + 
	//					sobel_x[2] * greyed.getPixelValueAt(i + 1, j - 1, 0) + 
	//					sobel_x[3] * greyed.getPixelValueAt(i - 1, j, 0) +
	//					sobel_x[5] * greyed.getPixelValueAt(i + 1, j, 0) +
	//					sobel_x[6] * greyed.getPixelValueAt(i - 1, j + 1, 0) +
	//					sobel_x[8] * greyed.getPixelValueAt(i + 1, j + 1, 0);

	//		auto valY = sobel_y[0] * greyed.getPixelValueAt(i - 1, j - 1, 0) +
	//					sobel_y[1] * greyed.getPixelValueAt(i, j - 1, 0) +
	//					sobel_y[2] * greyed.getPixelValueAt(i + 1, j - 1, 0) +
	//					sobel_y[6] * greyed.getPixelValueAt(i - 1, j + 1, 0) +
	//					sobel_y[7] * greyed.getPixelValueAt(i, j + 1, 0) +
	//					sobel_y[8] * greyed.getPixelValueAt(i + 1, j + 1, 0);
	//		gradX->setPixelValueAt(valX, i, j, 0);
	//		gradY->setPixelValueAt(valY, i, j, 0);
	//	}
	//	// TODO: Guard against borders
	//	// https://stackoverflow.com/questions/45047672/sobel-filter-algorithm-c-no-libraries
	//}
	double sobel_x[7][7] = {
		{-3 / 18, -2 / 13, -1 / 10, 0,  1 / 10, 2 / 13, 3 / 18},
		{-3 / 13, -2 / 8,  -1 / 5,  0,  1 / 5,  2 / 8,  3 / 13},
		{-3 / 10, -2 / 5,  -1 / 2,  0,  1 / 2,  2 / 5,  3 / 10},
		{-3 / 9,  -2 / 4,  -1 / 1,  0,  1 / 1,  2 / 4,  3 / 9} ,
		{-3 / 10, -2 / 5,  -1 / 2,  0,  1 / 2,  2 / 5,  3 / 10},
		{-3 / 13, -2 / 8,  -1 / 5,  0,  1 / 5,  2 / 8,  3 / 13},
		{-3 / 18, -2 / 13, -1 / 10, 0,  1 / 10, 2 / 13, 3 / 18}
	};
	//// TODO: transpose https://stackoverflow.com/a/16743203
	double sobel_y[7][7] = {
		{-1 / 6, -3 / 13, -3 / 10, -1 / 3, -3 / 10, -3 / 13, -1 / 6},
		{-2 / 13, -1 / 4, -2 / 5, -1 / 2, -2 / 5, -1 / 4, -2 / 13},
		{-1 / 10, -1 / 5, -1 / 2, -1,     -1 / 2, -1 / 5, -1 / 10},
		{0,       0,     0,     0,     0,     0,     0},
		{1 / 10, 1 / 5,     1 / 2,     1,     1 / 2,     1 / 5,     1 / 10},
		{2 / 13, 1 / 4,     2 / 5,     1 / 2,     2 / 5,     1 / 4,     2 / 13},
		{1 / 6,     3 / 13, 3 / 10, 1 / 3,     3 / 10, 3 / 13, 1 / 6}
	};
	/*double sobel_x[3][3] = {
		{ 1, 0, -1 },
		{ 2, 0, -2 },
		{ 1, 0, -1 }
	};
	double sobel_y[3][3] = {
		{ 1, 2, 1 },
		{ 0, 0, 0 },
		{ -1, -2, -1 }
	};*/

	int maskWidth = sizeof sobel_x / sizeof sobel_x[0];
	int maskHeight = sizeof sobel_x[0] / sizeof(double);
	auto [width, height] = gradX->getDims();

	for (uint64_t i = floor(maskWidth / 2); i < width - floor(maskWidth / 2); i++)
	{
		for (uint64_t j = floor(maskHeight / 2); j < height - floor(maskHeight / 2); j++)
		{

			double valX = 0;
			double valY = 0;
			for (int u = -floor(maskWidth / 2); u < ceil(maskWidth / 2); u++)
			{
				for (int v = -floor(maskHeight / 2); v < ceil(maskHeight / 2); v++)
				{
					valX += sobel_x[u + (int)floor(maskWidth / 2)][v + (int)floor(maskHeight / 2)] * this->getValAt(i + u, j + v);
					valY += sobel_y[u + (int)floor(maskWidth / 2)][v + (int)floor(maskHeight / 2)] * this->getValAt(i + u, j + v);
				}
			}
			gradX->setValAt(valX, i, j);
			gradY->setValAt(valY, i, j);
		}
	}
}

