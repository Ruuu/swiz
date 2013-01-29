#ifndef UTILS_H
#define UTILS_H

#include <opencv/cv.h>

class Utils
{
    public:
       Utils() {}

       static CvCapture * videoCapture()
       {
           cv::Mat imgCam1;
           cv::VideoCapture cap1(0); //albo -1 albo 1 jako argument
           cap1 >> imgCam1;

           return
       }

       static CvCapture * cameraCapture()
       {
           //CvCapture * capture = cvCaptureFromCAM(CV_CAP_ANY);
           CvCapture * capture = cvCreateCameraCapture( -1 );
           IplImage * image = NULL;
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
};

#endif // UTILS_H
