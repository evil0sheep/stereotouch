#include <opencv2/opencv.hpp>
#include <stdio.h>
#define MAX_FRAME_DT (1000.0 / 30)

 
using namespace cv;



int main()
{
   printf("begin\n");
    Mat frameL, frameR;
    double timestampL,timestampR;

    VideoCapture capL(2); 
    VideoCapture capR(1); 
    if(!capL.isOpened() || !capR.isOpened() ){
        fprintf(stderr, "Failed to open stereo capture device\n");
        return -1;
    }  // check if we succeeded
        

    capL >> frameL;
    capR >> frameR;

    
    Size combinedSize(frameL.cols + frameR.cols, frameR.rows);
    VideoWriter output("output.mpeg", CV_FOURCC('P','I','M','1'), 30.0, combinedSize);
    Mat frameCombined(combinedSize, frameL.type());

    
   
    cvNamedWindow("LiveFeed",CV_WINDOW_AUTOSIZE);


    frameL = Mat();
    frameR = Mat();
    while(1)
    {
        //printf("test value = %f\n", capL.get(propID));
       
        //Load the next frame
        capL.grab();
        capR.grab();
        timestampL = capL.get(CV_CAP_PROP_POS_MSEC);
        timestampR = capR.get(CV_CAP_PROP_POS_MSEC);

        while (abs(timestampL - timestampR) > MAX_FRAME_DT){
            if (timestampR > timestampL) {
                capL.grab();
                timestampL = capL.get(CV_CAP_PROP_POS_MSEC);
            } else {
                capR.grab();
                timestampR = capR.get(CV_CAP_PROP_POS_MSEC);
            }
            //printf("timestamp difference = %f\n", timestampL - timestampR);
        }
       // 

        if(!(
            capL.retrieve(frameL) &&
            capR.retrieve(frameR))){

            fprintf(stderr, "failed to capture stereo frame\n");
            break;
        }


        frameL.copyTo(frameCombined(Rect(0,0,frameL.cols, frameL.rows)));
        frameR.copyTo(frameCombined(Rect(frameL.cols, 0,  frameR.cols, frameR .rows)));

        imshow("LiveFeed",frameCombined);
        
        output << frameCombined;
        
        char c = cvWaitKey(10);
        //If the key pressed by user is Esc(ASCII is 27) then break out of the loop

        if(c == 'q')
           break;

        if(c == 'x'){
            VideoCapture temp = capL;
            capL = capR;
            capR = temp;
        }
           
    }
    //CleanUp

    cvDestroyAllWindows();
}