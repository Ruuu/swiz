#pragma once

#ifndef _DEPTH_ANALYZER_H
#define _DEPTH_ANALYZER_H

#include "StripePatternAnalyzer.h"

class DepthAnalyzer
{
private:
	StripePatternAnalyzer* patternAnalyzer;
	IplImage* phase0;
	IplImage* phase0Norm;
	IplImage* depthBuffer;
	IplImage* depthBufferNorm;

	float L; //odleglosc od plaszczyzny kalibracji
	float B; //odleglosc kamery od projektora

	bool smooth;
	bool accurate;
	bool calibrated;

public:
    DepthAnalyzer(int stripeCount, int shiftCount, int patternWidth, int patternHeight, int projectionOffset, int intensityBufferSize);

	~DepthAnalyzer();

	void calibrate(float l, float b, bool smooth = true, bool accurate = false);

	void analyze();

	IplImage* getDepthBuffer()
	{
		return depthBuffer;
	}

	IplImage* getForeground()
	{
		return patternAnalyzer->getForeground();
	}

	void showAll();

	void showCamImage()
	{
		patternAnalyzer->showCamImage();
	}
};

#endif
