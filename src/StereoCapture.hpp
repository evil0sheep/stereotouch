#ifndef _STEREO_CAPTURE_H_
#define _STEREO_CAPTURE_H_

#define MAX_FRAME_DT (1000.0 / 30)

#include <string>
#include <opencv2/opencv.hpp>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;

class StereoCapture{
public:
	virtual ~StereoCapture();
	virtual bool getStereoFrame(Mat & frameL, Mat & frameR) = 0;

};

class StereoCaptureCam : public StereoCapture{
public:
	VideoCapture capL, capR;
	StereoCaptureCam(int camIndex0, int camIndex1);
	virtual bool getStereoFrame(Mat  & frameL, Mat & frameR);

};

class StereoCaptureFile : public StereoCapture{
public:
	VideoCapture capture;
	StereoCaptureFile(std::string filename);
	virtual bool getStereoFrame(Mat & frameL, Mat & frameR);

};

#endif