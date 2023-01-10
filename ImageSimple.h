#pragma once
#include "stb_image.h"
#include "stb_image_write.h"
#include "MatrixSimple.h"
#include <memory>

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
	int getChannels();
	size_t getSize();
	int getWidth() const;
	int getHeight() const;
	int getChannels() const;
	size_t getSize() const;

	ImageSimple(const char* filename);
	ImageSimple(int w, int h, int ch = 1, int defaultPixelVal = 0); // Assume greyscale
	ImageSimple(const ImageSimple& img);
	ImageSimple(const MatrixSimple& mat, bool scale=true);
	~ImageSimple();

	bool load(const char* filename);
	bool write(const char* filename, bool scale=false);
	stbi_uc clipPixelVal(stbi_uc pixelVal);
	stbi_uc const getPixelValueAt(int w, int h, int ch) const;
	stbi_uc getPixelValueAt(int w, int h, int ch);
	stbi_uc getPixelValueAt(int pos);
	stbi_uc getPixelValueAt(int pos) const;
	void setPixelValueAt(double pixelVal, int w, int h, int ch);
	void setPixelValueAt(double pixelVal, int pos);
	void greyscale(ImageSimple* target);
	void dot(ImageSimple* prod, ImageSimple* img2);
	void gradient(ImageSimple* gradX, ImageSimple* gradY);
	float ssd(ImageSimple const *trg);

	void transformPixelsByMinMax();
	stbi_uc findMin();
	stbi_uc findMax();

	ImageSimple operator-(const ImageSimple* const subtrahend) const;
	ImageSimple operator+(const ImageSimple* const addended) const;
	ImageSimple operator*(const stbi_uc scalar) const;


	MatrixSimple transformToMatrix();

	ImageType get_file_type(const char* filename);
};

