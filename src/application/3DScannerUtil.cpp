#include "3DScannerUtil.h"

CvCapture* createCamCapture()
{
    CvCapture* capture = cvCaptureFromCAM(-1);
	IplImage* image = NULL;
	if(capture)
	{
		image = cvQueryFrame( capture );
		if(!image)
		{
			cvReleaseCapture(&capture);
			return NULL;
		}
		
		image = cvQueryFrame( capture );
		cvSetImageCOI(image, 1);
		if(cvCountNonZero(image)==0)
		{
			cvSetImageCOI(image, 2);
			if(cvCountNonZero(image)==0)
			{
				cvSetImageCOI(image, 3);
				if(cvCountNonZero(image)==0)
				{
					cvSetImageCOI(image, 0);
					cvReleaseCapture(&capture);

					capture = cvCaptureFromCAM(0);
					if(!capture)
						return NULL;

					image = cvQueryFrame( capture );
					if(!image)
					{
						cvReleaseCapture(&capture);
						return NULL;
					}
				}
				else
				{
					cvSetImageCOI(image, 0);
				}
			}
			else
			{
				cvSetImageCOI(image, 0);
			}
		}
		else
		{
			cvSetImageCOI(image, 0);
		}
	}
	else
	{
		capture = cvCaptureFromCAM(0);
		if(!capture)
			return NULL;

		image = cvQueryFrame( capture );
		if(!image)
		{
			cvReleaseCapture(&capture);
			return NULL;
		}
	}

	return capture;
}

bool showImageNoFrame(const char* window, const IplImage* image, int x, int y)
{
//	if(FindWindowA(0, window))
//	{
//		cvShowImage(window, image);
//		return true;
//	}
//	else
//	{
//		cvNamedWindow(window, CV_WINDOW_AUTOSIZE);
//		cvMoveWindow(window, x, y);
//		cvSetWindowProperty(window, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);

//		cvShowImage(window, image);

//		HWND win_handle = FindWindowA(0, window);
//		if (!win_handle)
//			return false;

//		unsigned int flags = (SWP_SHOWWINDOW | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER);
//		flags &= ~SWP_NOSIZE;
//		int w = image->width;
//		int h = image->height;
//		SetWindowPos(win_handle, HWND_NOTOPMOST, x, y, w, h, flags);

//		SetWindowLong(win_handle, GWL_STYLE, GetWindowLong(win_handle, GWL_EXSTYLE) | WS_EX_TOPMOST);
//		ShowWindow(win_handle, SW_SHOW);

//		return true;
//	}

    return true;
}
