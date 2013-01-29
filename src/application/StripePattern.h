#pragma once

#ifndef _STRIPE_PATTERN_H
#define _STRIPE_PATTERN_H

#include "3DScannerUtil.h"

#define max(a,b) (((a) > (b)) ? (a) : (b))

#define _PI    3.141592654f
#define _2PI   6.283185307f
#define _2PI3  2.094395102f

enum PatternType
{
	BINARY_CODE,
	GRAY_CODE,
	PHASE_SHIFT,
	HYBRID,
	RAINBOW,
	CONTINUOUSLY_VARYING_COLOR_CODE
	//DE_BRUIJN_SEQUENCE
};

enum PatternOrientation
{
	HORIZONTAL,
	VERTICAL
};

struct WrongStripeCountException {};

class StripePattern
{
private:
	PatternType type;
	PatternOrientation orientation;

	int width;
	int height;

	int stripeCount;
	int shiftCount;

	IplImage* singleShot;
	IplImage** multiShot;

	IplImage* background;
	IplImage* foreground;
	IplImage* checkerboard;

	int shotCount;

public:
	StripePattern(PatternType type, PatternOrientation orientation, int width, int height, int stripeCount = 1, int shiftCount = 3);

	~StripePattern();

	int getShotCount()
	{
		return shotCount;
	}

	int getStripeCount()
	{
		return stripeCount;
	}

	int getShiftCount()
	{
		return shiftCount;
	}

	int getWidth()
	{
		return width;
	}

	int getHeight()
	{
		return height;
	}

	PatternType getType()
	{
		return type;
	}

	PatternOrientation getOrientation()
	{
		return orientation;
	}

	IplImage* getShot(int index = 0);

	bool showShot(const char* window, int index = 0, int x = 0, int y = 0);

	bool showBackground(const char* window, int x = 0, int y = 0);

	bool showForeground(const char* window, int x = 0, int y = 0);

	bool showCheckerboard(const char* window, int x = 0, int y = 0);

private:
	void generateCheckerboard();

	void generateBinaryCode();

	void generateGrayCode();

	void generatePhaseShift();

	void generateHybrid();

	void generateRainbow();

	void generateContinuouslyVaryingColorCode();
};

#endif
