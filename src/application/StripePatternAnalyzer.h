#pragma once

#ifndef _STRIPE_PATTERN_ANALYZER_H
#define _STRIPE_PATTERN_ANALYZER_H

#include "StripePattern.h"

#define INTENSITY_THRESHOLD 5 // 5/255

struct CamCaptureCreationException {};

class StripePatternAnalyzer
{
private:	
	int stripeCount;
	int shiftCount;
	int patternWidth;
	int patternHeight;
	int projectionOffset;
	StripePattern* pattern;

	CvCapture* capture;

	IplImage* frame;
	IplImage* image;	
	IplImage* lastImage;
	IplImage* background;
	IplImage* foreground;
	IplImage* mask;
	IplImage** patternBuffer;
	IplImage* stripeIndexes;
	IplImage* phaseMapMod2PI;
	IplImage* grayCode;
	IplImage* phaseMap;
	
	int intensityBufferSize;
	double* intensity;

	int imageSize;

	bool maskProvided;

	IplImage* getStableImage();

	void filterPattern(IplImage* src, int index);

	void createMask();

	void calculatePhaseMapMod2PI();

	void calculateGrayCode();

	void demodulatePhase(bool smooth);

public:
    StripePatternAnalyzer(int stripeCount, int shiftCount, int patternWidth, int patternHeight, int projectionOffset, int intensityBufferSize = 5, IplImage* projectionMask = NULL);

	~StripePatternAnalyzer();

	void analyze(bool smooth = true);

	void showAll();

	void setMask(IplImage* projectionMask);

	void showCamImage();

	int getStripeCount()
	{
		return stripeCount;
	}

	int getShiftCount()
	{
		return shiftCount;
	}

	IplImage* getBackground()
	{
		return background;
	}

	IplImage* getForeground()
	{
		return foreground;
	}

	IplImage* getMask()
	{
		return mask;
	}

	IplImage* getPhaseMapMod2PI()
	{
		return phaseMapMod2PI;
	}

	IplImage* getPhaseMap()
	{
		return phaseMap;
	}
};

#endif
