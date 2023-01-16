#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ImageSimple.h"
#include <stdint.h>
#include <stdexcept>
#include <iostream>

#define CLIP_PIXELS(value) value < 0 ? 0 : (value > 1 ? 1 : value)

int ImageSimple::getWidth()
{
	return width;
}

int ImageSimple::getWidth() const
{
	return width;
}

int ImageSimple::getHeight()
{
	return height;
}

int ImageSimple::getHeight() const
{
	return height;
}

int ImageSimple::getChannels() const
{
	return channels;
}

int ImageSimple::getChannels()
{
	return channels;
}

size_t ImageSimple::getSize() const
{
	return size;
}

size_t ImageSimple::getSize()
{
	return size;
}

ImageSimple::ImageSimple(const char* filename)
{
	if (load(filename))
	{
		printf("Read %s\n", filename);
		size = width * height * channels;
		//transformPixelsByMinMax();
	} else
	{
		printf("Failed to read %s\n", filename);
	}
}

ImageSimple::ImageSimple(int w, int h, int ch, int defaultPixelVal) : width(w), height(h), channels(ch)
{
	size = w * h * channels;
	//pixels = new stbi_uc[size]; // 20221018
	pixels = reinterpret_cast<stbi_uc*>(malloc(size));
	if (!pixels)
	{
		std::cout << "Error trying to malloc pixels with provided value" << std::endl;
		exit(1);
	}
	for (int i = 0; i < size; i++)
	{
		pixels[i] = defaultPixelVal;
	}
}

//ImageSimple::ImageSimple(const ImageSimple& img)
ImageSimple::ImageSimple(const ImageSimple& img) : ImageSimple(img.width, img.height, img.channels)
{
	size = img.size;
	pixels = reinterpret_cast<stbi_uc*>(malloc(size)); // 20221018
	if (!pixels)
	{
		std::cout << "Error trying to malloc pixels with provided value" << std::endl;
		exit(1);
	}
	for (int i = 0; i < size; i++)
	{
		pixels[i] = img.pixels[i];
	}
	//memcpy(pixels, img.pixels, sizeof(stbi_uc) * size); // 20221018
	//memcpy(this, &img, sizeof(ImageSimple));
}

ImageSimple::ImageSimple(const MatrixSimple& mat, bool scale)
{
	MatrixSimple minMaxedMat = mat.getMinMaxVersion();
	auto [width, height] = minMaxedMat.getDims();
	this->width = width;
	this->height = height;
	this->channels = 1;
	this->size = minMaxedMat.getLength();
	pixels = reinterpret_cast<stbi_uc*>(malloc(this->size));
	if (!pixels)
	{
		std::cout << "Error trying to malloc pixels with provided value" << std::endl;
		exit(1);
	}

	if (scale)
	{
		for (int i = 0; i < this->size; i++)
		{
			pixels[i] = minMaxedMat.getValAt(i) * 255;
		}
	} else
	{
		for (int i = 0; i < this->size; i++)
		{
			pixels[i] = minMaxedMat.getValAt(i);
		}
	}
}

ImageSimple::~ImageSimple()
{
	stbi_image_free(pixels);
}

void ImageSimple::transformPixelsByMinMax()
{
	stbi_uc min = findMin();
	stbi_uc max = findMax();
	if (min == max)
	{
		std::cout << "Min and max are equal. Not transforming." << std::endl;
		return;
	}
	for (int d = 0; d < getSize(); d++)
	{
		setPixelValueAt(((pixels[d] - min) / (max - min)), d);
	}
}

stbi_uc ImageSimple::findMin()
{
	stbi_uc res = pixels[0];
	for (int d = 0; d < getSize(); d++)
	{
		if (pixels[d] < res)
		{
			res = pixels[d];
		}
	}
	return res;
}

stbi_uc ImageSimple::findMax()
{
	stbi_uc res = pixels[0];
	for (int d = 0; d < getSize(); d++)
	{
		if (pixels[d] > res)
		{
			res = pixels[d];
		}
	}
	return res;
}

