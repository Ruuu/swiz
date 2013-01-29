#include "StripePattern.h"

StripePattern::StripePattern(PatternType type, PatternOrientation orientation, int width, int height, int stripeCount, int shiftCount)
{		
	this->type = type;
	this->orientation = orientation;
	
	this->width = width;
	this->height = height;

	this->stripeCount = stripeCount;
	this->shiftCount = shiftCount;

	singleShot = NULL;

	multiShot = NULL;

	switch(type)
	{
		case BINARY_CODE:
			if((log((double)stripeCount)/log(2.0)) == (int)(log((double)stripeCount)/log(2.0)) &&
				((orientation == HORIZONTAL && (height%stripeCount) == 0) ||
				(orientation == VERTICAL && (width%stripeCount) == 0)))
			{
				generateBinaryCode();
			}
			else
			{
				throw new WrongStripeCountException();
			}
			break;

		case GRAY_CODE:
			if((log((double)stripeCount)/log(2.0)) == (int)(log((double)stripeCount)/log(2.0)) &&
				((orientation == HORIZONTAL && (height%stripeCount) == 0) ||
				(orientation == VERTICAL && (width%stripeCount) == 0)))
			{
				generateGrayCode();
			}
			else
			{
				throw new WrongStripeCountException();
			}
			break;

		case PHASE_SHIFT:
			if((shiftCount == 3 || shiftCount == 4 || shiftCount == 5) && 
				((orientation == HORIZONTAL && (height%stripeCount) == 0) ||
				(orientation == VERTICAL && (width%stripeCount) == 0)))
			{
				generatePhaseShift();
			}
			else
			{
				throw new WrongStripeCountException();
			}
			break;

		case HYBRID:
			if((shiftCount == 3 || shiftCount == 4 || shiftCount == 5) && 
				((log((double)stripeCount)/log(2.0)) == (int)(log((double)stripeCount)/log(2.0)) &&
				((orientation == HORIZONTAL && (height%stripeCount) == 0) ||
				(orientation == VERTICAL && (width%stripeCount) == 0))))
			{
				generateHybrid();
			}
			else
			{
				throw new WrongStripeCountException();
			}
			break;

		case RAINBOW:
			generateRainbow();
			break;

		case CONTINUOUSLY_VARYING_COLOR_CODE:
			if((orientation == HORIZONTAL && (height%stripeCount) == 0) ||
				(orientation == VERTICAL && (width%stripeCount) == 0))
			{
				generateContinuouslyVaryingColorCode();
			}
			else
			{
				throw new WrongStripeCountException();
			}
			break;
	}

	background = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvSet(background, cvScalarAll(0.0));

	foreground = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
	cvSet(foreground, cvScalarAll(255.0));

	generateCheckerboard();
}

StripePattern::~StripePattern()
{
	if(singleShot != NULL)
		cvReleaseData(singleShot);

	if(multiShot != NULL)
	{
		for(int i=0; i<shotCount; i++)
		{
			if(multiShot[i] != NULL)
				cvReleaseData(multiShot[i]);
		}

		delete[] multiShot;
	}

	if(background != NULL)
		cvReleaseData(background);

	if(foreground != NULL)
		cvReleaseData(foreground);

	if(checkerboard != NULL)
		cvReleaseData(checkerboard);
}

IplImage* StripePattern::getShot(int index)
{
	if(singleShot != NULL)
		return singleShot;

	if(multiShot != NULL && index < shotCount)
		return multiShot[index];

	return NULL;
}

bool StripePattern::showShot(const char* window, int index, int x, int y)
{
	IplImage* image = getShot(index);
	
	if(image)
		return showImageNoFrame(window, image, x, y);

	return false;
}

bool StripePattern::showBackground(const char* window, int x, int y)
{
	if(background)
		return showImageNoFrame(window, background, x, y);

	return false;
}

bool StripePattern::showForeground(const char* window, int x, int y)
{
	if(foreground)
		return showImageNoFrame(window, foreground, x, y);

	return false;
}

bool StripePattern::showCheckerboard(const char* window, int x, int y)
{
	if(checkerboard)
		return showImageNoFrame(window, checkerboard, x, y);

	return false;
}

void StripePattern::generateCheckerboard()
{
	checkerboard = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);

	int size = (int)(width / 8);
	uchar color;
	for(int y=0; y<height; y++)
	{
		uchar* imgPtr = (uchar*)(checkerboard->imageData + y*checkerboard->widthStep);
		for(int x=0; x<width; x++)
		{
			color = ((int)(y/size)%2 == 0) ? 255 : 0;

			if((int)(x/size)%2 != 0)
				color = (color==0) ? 255 : 0;
			
			imgPtr[x] = color;
		}
	}
}

