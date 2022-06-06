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
	stbi_uc* pixels = nullptr;

public:
	int getWidth();
	int getHeight();
	int getWidth() const;
	int getHeight() const;

	ImageSimple(const char* filename);
	ImageSimple(int w, int h, int ch = 1); // Assume greyscale
	ImageSimple(const ImageSimple& img);
	~ImageSimple();

	bool load(const char* filename);
	bool write(const char* filename);
	stbi_uc clipPixelVal(stbi_uc pixelVal);
	stbi_uc getPixelValueAt(int w, int h, int ch);
	stbi_uc getPixelValueAt(int pos);
	stbi_uc getPixelValueAt(int pos) const;
	void setPixelValueAt(stbi_uc pixelVal, int w, int h, int ch);
	void setPixelValueAt(stbi_uc pixelVal, int pos);
	void substract(ImageSimple* diff, const ImageSimple* subtrahend);
	void dot(ImageSimple* prod, ImageSimple* img2);
	void gradient(ImageSimple* gradX, ImageSimple* gradY);

	ImageSimple operator-(const ImageSimple* const subtrahend) const;

	ImageType get_file_type(const char* filename);
};