bool ImageSimple::load(const char* filename)
{
	pixels = reinterpret_cast<stbi_uc*>(stbi_load(filename, &width, &height, &channels, 0));
	return pixels != nullptr;
}

bool ImageSimple::write(const char* filename, bool scale)
{
	//std::shared_ptr<stbi_uc> scaledPixels = std::make_shared<stbi_uc>(pixels);
	/*size_t nPixels = getSize();
	//stbi_uc* scaledPixels = reinterpret_cast<stbi_uc*>(malloc(nPixels));
	stbi_uc* scaledPixels = new stbi_uc[nPixels];
	if (!scaledPixels)
	{
		std::cout << "Error trying to malloc pixels with provided value" << std::endl;
		exit(1);
	}
	if (scale)
	{
		for (int i = 0; i < nPixels; i++)
		{
			auto p = getPixelValueAt(i);
			scaledPixels[i] = p * 255.00;
		}
	} else
	{
		for (int i = 0; i < nPixels; i++)
		{
			auto p = getPixelValueAt(i);
			scaledPixels[i] = p;
		}
	}*/
	ImageType type = get_file_type(filename);
	int success = 0;

	switch (type)
	{
	case ImageType::PNG:
		success = stbi_write_png(filename, width, height, channels, pixels, width * channels);
		break;
	case ImageType::BMP:
		success = stbi_write_bmp(filename, width, height, channels, pixels);
		break;
	case ImageType::JPG:
		success = stbi_write_jpg(filename, width, height, channels, pixels, 100);
		break;
	case ImageType::TGA:
		success = stbi_write_tga(filename, width, height, channels, pixels);
		break;
	}
	if (success != 0)
	{
		printf("\033[32mWrote \e[36m%s\e[0m, %d, %d, %d, %zu\n", filename, width, height, channels, size);
		//delete[] scaledPixels;
		return true;
	} else
	{
		printf("\033[31;1m Failed to write \e[36m%s\e[0m, %d, %d, %d, %zu\n", filename, width, height, channels, size);
		printf("Failed to write %s, %d, %d, %d, %zu\n", filename, width, height, channels, size);
		//delete[] scaledPixels;
		return false;
	}
}

stbi_uc const ImageSimple::getPixelValueAt(int w, int h, int ch) const
{
	if (w >= width || h >= height || ch >= channels)
	{
		throw std::invalid_argument("Given dims exceed that of image's.");
	}
	int  pos = ch + channels * w + channels * width * h;
	return getPixelValueAt(pos);
}


stbi_uc ImageSimple::getPixelValueAt(int w, int h, int ch)
{
	if (w >= width || h >= height || ch >= channels)
	{
		throw std::invalid_argument("Given dims exceed that of image's.");
	}
	//int pos = ch * height * width + w * height + h;
	int  pos = ch + channels * w + channels * width * h;
	//int  pos = w + width * ch + channels * width * h;
	return getPixelValueAt(pos);
}

stbi_uc ImageSimple::getPixelValueAt(int pos)
{
	if (pos >= size)
	{
		throw std::invalid_argument("Given pos >= height * width * channels");
	}
	return pixels[pos];
}

stbi_uc ImageSimple::getPixelValueAt(int pos) const
{
	if (pos >= height * width * channels)
	{
		throw std::invalid_argument("Given pos >= height * width");
	}
	return pixels[pos];
}

stbi_uc ImageSimple::clipPixelVal(stbi_uc pixelVal)
{
	return CLIP_PIXELS(pixelVal);
}

void ImageSimple::setPixelValueAt(double pixelVal, int w, int h, int ch)
{
	if (w >= width || h >= height || ch >= channels)
	{
		throw std::invalid_argument("Given dims exceed that of image's.");
	}
	int  pos = ch + channels * w + channels * width * h;
	setPixelValueAt(pixelVal, pos);
}

void ImageSimple::setPixelValueAt(double pixelVal, int pos)
{
	if (pos >= size)
	{
		throw std::invalid_argument("Given pos >= height * width * channels");
	}
	pixels[pos] = pixelVal;
}

