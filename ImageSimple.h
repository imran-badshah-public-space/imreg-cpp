#pragma once
#include "stb_image.h"
#include "stb_image_write.h"

enum class ImageType
{
	PNG, JPG, BMP, TGA
};

class ImageSimple
{
private:
	int width;
	int height;
	int channels;
	size_t size = 0;
	unsigned int* pixels = nullptr;

public:
	int getWidth();
	int getHeight();

	ImageSimple(const char* filename);
	ImageSimple(int w, int h, int ch = 1); // Assume greyscale
	ImageSimple(const ImageSimple& img);
	~ImageSimple();

	bool load(const char* filename);
	bool write(const char* filename);

	ImageType get_file_type(const char* filename);
};

