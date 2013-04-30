#include "StereoCapture.hpp"

StereoCapture::~StereoCapture(){}

StereoCaptureCam::StereoCaptureCam(int camIndex0, int camIndex1)
	:capL(camIndex0), capR(camIndex1){
	if(!capL.isOpened() || !capR.isOpened() ){
        fprintf(stderr, "Failed to open stereo capture device\n");
        exit(-1);
    } 

}
StereoCaptureFile::StereoCaptureFile(std::string filename)
	: capture(filename){}


bool StereoCaptureCam::getStereoFrame(Mat & frameL, Mat & frameR){
	capL.grab();
    capR.grab();
    float timestampL = capL.get(CV_CAP_PROP_POS_MSEC);
    float timestampR = capR.get(CV_CAP_PROP_POS_MSEC);

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

    if(!(capL.retrieve(frameL) &&
        capR.retrieve(frameR))){

        fprintf(stderr, "failed to capture stereo frame\n");
        return false;
    }
	return true;
}

bool StereoCaptureFile::getStereoFrame(Mat & frameL, Mat & frameR){
	Mat frame;
	capture >> frame;

	frameL = frame(Range(0, frame.rows), Range(0, frame.cols/2));
    frameR = frame(Range(0, frame.rows), Range(frame.cols/2, frame.cols));
	return true;
}