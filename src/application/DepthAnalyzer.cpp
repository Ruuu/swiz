#include "DepthAnalyzer.h"

DepthAnalyzer::DepthAnalyzer(int stripeCount, int shiftCount, int patternWidth, int patternHeight, int projectionOffset, int intensityBufferSize)
{
    patternAnalyzer = new StripePatternAnalyzer(stripeCount, shiftCount, patternWidth, patternHeight, projectionOffset, intensityBufferSize);
	phase0 = NULL;
	phase0Norm = NULL;
	depthBuffer = NULL;
	depthBufferNorm = NULL;
}

DepthAnalyzer::~DepthAnalyzer()
{
	if(patternAnalyzer != NULL)
		delete patternAnalyzer;

	if(phase0 != NULL)
		cvReleaseData(phase0);

	if(phase0Norm != NULL)
		cvReleaseData(phase0Norm);	

	if(depthBuffer != NULL)
		cvReleaseData(depthBuffer);

	if(depthBufferNorm != NULL)
		cvReleaseData(depthBufferNorm);	
}

void DepthAnalyzer::calibrate(float l, float b, bool smooth, bool accurate)
{
	L = l;
	B = b;
	this->smooth = smooth;
	this->accurate = accurate;
	
	patternAnalyzer->analyze(smooth);
	
	if(phase0 != NULL)
		cvReleaseData(phase0);
	phase0 = cvCreateImage(cvSize(patternAnalyzer->getPhaseMap()->width, patternAnalyzer->getPhaseMap()->height), IPL_DEPTH_32F, 1);
	cvCopy(patternAnalyzer->getPhaseMap(), phase0);

	if(phase0Norm != NULL)
		cvReleaseData(phase0Norm);
	phase0Norm = cvCreateImage(cvSize(phase0->width, phase0->height), IPL_DEPTH_32F, 1);
	
	if(depthBuffer != NULL)
		cvReleaseData(depthBuffer);
	depthBuffer = cvCreateImage(cvSize(phase0->width, phase0->height), IPL_DEPTH_32F, 1);

	if(depthBufferNorm != NULL)
		cvReleaseData(depthBufferNorm);
	depthBufferNorm = cvCreateImage(cvSize(phase0->width, phase0->height), IPL_DEPTH_32F, 1);
	
	calibrated = true;
}

void DepthAnalyzer::analyze()
{	
	if(calibrated)
	{
		patternAnalyzer->analyze(smooth);
		cvZero(depthBuffer);
		for(int y=0; y<depthBuffer->height; y++)
		{
			float* mask = (float*)(patternAnalyzer->getMask()->imageData + y*patternAnalyzer->getMask()->widthStep);
			float* depth = (float*)(depthBuffer->imageData + y*depthBuffer->widthStep);
			float* p0 = (float*)(phase0->imageData + y*phase0->widthStep);
			float* p = (float*)(patternAnalyzer->getPhaseMap()->imageData + y*patternAnalyzer->getPhaseMap()->widthStep);
			for(int x=0; x<depthBuffer->width; x++)
			{
				if(mask[x] == 1.0f)
				{
					depth[x] = (accurate ? (L * (p[x]-p0[x]) / (B + p[x] - p0[x])) : (L * (p[x]-p0[x]) / B));
				}
				else
				{
					depth[x] = 0.0f;
				}
			}
		}
		cvSmooth(depthBuffer, depthBuffer, CV_MEDIAN);
	}
}

void DepthAnalyzer::showAll()
{
	patternAnalyzer->showAll();	

	for(int y=0; y<depthBuffer->height; y++)
	{		
		float* p0 = (float*)(phase0->imageData + y*phase0->widthStep);
		float* p0n = (float*)(phase0Norm->imageData + y*phase0Norm->widthStep);
		float* depth = (float*)(depthBuffer->imageData + y*depthBuffer->widthStep);
		float* depthNorm = (float*)(depthBufferNorm->imageData + y*depthBufferNorm->widthStep);
		for(int x=0; x<depthBuffer->width; x++)
		{
			p0n[x] = p0[x] / ((patternAnalyzer->getStripeCount()-1)*_2PI);
			depthNorm[x] = depth[x] / L;
		}
	}

	cvShowImage("phase0", phase0Norm);
	cvShowImage("depthBuffer", depthBufferNorm);
}