ImageSimple ImageSimple::operator-(const ImageSimple* const subtrahend) const
{
	if (subtrahend->getWidth() != width || subtrahend->getHeight() != height || subtrahend->channels != channels)
	{
		throw std::invalid_argument("Dimensions of subtrahend do not match that of minuend");
	}

	ImageSimple diff = ImageSimple(width, height, channels);
	for (int d = 0; d < size; d++)
	{
		diff.setPixelValueAt(getPixelValueAt(d) - subtrahend->getPixelValueAt(d), d);
	}

	return diff;
}

ImageSimple ImageSimple::operator+(const ImageSimple* const addended) const
{
	if (addended->getWidth() != width || addended->getHeight() != height || addended->channels != channels)
	{
		throw std::invalid_argument("Dimensions of addends do not match.");
	}

	ImageSimple diff = ImageSimple(width, height, channels);
	for (int d = 0; d < size; d++)
	{
		diff.setPixelValueAt(getPixelValueAt(d) + addended->getPixelValueAt(d), d);
	}

	return diff;
}

ImageSimple ImageSimple::operator*(const stbi_uc scalar) const
{
	ImageSimple result = *this;
	for (int d = 0; d < size; d++)
	{
		result.setPixelValueAt(this->getPixelValueAt(d) * scalar, d);
	}
	return result;
}

// TODO: Rename this func
void ImageSimple::dot(ImageSimple* prod, ImageSimple* img2)
{
	if (
		img2->getWidth() != width || img2->getHeight() != height || img2->channels != channels ||
		prod->getWidth() != width || prod->getHeight() != height || prod->channels != channels
		)
	{
		throw std::invalid_argument("Dimensions do not match.");
	}
	for (int i = 0; i < size; i++)
	{
		prod->setPixelValueAt(this->pixels[i] * img2->pixels[i], i);
	}
}

void ImageSimple::greyscale(ImageSimple* target)
{
	//for (int i = 0; i < height * width; i += channels)
	//{
	//	// Super simple for now: (r+g+b)/3
	//	int grey = (pixels[i] + pixels[i + 1] + pixels[i + 2]) / 3;
	//	target->setPixelValueAt(grey, i);
	//}
	if (target->channels == 1)
	{
		return;
	}
	target->channels = 1;
	target->size = target->height * target->width * target->channels;
	for (uint64_t k = 0; k < height * width; ++k)
	{
		target->pixels[k] = pixels[channels * k];
	}
}

