#pragma once

#ifndef _3D_SCANNER_UTIL_H
#define _3D_SCANNER_UTIL_H

#include "cv.h"
#include "cxcore.h"
#include "highgui.h"

CvCapture* createCamCapture();

bool showImageNoFrame(const char* window, const IplImage* image, int x = 0, int y = 0);

#endif