void StripePattern::generateBinaryCode()
{
	shotCount = (int)(log((double)stripeCount)/log(2.0));
	multiShot = new IplImage*[shotCount];

	int stripeWidth = (orientation == HORIZONTAL ? height : width);
	int stripeWidthCounter;
	uchar stripeColor;
	IplImage* image;
	for(int i=0; i<shotCount; i++)
	{
		image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		stripeWidth = (int)(stripeWidth/2);
		stripeWidthCounter = 0;
		stripeColor = 0;

		for(int y=0; y<height; y++)
		{
			if(orientation == HORIZONTAL)
			{					
				if(y == 0)
				{
					stripeWidthCounter = 0;
					stripeColor = 0;
				}

				if(stripeWidthCounter == stripeWidth)
				{
					stripeWidthCounter = 0;
					stripeColor = ((stripeColor == 0) ? 255 : 0);
				}
				stripeWidthCounter++;
			}

			uchar* imgPtr = (uchar*)(image->imageData + y*image->widthStep);
			for(int x=0; x<width; x++)
			{
				if(orientation == VERTICAL)
				{					
					if(x == 0)
					{
						stripeWidthCounter = 0;
						stripeColor = 0;
					}

					if(stripeWidthCounter == stripeWidth)
					{
						stripeWidthCounter = 0;
						stripeColor = ((stripeColor == 0) ? 255 : 0);
					}
					stripeWidthCounter++;
				}
				
				imgPtr[x] = stripeColor;
			}
		}
		
		multiShot[i] = image;
	}
}

void StripePattern::generateGrayCode()
{
	shotCount = (int)(log((double)stripeCount)/log(2.0));
	multiShot = new IplImage*[shotCount];

	int stripeWidth = (orientation == HORIZONTAL ? height : width);
	int stripeWidthCounter;
	uchar stripeColor;
	uchar lastStripeColor;
	IplImage* image;
	for(int i=0; i<shotCount; i++)
	{
		image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
		stripeWidth = (int)(stripeWidth/2);
		stripeWidthCounter = 0;
		stripeColor = 0;
		lastStripeColor = 0;

		for(int y=0; y<height; y++)
		{
			if(orientation == HORIZONTAL)
			{	
				if(y == 0)
				{
					stripeWidthCounter = 0;
					stripeColor = 0;
					lastStripeColor = 0;
				}
				
				if(stripeWidthCounter == stripeWidth)
				{
					stripeWidthCounter = 0;
					if(lastStripeColor == stripeColor)
						stripeColor = ((stripeColor == 0) ? 255 : 0);
					else
						lastStripeColor = stripeColor;
				}
				stripeWidthCounter++;
			}

			uchar* imgPtr = (uchar*)(image->imageData + y*image->widthStep);
			for(int x=0; x<width; x++)
			{
				if(orientation == VERTICAL)
				{					
					if(x == 0)
					{
						stripeWidthCounter = 0;
						stripeColor = 0;
						lastStripeColor = 0;
					}

					if(stripeWidthCounter == stripeWidth)
					{
						stripeWidthCounter = 0;
						if(lastStripeColor == stripeColor)
							stripeColor = ((stripeColor == 0) ? 255 : 0);
						else
							lastStripeColor = stripeColor;
					}
					stripeWidthCounter++;
				}
				
				imgPtr[x] = stripeColor;
			}
		}
		
		multiShot[i] = image;
	}
}

void StripePattern::generatePhaseShift()
{
	shotCount = shiftCount;
	multiShot = new IplImage*[shotCount];
	
	int stripeLenght = ((orientation == HORIZONTAL) ? (height / stripeCount) : (width / stripeCount));	
	
	float shift;
	float theta;
	switch(shiftCount)
	{
		case 3:
			shift = _2PI / 3.0f;
			theta = -_2PI / 3.0f;
			break;

		case 4:
			shift = _PI / 2.0f;
			theta = 0;
			break;

		case 5:
			shift = _PI / 2.0f;
			theta = -_PI;
			break;
	}

	float color;	
	IplImage* image;
	for(int i=0; i<shotCount; i++)
	{
		image = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);

		for(int y=0; y<height; y++)
		{
			if(orientation == HORIZONTAL)
			{
				color = 0.5f + 0.5f*cos(_2PI*(float)(y%stripeLenght)/(float)stripeLenght + theta);
			}

			float* imgPtr = (float*)(image->imageData + y*image->widthStep);
			for(int x=0; x<width; x++)
			{
				if(orientation == VERTICAL)
				{
					color = 0.5f + 0.5f*cos(_2PI*(float)(x%stripeLenght)/(float)stripeLenght + theta);
				}

				imgPtr[x] = color;
			}
		}

		theta += shift;

		multiShot[i] = image;
	}
}

