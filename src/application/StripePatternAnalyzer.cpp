#include "StripePatternAnalyzer.h"

StripePatternAnalyzer::StripePatternAnalyzer(int stripeCount, int shiftCount, int patternWidth, int patternHeight, int projectionOffset, int intensityBufferSize, IplImage* projectionMask)
{
	this->stripeCount = stripeCount;
	this->shiftCount = shiftCount;
	this->patternWidth = patternWidth;
	this->patternHeight = patternHeight;	
	this->projectionOffset = projectionOffset;

	this->intensityBufferSize = intensityBufferSize;
	this->intensity = new double[intensityBufferSize];

	pattern = NULL;	
	capture = NULL;
	frame = NULL;
	image = NULL;
	lastImage = NULL;
	background = NULL;
	foreground = NULL;
	mask = NULL;
	patternBuffer = NULL;
	stripeIndexes = NULL;
	phaseMapMod2PI = NULL;
	grayCode = NULL;
	phaseMap = NULL;


    pattern = new StripePattern(HYBRID, VERTICAL, patternWidth, patternHeight, stripeCount, shiftCount);

	capture = createCamCapture();
	if(!capture)
		throw new CamCaptureCreationException();

	frame = cvQueryFrame(capture);
	image = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
	lastImage = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
	background = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
	foreground = cvCreateImage(cvSize(frame->width, frame->height), frame->depth, frame->nChannels);
	mask = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_32F, 1);
	setMask(projectionMask);
	stripeIndexes = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_32F, 1);
	phaseMapMod2PI = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_32F, 1);
	grayCode = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 1);
	phaseMap = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_32F, 1);
	
	imageSize = image->width * image->height;

	patternBuffer = new IplImage*[pattern->getShotCount()];
	for(int i=0; i<pattern->getShotCount(); i++)
	{
		patternBuffer[i] = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_32F, 1);
	}
}

StripePatternAnalyzer::~StripePatternAnalyzer()
{
	if(patternBuffer != NULL)
	{
		for(int i=0; i<pattern->getShotCount(); i++)
		{
			if(patternBuffer[i] != NULL)
				cvReleaseData(patternBuffer[i]);
		}

		delete[] patternBuffer;
	}

	if(image != NULL)
		cvReleaseData(image);

	if(lastImage != NULL)
		cvReleaseData(lastImage);

	if(background != NULL)
		cvReleaseData(background);

	if(foreground != NULL)
		cvReleaseData(foreground);

	if(mask != NULL)
		cvReleaseData(mask);	

	if(stripeIndexes != NULL)
		cvReleaseData(stripeIndexes);

	if(phaseMapMod2PI != NULL)
		cvReleaseData(phaseMapMod2PI);

	if(grayCode != NULL)
		cvReleaseData(grayCode);	

	if(phaseMap != NULL)
		cvReleaseData(phaseMap);

	if(capture != NULL)
		cvReleaseCapture(&capture);

	if(pattern != NULL)
		delete pattern;

	delete[] intensity;
}

void StripePatternAnalyzer::showAll()
{
	cvShowImage("background", background);
	cvShowImage("foreground", foreground);
	cvShowImage("mask", mask);

	for(int i=0; i<pattern->getShotCount(); i++)
	{
		cvShowImage(("pattern image" + i), patternBuffer[i]);
	}

	for(int y=0; y<phaseMapMod2PI->height; y++)
	{		
		float* phaseMod = (float*)(phaseMapMod2PI->imageData + y*phaseMapMod2PI->widthStep);
		float* phase = (float*)(phaseMap->imageData + y*phaseMap->widthStep);
		uchar* gray = (uchar*)(grayCode->imageData + y*grayCode->widthStep);
		for(int x=0; x<phaseMapMod2PI->width; x++)
		{
			phase[x] /= ((stripeCount-1)*_2PI);
			phaseMod[x] /= _2PI;
			gray[x] *= (stripeCount-1);
		}
	}
	cvShowImage("phaseMapMod2PI", phaseMapMod2PI);
	cvShowImage("phaseMap", phaseMap);
	cvShowImage("grayCode", grayCode);
}

