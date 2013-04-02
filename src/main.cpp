#include <opencv2/opencv.hpp>
#include <stdio.h>
#define MAX_FRAME_DT (1000.0 / 60)

 
using namespace cv;



int main()
{
    Mat frameL, frameR, frameLUnRotated, frameRUnRotated;
    double timestampL,timestampR;
    int imageIndex = 15;
    char imageNameL[256], imageNameR[256];

    VideoCapture capL(1); 
    VideoCapture capR(2); 
    if(!capL.isOpened() || !capR.isOpened() ){
        fprintf(stderr, "Failed to open stereo capture device\n");
        return -1;
    }  // check if we succeeded
        

    capL >> frameL;
    capR >> frameR;


    //Window to show livefeed
    cvNamedWindow("LiveFeedL",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("LiveFeedR",CV_WINDOW_AUTOSIZE);

    Mat rotMatL = cvCreateMat(2,3,CV_32FC1);
    Mat rotMatR = cvCreateMat(2,3,CV_32FC1);
    // Compute rotation matrix
    CvPoint2D32f centerL = cvPoint2D32f( frameL.cols/2, frameL.rows/2 );
    rotMatL = getRotationMatrix2D( centerL, 180, 1 );

    CvPoint2D32f centerR = cvPoint2D32f( frameR.cols/2, frameR.rows/2 );
    rotMatR = getRotationMatrix2D( centerR, 0, 1 );


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
        }
       // printf("timestamp difference = %f\n", timestampL - timestampR);

        if(!(
            capL.retrieve(frameLUnRotated) &&
            capR.retrieve(frameRUnRotated))){

            fprintf(stderr, "failed to capture stereo frame\n");
            break;
        }
        frameL= Mat();
        warpAffine( frameLUnRotated, frameL, rotMatL, frameL.size() );
        warpAffine( frameRUnRotated, frameR, rotMatR, frameR.size() );
        


        //Show the present frame
        imshow("LiveFeedL",frameL);
        imshow("LiveFeedR",frameR);
        //Escape Sequence
        char c=cvWaitKey(10);
        //If the key pressed by user is Esc(ASCII is 27) then break out of the loop

        if(c=='q')
           break;

       const char *imageOutputDirectory = "media/calibration";
        if(c==' '){
            imageIndex ++;
            sprintf(imageNameL, "%s/%.4dleft.png", imageOutputDirectory, imageIndex);
            sprintf(imageNameR, "%s/%.4dright.png", imageOutputDirectory, imageIndex);
            printf("writing images:\n\t%s\n\t%s\n", imageNameL, imageNameR);
            imwrite(imageNameL, frameL);
            imwrite(imageNameR, frameR);
        }
           
    }
    //CleanUp

    cvDestroyAllWindows();
}