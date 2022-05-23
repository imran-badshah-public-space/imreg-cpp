#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "ImageSimple.h"
#include <stdint.h>


#define CLIP_PIXELS(value) value < 0 ? 0 : (value > 255 ? 255 : value)

int ImageSimple::getWidth()
{
	return width;
}

int ImageSimple::getHeight()
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
	pixels = new unsigned int[size];
}

ImageSimple::ImageSimple(const ImageSimple& img) : ImageSimple(img.width, img.height, img.channels)
{
	memcpy(pixels, img.pixels, size);
}

ImageSimple::~ImageSimple()
{
	stbi_image_free(pixels);
}

bool ImageSimple::load(const char* filename)
{
	pixels = reinterpret_cast<unsigned int*>(stbi_load(filename, &width, &height, &channels, 0));
	channels = 0;
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
		return false;
	}
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
