#include <opencv2/opencv.hpp>
#include <stdio.h>
#define MAX_FRAME_DT (1000.0 / 30)

 
using namespace cv;



int main()
{
    Mat disparityMap, disparityMap8bit;
    Mat frameL, frameR, frameUnRotatedL, frameUnRotatedR, frameRemappedL, frameRemappedR, frameGrayscaleL, frameGrayscaleR;
    double timestampL,timestampR;
    int imageIndex = 0;
    char imageNameL[256], imageNameR[256];

    StereoBM BlockMatcher;
    // BlockMatcher.state->preFilterType = CV_STEREO_BM_XSOBEL; //CV_STEREO_BM_NORMALIZED_RESPONSE;
    // BlockMatcher.state->preFilterSize = 9;
    // BlockMatcher.state->preFilterCap = 31;
     BlockMatcher.state->SADWindowSize = 31;
    // BlockMatcher.state->minDisparity = 0;
     BlockMatcher.state->numberOfDisparities =  64;
    // BlockMatcher.state->textureThreshold = 10;
    // BlockMatcher.state->uniquenessRatio = 15;
     //BlockMatcher.state->speckleRange = 60;
     //BlockMatcher.state->speckleWindowSize = 20;
    // BlockMatcher.state->trySmallerWindows = 0;
    // BlockMatcher.state->roi1 = BlockMatcher.state->roi2 = cvRect(160,120,480,360);
    // BlockMatcher.state->disp12MaxDiff = -1;



    VideoCapture capL(1); 
    VideoCapture capR(2); 
    if(!capL.isOpened() || !capR.isOpened() ){
        fprintf(stderr, "Failed to open stereo capture device\n");
        return -1;
    }  // check if we succeeded
        

    capL >> frameL;
    capR >> frameR;


    //laod camera matrices

    Mat M1, M2, D1, D2, R, T, R1, R2, P1, P2, Q, rmap[2][2];
    FileStorage intrinsics("device/camera_properties/intrinsics.yml", FileStorage::READ);
    FileStorage extrinsics("device/camera_properties/extrinsics.yml", FileStorage::READ);

    intrinsics["M1"] >> M1;
    intrinsics["M2"] >> M2;
    intrinsics["D1"] >> D1;
    intrinsics["D2"] >> D2;
    extrinsics["R1"] >> R1;
    extrinsics["R2"] >> R2;
    extrinsics["P1"] >> P1;
    extrinsics["P2"] >> P2;

    intrinsics.release();
    extrinsics.release();

    Size imageSize = Size(frameL.cols, frameL.rows);
    initUndistortRectifyMap(M1, D1, R1, P1, imageSize, CV_16SC2, rmap[0][0], rmap[0][1]);
    initUndistortRectifyMap(M2, D2, R2, P2, imageSize, CV_16SC2, rmap[1][0], rmap[1][1]);

    //Window to show livefeed
    cvNamedWindow("LiveFeedL",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("LiveFeedR",CV_WINDOW_AUTOSIZE);
    cvNamedWindow("Disparity",CV_WINDOW_AUTOSIZE);

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
            printf("timestamp difference = %f\n", timestampL - timestampR);
        }
       // 

        if(!(
            capL.retrieve(frameUnRotatedL) &&
            capR.retrieve(frameUnRotatedR))){

            fprintf(stderr, "failed to capture stereo frame\n");
            break;
        }
        frameL= Mat();
        warpAffine( frameUnRotatedL, frameL, rotMatL, frameL.size() );
        warpAffine( frameUnRotatedR, frameR, rotMatR, frameR.size() );
        
         remap(frameL, frameRemappedL, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
         remap(frameR, frameRemappedR, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);

        imshow("LiveFeedL",frameRemappedL);
        imshow("LiveFeedR",frameRemappedR);

         cvtColor(frameRemappedL, frameGrayscaleL, CV_RGB2GRAY);
         cvtColor(frameRemappedR, frameGrayscaleR, CV_RGB2GRAY);


         BlockMatcher( frameGrayscaleL, frameGrayscaleR, disparityMap, CV_32F);
         normalize(disparityMap, disparityMap8bit, 0, 255, CV_MINMAX, CV_8U);

        //Show the present frame

        imshow("Disparity", disparityMap8bit);
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
            imwrite(imageNameL, frameRemappedL);
            imwrite(imageNameR, frameRemappedR);
        }

        if(c == 'x'){
            VideoCapture temp = capL;
            capL = capR;
            capR = temp;
        }
           
    }
    //CleanUp

    cvDestroyAllWindows();
}