void ImageSimple::gradient(ImageSimple* gradX, ImageSimple* gradY)
{
	greyscale(gradX);
	greyscale(gradY);
	ImageSimple greyed = *this;
	this->greyscale(&greyed);
	
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
	//double sobel_x[7][7] = {
	//	{-3 / 18, -2 / 13, -1 / 10, 0,  1 / 10, 2 / 13, 3 / 18},
	//	{-3 / 13, -2 / 8,  -1 / 5,  0,  1 / 5,  2 / 8,  3 / 13},
	//	{-3 / 10, -2 / 5,  -1 / 2,  0,  1 / 2,  2 / 5,  3 / 10},
	//	{-3 / 9,  -2 / 4,  -1 / 1,  0,  1 / 1,  2 / 4,  3 / 9} ,
	//	{-3 / 10, -2 / 5,  -1 / 2,  0,  1 / 2,  2 / 5,  3 / 10},
	//	{-3 / 13, -2 / 8,  -1 / 5,  0,  1 / 5,  2 / 8,  3 / 13},
	//	{-3 / 18, -2 / 13, -1 / 10, 0,  1 / 10, 2 / 13, 3 / 18}
	//};
	//// TODO: transpose https://stackoverflow.com/a/16743203
	//double sobel_y[7][7] = {
	//	{-1 / 6, -3 / 13, -3 / 10, -1 / 3, -3 / 10, -3 / 13, -1 / 6},
	//	{-2 / 13, -1 / 4, -2 / 5, -1 / 2, -2 / 5, -1 / 4, -2 / 13},
	//	{-1 / 10, -1 / 5, -1 / 2, -1,     -1 / 2, -1 / 5, -1 / 10},
	//	{0,       0,     0,     0,     0,     0,     0},
	//	{1 / 10, 1 / 5,     1 / 2,     1,     1 / 2,     1 / 5,     1 / 10},
	//	{2 / 13, 1 / 4,     2 / 5,     1 / 2,     2 / 5,     1 / 4,     2 / 13},
	//	{1 / 6,     3 / 13, 3 / 10, 1 / 3,     3 / 10, 3 / 13, 1 / 6}
	//};
	double sobel_x[3][3] = {
		{ 1, 0, -1 },
		{ 2, 0, -2 },
		{ 1, 0, -1 }
	};
	double sobel_y[3][3] = {
		{ 1, 2, 1 },
		{ 0, 0, 0 },
		{ -1, -2, -1 }
	};

	int maskWidth = sizeof sobel_x / sizeof sobel_x[0];
	int maskHeight = sizeof sobel_x[0] / sizeof(double);

	for (uint64_t i = floor(maskWidth / 2); i < gradX->getWidth() - floor(maskWidth / 2); i++)
	{
		for (uint64_t j = floor(maskHeight / 2); j < gradX->getHeight() - floor(maskHeight / 2); j++)
		{
			
			stbi_uc valX = 0;
			stbi_uc valY = 0;
			for (int u = -floor(maskWidth / 2); u < ceil(maskWidth / 2); u++)
			{
				for (int v = -floor(maskHeight / 2); v < ceil(maskHeight / 2); v++)
				{
					valX += sobel_x[u + (int)floor(maskWidth / 2)][v + (int)floor(maskHeight / 2)] * greyed.getPixelValueAt(i + u, j + v, 0);
					valY += sobel_y[u + (int)floor(maskWidth / 2)][v + (int)floor(maskHeight / 2)] * greyed.getPixelValueAt(i + u, j + v, 0);
				}
			}
			gradX->setPixelValueAt(valX, i, j, 0);
			gradY->setPixelValueAt(valY, i, j, 0);
		}
	}

}


float ImageSimple::ssd(ImageSimple const *trg)
{
	float sim = 0;
	ImageSimple diff = *this - trg;
	for (uint64_t k = 0; k < size; ++k)
	{
		diff.pixels[k] *= diff.pixels[k];
	}
	for (uint64_t k = 0; k < size; ++k)
	{
		sim += diff.pixels[k];
	}
	return sim / size;
}


MatrixSimple ImageSimple::transformToMatrix()
{
	MatrixSimple mat(width, height);
	// Since MatrixSimple is currently only 2D, transform to greyscale
	ImageSimple greyed = *this;
	this->greyscale(&greyed);
	for (uint64_t k = 0; k < greyed.getSize(); ++k)
	{
		mat.setValAt(greyed.getPixelValueAt(k), k);
	}
	return mat;
}


//Image& Image::convolve_linear(uint8_t channel, uint32_t ker_w, uint32_t ker_h, double ker[], uint32_t cr, uint32_t cc)
//{
//	if (ker_w * ker_h > 224)
//	{
//		return fd_convolve_clamp_to_0(channel, ker_w, ker_h, ker, cr, cc);
//	} else
//	{
//		return std_convolve_clamp_to_0(channel, ker_w, ker_h, ker, cr, cc);
//	}
//}

ImageType ImageSimple::get_file_type(const char* filename)
{
	const char* ext = strrchr(filename, '.');
	if (ext != nullptr)
	{
		if (strcmp(ext, ".png") == 0)
		{
			return ImageType::PNG;
		} else if (strcmp(ext, ".jpg") == 0)
		{
			return ImageType::JPG;
		} else if (strcmp(ext, ".bmp") == 0)
		{
			return ImageType::BMP;
		} else if (strcmp(ext, ".tga") == 0)
		{
			return ImageType::TGA;
		}
	}
	return ImageType::PNG;
}
