#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ImageSimple.h"
#include <stdint.h>
#include <stdexcept>
#include <iostream>

#define CLIP_PIXELS(value) value < 0 ? 0 : (value > 255 ? 255 : value)

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

int ImageSimple::getChannels()
{
	return channels;
}


ImageSimple::ImageSimple(const char* filename)
{
	if (load(filename))
	{
		printf("Read %s\n", filename);
		size = width * height * channels;
	} else
	{
		printf("Failed to read %s\n", filename);
	}
}

ImageSimple::ImageSimple(int w, int h, int ch, int defaultPixelVal) : width(w), height(h), channels(ch)
{
	size = w * h * channels;
	pixels = new stbi_uc[size];
	for (int i = 0; i < size; i++)
	{
		pixels[i] = defaultPixelVal;
	}
}

ImageSimple::ImageSimple(const ImageSimple& img) : ImageSimple(img.width, img.height, img.channels)
{
	size = img.size;
	memcpy(pixels, img.pixels, sizeof(stbi_uc) * size);
}

ImageSimple::~ImageSimple()
{
	stbi_image_free(pixels);
}

bool ImageSimple::load(const char* filename)
{
	pixels = reinterpret_cast<stbi_uc*>(stbi_load(filename, &width, &height, &channels, 3));
	return pixels != nullptr;
}

bool ImageSimple::write(const char* filename)
{
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
		return true;
	} else
	{
		printf("\033[31;1m Failed to write \e[36m%s\e[0m, %d, %d, %d, %zu\n", filename, width, height, channels, size);
		printf("Failed to write %s, %d, %d, %d, %zu\n", filename, width, height, channels, size);
		return false;
	}
}

stbi_uc ImageSimple::getPixelValueAt(int w, int h, int ch)
{
	if (w >= width || h >= height || ch >= channels)
	{
		throw std::invalid_argument("Given dims exceed that of image's.");
	}
	int pos = ch * height * width + w * height + h;
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

void ImageSimple::setPixelValueAt(stbi_uc pixelVal, int w, int h, int ch)
{
	if (w >= width || h >= height || ch >= channels)
	{
		throw std::invalid_argument("Given dims exceed that of image's.");
	}
	int  pos = ch + channels * w + channels * width * h;
	setPixelValueAt(pixelVal, pos);
}

void ImageSimple::setPixelValueAt(stbi_uc pixelVal, int pos)
{
	if (pos >= size)
	{
		throw std::invalid_argument("Given pos >= height * width * channels");
	}
	pixels[pos] = static_cast<stbi_uc>(CLIP_PIXELS(pixelVal));
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
	
	ImageSimple blurImg(gradX->width, gradX->height, 1);
	double gauss[9] = {
		1 / 16., 2 / 16., 1 / 16.,
		2 / 16., 4 / 16., 2 / 16.,
		1 / 16., 2 / 16., 1 / 16.
	};
	//gradX->convolveLinear(0, 3, 3, gauss, 1, 1);
	for (uint64_t k = 0; k < width * height; ++k)
	{
		blurImg.pixels[k] = gradX->pixels[k];
	}
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