void StripePatternAnalyzer::analyze(bool smooth)
{
/*
Pobieram klatke t³a (background), potem wszystkie shoty patternu, a na koñcu foreground.
Zeby pobrac klatke musze sparwdzic czy jasnosc jest w miare stala w porownaniu z poprzednia klatka,
licze roznice sredniej wartosci z calej macierzy obrazu poprzedniego i obecnego i jesli mniejsza
od progu to moge zapamietac obecna klatke, jesli nie to pobieram nastepna dla tego samego shota az 
warunek spelniony.
Klatki background i foreground zapamietuje tak jak sa (24 bitowy kolor).
Klatki patternu zapamietuje w skali szarosci (32 bity - float) po uprzednim odfiltrowaniu tla: I = I/(2*Io).
Nastepnie obliczam faze mod2PI wed³ug wzorów dla kazdego miejsca w obrazie.
Po zapisaniu wszystkich klatek numeruje prazki - dekoduje kod gray'a.
Na podstawie numeracji prazkow demoduluje wczesniej obliczona faze.
Jesli smooth == true wtedy wygladzam ostateczny wynik filtrem medianowym o rozmiarze 5.
*/
	for(int i=0; i<pattern->getShotCount() + 2; i++)
	{
		if(i==0) //background
		{
			pattern->showBackground("Pattern", projectionOffset);
			cvCopy(getStableImage(), background);
		}
		else if(i == 1) //foreground
		{
			pattern->showForeground("Pattern", projectionOffset);
			cvCopy(getStableImage(), foreground);
			if(!maskProvided) createMask();
		}
		else
		{
			pattern->showShot("Pattern", i-2, projectionOffset);
			filterPattern(getStableImage(), i-2);
		}
	}

	calculatePhaseMapMod2PI();

	demodulatePhase(smooth);
}

void StripePatternAnalyzer::setMask(IplImage* projectionMask)
{
	if(projectionMask)
	{
		if(projectionMask != mask)
			cvCopy(projectionMask, mask);
		maskProvided = true;
	}
	else
	{
		maskProvided = false;
	}
}

void StripePatternAnalyzer::showCamImage()
{
	frame = cvQueryFrame(capture);
	cvCopy(frame, image);
	cvShowImage("CamImage", image);
}

IplImage* StripePatternAnalyzer::getStableImage()
{
	int idx = 0;
	bool ready = false;
	bool stable = false;	
	while(!stable)
	{
		cvWaitKey(5);

		cvCopy(image, lastImage);
		
		frame = cvQueryFrame(capture);
		cvCopy(frame, image);
		CvScalar sum = cvSum(image);
		intensity[idx] = ((sum.val[0] + sum.val[1] + sum.val[2]) / (3*imageSize));
		
		idx++;
		if(idx >= intensityBufferSize)
		{
			idx = 0;
			ready = true;
		}

		if(ready)
		{
			stable = true;
			for(int i=1; i<intensityBufferSize; i++)
			{
				if(abs(intensity[i-1] - intensity[i]) > INTENSITY_THRESHOLD)
					stable = false;
			}
		}
	}

	return image;
}

void StripePatternAnalyzer::filterPattern(IplImage* src, int index)
{
	for(int y=0; y<src->height; y++)
	{
		uchar* srcPtr = (uchar*)(src->imageData + y*src->widthStep);
		uchar* fore = (uchar*)(foreground->imageData + y*foreground->widthStep);
		float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
		float* dstPtr = (float*)(patternBuffer[index]->imageData + y*patternBuffer[index]->widthStep);
		for(int x=0; x<src->width; x++)
		{			
			dstPtr[x] = maskPtr[x] * 128.0f * (float)((float)srcPtr[3*x]/(float)fore[3*x] + (float)srcPtr[3*x+1]/(float)fore[3*x+1] + (float)srcPtr[3*x+2]/(float)fore[3*x+2]) / 765.0f;
			//dstPtr[x] = (float)(srcPtr[3*x] + srcPtr[3*x+1] + srcPtr[3*x+2]) / 765.0f;

			if(index < (pattern->getShotCount() - pattern->getShiftCount()))
			{
				if(maskPtr[x] == 1.0f)
				{
					if(dstPtr[x] > 0.5f)
						dstPtr[x] = 1.0f;
					else
						dstPtr[x] = 0.0f;
				}
				else
				{
					dstPtr[x] = 0.5f;
				}
			}
		}
	}
}

void StripePatternAnalyzer::createMask()
{
	for(int y=0; y<foreground->height; y++)
	{		
		uchar* fore = (uchar*)(foreground->imageData + y*foreground->widthStep);
		float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
		for(int x=0; x<foreground->width; x++)
		{			
			if(((fore[3*x] + fore[3*x+1] + fore[3*x+2])/3) > 64)
			{
				maskPtr[x] = 1.0f;
			}
			else
			{
				maskPtr[x] = 0.0f;
			}
		}
	}
}

