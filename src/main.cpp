#include <opencv2/opencv.hpp>
#include <stdio.h>
#include "StereoCapture.hpp"
#include "FrameProcessor.hpp"

 
using namespace cv;



int main()
{
    Mat frameL, frameR, pointCloudMat;
    int imageIndex = 0;
    char imageNameL[256], imageNameR[256];

    //StereoCapture *capture = new StereoCaptureCam(2, 1);
    StereoCapture *capture = new StereoCaptureFile("media/video/test1.mpeg");

    if(!(capture->getStereoFrame(frameL, frameR))) exit(-1);

    FrameProcessor *processor = new FrameProcessor("device/camera_properties/intrinsics.yml", "device/camera_properties/extrinsics.yml", Size(frameL.cols, frameL.rows));


    //Window to show livefeed
    cvNamedWindow("LiveFeedL",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("LiveFeedR",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Disparity",CV_WINDOW_AUTOSIZE);



    while(1)
    {
        if(!(capture->getStereoFrame(frameL, frameR))){
            fprintf(stderr, "Failed to capture stereo frame\n");
            exit(-1);
        }
        processor->processStereoFrame(frameL, frameR, pointCloudMat);


        //Escape Sequence
        char c = cvWaitKey(10);
        //If the key pressed by user is Esc(ASCII is 27) then break out of the loop

        if(c == 'q')
           break;

       const char *imageOutputDirectory = "media/stereo_output";
        if(c == ' '){
            imageIndex ++;
            sprintf(imageNameL, "%s/%.4dleft.png", imageOutputDirectory, imageIndex);
            sprintf(imageNameR, "%s/%.4dright.png", imageOutputDirectory, imageIndex);
            printf("writing images:\n\t%s\n\t%s\n", imageNameL, imageNameR);
            imwrite(imageNameL, frameL);
            imwrite(imageNameR, frameR);
        }

        if(c == 'x'){

        }
           
    }
    //CleanUp
    delete capture;
    delete processor;
    cvDestroyAllWindows();
}