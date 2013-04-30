#include "FrameProcessor.hpp"

FrameProcessor::FrameProcessor(std::string intrinsicsFile, std::string extrinsicsFile, Size frameSize){
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
    extrinsics["Q"] >> Q;

    intrinsics.release();
    extrinsics.release();

    initUndistortRectifyMap(M1, D1, R1, P1, frameSize, CV_16SC2, rmap[0][0], rmap[0][1]);
    initUndistortRectifyMap(M2, D2, R2, P2, frameSize, CV_16SC2, rmap[1][0], rmap[1][1]);

    // BlockMatcher.state->preFilterType = CV_STEREO_BM_XSOBEL; //CV_STEREO_BM_NORMALIZED_RESPONSE;
    // BlockMatcher.state->preFilterSize = 9;
     BlockMatcher.state->preFilterCap = 45;
     BlockMatcher.state->SADWindowSize = 31;
    // BlockMatcher.state->minDisparity = 0;
     BlockMatcher.state->numberOfDisparities =  128;
    // BlockMatcher.state->textureThreshold = 10;
    // BlockMatcher.state->uniquenessRatio = 15;
     //BlockMatcher.state->speckleRange = 60;
     //BlockMatcher.state->speckleWindowSize = 20;
    // BlockMatcher.state->trySmallerWindows = 0;
    // BlockMatcher.state->roi1 = BlockMatcher.state->roi2 = cvRect(160,120,480,360);
    // BlockMatcher.state->disp12MaxDiff = -1;
}

void FrameProcessor::processStereoFrame(const Mat & frameL, const Mat & frameR, Mat & pointCloud){   

	Mat disparityMap, disparityMapNormalized;
    Mat frameTransposedL, frameTransposedR, frameRemappedL, frameRemappedR, frameGrayscaleL, frameGrayscaleR;     

    Mat rotMatL = cvCreateMat(2,3,CV_32FC1);
    Mat rotMatR = cvCreateMat(2,3,CV_32FC1);
    // Compute rotation matrix
    CvPoint2D32f centerL = cvPoint2D32f( frameL.cols/2, frameL.rows/2 );
    rotMatL = getRotationMatrix2D( centerL, 90, 1 );

    CvPoint2D32f centerR = cvPoint2D32f( frameR.cols/2, frameR.rows/2 );
    rotMatR = getRotationMatrix2D( centerR, 90, 1 );

    warpAffine(frameL, frameTransposedL, rotMatL, frameL.size() );
    warpAffine(frameR, frameTransposedR, rotMatR, frameR.size() );




    //transpose(frameL, frameTransposedL);
    //transpose(frameR, frameTransposedR);


    remap(frameTransposedL, frameRemappedL, rmap[0][0], rmap[0][1], CV_INTER_LINEAR);
    remap(frameTransposedR, frameRemappedR, rmap[1][0], rmap[1][1], CV_INTER_LINEAR);

    //imshow("LiveFeedL",frameTransposedL);
   //imshow("LiveFeedR",frameTransposedR);

    cvtColor(frameRemappedL, frameGrayscaleL, CV_RGB2GRAY);
    cvtColor(frameRemappedR, frameGrayscaleR, CV_RGB2GRAY);


    BlockMatcher( frameGrayscaleL, frameGrayscaleR, disparityMap, CV_32F);
    normalize(disparityMap, disparityMapNormalized, 0, 255, CV_MINMAX, CV_8U);

    imshow("Disparity", disparityMapNormalized);

    reprojectImageTo3D( disparityMap, pointCloud, Q, false);
}