void StripePatternAnalyzer::calculatePhaseMapMod2PI()
{
	cvZero(phaseMapMod2PI);

	if(shiftCount == 3)
	{
		for(int y=0; y<phaseMapMod2PI->height; y++)
		{		
			float* phase = (float*)(phaseMapMod2PI->imageData + y*phaseMapMod2PI->widthStep);
			float* I1 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+0]->imageData + y*phaseMapMod2PI->widthStep);
			float* I2 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+1]->imageData + y*phaseMapMod2PI->widthStep);
			float* I3 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+2]->imageData + y*phaseMapMod2PI->widthStep);
			float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
			for(int x=0; x<phaseMapMod2PI->width; x++)
			{			
				if(maskPtr[x] == 1.0f)
				{
					phase[x] = atan2f(sqrt(3.0f)*(I1[x]-I3[x]), 2.0f*I2[x]-I1[x]-I3[x]);
					if(phase[x] < 0)
						phase[x] += _2PI;
				}
			}
		}
	}
	else if(shiftCount == 4)
	{
		for(int y=0; y<phaseMapMod2PI->height; y++)
		{		
			float* phase = (float*)(phaseMapMod2PI->imageData + y*phaseMapMod2PI->widthStep);
			float* I1 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+0]->imageData + y*phaseMapMod2PI->widthStep);
			float* I2 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+1]->imageData + y*phaseMapMod2PI->widthStep);
			float* I3 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+2]->imageData + y*phaseMapMod2PI->widthStep);
			float* I4 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+3]->imageData + y*phaseMapMod2PI->widthStep);
			float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
			for(int x=0; x<phaseMapMod2PI->width; x++)
			{			
				if(maskPtr[x] == 1.0f)
				{
					phase[x] = atan2f(I4[x]-I2[x], I1[x]-I3[x]);
					if(phase[x] < 0)
						phase[x] += _2PI;
				}
			}
		}
	}
	else if(shiftCount == 5)
	{
		for(int y=0; y<phaseMapMod2PI->height; y++)
		{		
			float* phase = (float*)(phaseMapMod2PI->imageData + y*phaseMapMod2PI->widthStep);
			float* I1 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+0]->imageData + y*phaseMapMod2PI->widthStep);
			float* I2 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+1]->imageData + y*phaseMapMod2PI->widthStep);
			float* I3 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+2]->imageData + y*phaseMapMod2PI->widthStep);
			float* I4 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+3]->imageData + y*phaseMapMod2PI->widthStep);
			float* I5 = (float*)(patternBuffer[pattern->getShotCount()-pattern->getShiftCount()+4]->imageData + y*phaseMapMod2PI->widthStep);
			float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
			for(int x=0; x<phaseMapMod2PI->width; x++)
			{		
				if(maskPtr[x] == 1.0f)
				{
					phase[x] = atan2f(2.0f*(I2[x]-I4[x]), 2.0f*I3[x]-I1[x]-I5[x]);
					if(phase[x] < 0)
						phase[x] += _2PI;
				}
			}
		}
	}
}

void StripePatternAnalyzer::calculateGrayCode()
{
	cvZero(grayCode);

	for(int i=0; i<pattern->getShotCount()-pattern->getShiftCount(); i++)
	{
		for(int y=0; y<grayCode->height; y++)
		{
			uchar* gray = (uchar*)(grayCode->imageData + y*grayCode->widthStep);
			float* stripe = (float*)(patternBuffer[i]->imageData + y*patternBuffer[i]->widthStep);
			float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
			for(int x=0; x<grayCode->width; x++)
			{
				if(maskPtr[x] == 1.0f)
				{
					gray[x] = (gray[x] << 1) + (int)stripe[x];
				}
			}
		}
	}

	for(int y=0; y<grayCode->height; y++)
	{
		uchar* gray = (uchar*)(grayCode->imageData + y*grayCode->widthStep);
		float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
		for(int x=0; x<grayCode->width; x++)
		{
			if(maskPtr[x] == 1.0f)
			{
				for(int shift = 1; shift < 8; shift = 2*shift)
				{
					gray[x] = gray[x] ^ (gray[x] >> shift);
				}
			}
		}
	}
}

void StripePatternAnalyzer::demodulatePhase(bool smooth)
{
	calculateGrayCode();
	
	cvZero(phaseMap);

	for(int y=0; y<phaseMapMod2PI->height; y++)
	{		
		float* phaseMod = (float*)(phaseMapMod2PI->imageData + y*phaseMapMod2PI->widthStep);
		float* phase = (float*)(phaseMap->imageData + y*phaseMap->widthStep);
		uchar* gray = (uchar*)(grayCode->imageData + y*grayCode->widthStep);
		float* maskPtr = (float*)(mask->imageData + y*mask->widthStep);
		for(int x=0; x<phaseMapMod2PI->width; x++)
		{
			if(maskPtr[x] == 1.0f)
			{
				phase[x] = phaseMod[x] + gray[x] * _2PI;
			}
		}
	}

	if(smooth)
	{
		cvSmooth(phaseMap, phaseMap, CV_MEDIAN, 5);
	}
}
