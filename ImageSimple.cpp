#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ImageSimple.h"
#include "stb_image.h"
#include "stb_image_write.h"
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

ImageSimple::ImageSimple(int w, int h, int ch) : width(w), height(h), channels(ch)
{
	size = w * h * channels;
	pixels = new stbi_uc[size];
	for (int i = 0; i < size; i++)
	{
		pixels[i] = 255;
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

	if (pixelVal < 0)
	{
		return 0;
	} else if (pixelVal > 255)
	{
		return 255;
	}
}

void ImageSimple::setPixelValueAt(stbi_uc pixelVal, int w, int h, int ch)
{
	if (w >= width || h >= height || ch >= channels)
	{
		throw std::invalid_argument("Given dims exceed that of image's.");
	}
	int pos = ch * height * width + w * height + h;
	setPixelValueAt(pixelVal, pos);
	/*pixels[w * height + h] = (stbi_uc)stbi__float2int(clipPixelVal(pixelVal));
	pixels[w * height + h] = (stbi_uc)stbi__float2int(clipPixelVal(pixelVal));
	pixels[w * height + h] = (stbi_uc)stbi__float2int(clipPixelVal(pixelVal));*/
}

void ImageSimple::setPixelValueAt(stbi_uc pixelVal, int pos)
{
	if (pos >= size)
	{
		throw std::invalid_argument("Given pos >= height * width * channels");
	}
	pixels[pos] = static_cast<stbi_uc>(clipPixelVal(pixelVal));
}

void ImageSimple::substract(ImageSimple* diff, const ImageSimple* subtrahend)
{
	// Sanity check for dims and ch
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


void ImageSimple::dot(ImageSimple* prod, ImageSimple* img2)
{

}

void ImageSimple::gradient(ImageSimple* gradX, ImageSimple* gradY)
{

}

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
