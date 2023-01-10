#pragma once
//#include "ImageSimple.h"
#include <tuple>

class MatrixSimple
{
public:
	MatrixSimple(int x, int y, double defaultVal = 0);
	MatrixSimple(const MatrixSimple& mat);
	~MatrixSimple();

	void prettyPrint();
	void setValAt(double val, int d);
	void setValAt(double val, int x, int y);
	double getValAt(int d) const;
	double getValAt(int x, int y) const;
	double getValAt(int d);
	double getValAt(int x, int y);
	int getLength() const;
	int getLength();
	//ImageSimple toImageSimpleMinMax();
	MatrixSimple getMinMaxVersion() const;
	double findMin();
	double findMax();
	std::tuple<int, int> getDims();
	std::tuple<int, int> getDims() const;

	MatrixSimple operator-(const MatrixSimple* const subtrahend) const;
	void dot(MatrixSimple* prod, MatrixSimple* mat2);
	double ssd(MatrixSimple const* trg);
	void gradient(MatrixSimple* gradX, MatrixSimple* gradY);

private:
	int dims[2];
	double* data;
};