void StripePattern::generateHybrid()
{
	shotCount = (int)(log((double)stripeCount)/log(2.0)) + shiftCount;
	multiShot = new IplImage*[shotCount];

	int stripeWidth = (orientation == HORIZONTAL ? height : width);
	int stripeLenght = ((orientation == HORIZONTAL) ? (height / stripeCount) : (width / stripeCount));	
	
	float shift;
	float theta;
	switch(shiftCount)
	{
		case 3:
			shift = _2PI / 3.0f;
			theta = -_2PI / 3.0f;
			break;

		case 4:
			shift = _PI / 2.0f;
			theta = 0;
			break;

		case 5:
			shift = _PI / 2.0f;
			theta = -_PI;
			break;
	}

	float color;
	int stripeWidthCounter;
	uchar stripeColor;
	uchar lastStripeColor;
	IplImage* image;
	for(int i=0; i<shotCount; i++)
	{
		if(i<shotCount - shiftCount) //GRAY CODE
		{
			image = cvCreateImage(cvSize(width, height), IPL_DEPTH_8U, 1);
			stripeWidth = (int)(stripeWidth/2);
			stripeWidthCounter = 0;
			stripeColor = 0;
			lastStripeColor = 0;

			for(int y=0; y<height; y++)
			{
				if(orientation == HORIZONTAL)
				{	
					if(y == 0)
					{
						stripeWidthCounter = 0;
						stripeColor = 0;
						lastStripeColor = 0;
					}
					
					if(stripeWidthCounter == stripeWidth)
					{
						stripeWidthCounter = 0;
						if(lastStripeColor == stripeColor)
							stripeColor = ((stripeColor == 0) ? 255 : 0);
						else
							lastStripeColor = stripeColor;
					}
					stripeWidthCounter++;
				}

				uchar* imgPtr = (uchar*)(image->imageData + y*image->widthStep);
				for(int x=0; x<width; x++)
				{
					if(orientation == VERTICAL)
					{					
						if(x == 0)
						{
							stripeWidthCounter = 0;
							stripeColor = 0;
							lastStripeColor = 0;
						}

						if(stripeWidthCounter == stripeWidth)
						{
							stripeWidthCounter = 0;
							if(lastStripeColor == stripeColor)
								stripeColor = ((stripeColor == 0) ? 255 : 0);
							else
								lastStripeColor = stripeColor;
						}
						stripeWidthCounter++;
					}
					
					imgPtr[x] = stripeColor;
				}
			}
		}
		else //PHASE SHIFT
		{
			image = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 1);
			
			for(int y=0; y<height; y++)
			{
				if(orientation == HORIZONTAL)
				{
					color = 0.5f + 0.5f*cos(_2PI*(float)(y%stripeLenght)/(float)stripeLenght + theta);
				}

				float* imgPtr = (float*)(image->imageData + y*image->widthStep);
				for(int x=0; x<width; x++)
				{
					if(orientation == VERTICAL)
					{
						color = 0.5f + 0.5f*cos(_2PI*(float)(x%stripeLenght)/(float)stripeLenght + theta);
					}

					imgPtr[x] = color;
				}
			}

			theta += shift;
		}
		
		multiShot[i] = image;
	}
}

void StripePattern::generateRainbow()
{
	shotCount = 1;
	singleShot = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 3);

	float r, g, b;
	for(int y=0; y<height; y++)
	{
		if(orientation == HORIZONTAL)
		{
			r = max((float)(height - 2*y)/(float)height, 0);
			g = ((y < height/2) ? ((float)(2*y)/(float)height) : ((float)(2*(height - y))/(float)height));
			b = max((float)(2*y - height)/(float)height, 0);
		}

		float* imgPtr = (float*)(singleShot->imageData + y*singleShot->widthStep);
		for(int x=0; x<width; x++)
		{
			if(orientation == VERTICAL)
			{
				r = max((float)(width - 2*x)/(float)width, 0);
				g = ((x < width/2) ? ((float)(2*x)/(float)width) : ((float)(2*(width - x))/(float)width));
				b = max((float)(2*x - width)/(float)width, 0);
			}
			
			imgPtr[3*x+2] = r;
			imgPtr[3*x+1] = g;
			imgPtr[3*x] = b;
		}
	}
}

void StripePattern::generateContinuouslyVaryingColorCode()
{
	shotCount = 1;
	singleShot = cvCreateImage(cvSize(width, height), IPL_DEPTH_32F, 3);
	
	int stripeLenght = ((orientation == HORIZONTAL) ? (height / stripeCount) : (width / stripeCount));

	float r, g, b;
	for(int y=0; y<height; y++)
	{
		if(orientation == HORIZONTAL)
		{
			r = 0.5f + 0.5f*cos(_2PI*(float)(y%stripeLenght)/(float)stripeLenght - _2PI3);
			g = 0.5f + 0.5f*cos(_2PI*(float)(y%stripeLenght)/(float)stripeLenght);
			b = 0.5f + 0.5f*cos(_2PI*(float)(y%stripeLenght)/(float)stripeLenght + _2PI3);
		}

		float* imgPtr = (float*)(singleShot->imageData + y*singleShot->widthStep);
		for(int x=0; x<width; x++)
		{
			if(orientation == VERTICAL)
			{
				r = 0.5f + 0.5f*cos(_2PI*(float)(x%stripeLenght)/(float)stripeLenght - _2PI3);
				g = 0.5f + 0.5f*cos(_2PI*(float)(x%stripeLenght)/(float)stripeLenght);
				b = 0.5f + 0.5f*cos(_2PI*(float)(x%stripeLenght)/(float)stripeLenght + _2PI3);
			}

			imgPtr[3*x+2] = r;
			imgPtr[3*x+1] = g;
			imgPtr[3*x] = b;
		}
	}
}
