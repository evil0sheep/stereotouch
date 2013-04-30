#ifndef _FRAME_PROCESSOR_H_
#define _FRAME_PROCESSOR_H_

#include <string>
#include <opencv2/opencv.hpp>
#include <stdio.h>

using namespace cv;

class FrameProcessor{
public:
	Mat M1, M2, D1, D2, R, T, R1, R2, P1, P2, Q, rmap[2][2];
	StereoBM BlockMatcher;

	FrameProcessor(std::string intrinsicsFile, std::string extrinsicsFile, Size frameSize);
	void processStereoFrame(const Mat & frameL, const Mat & frameR, Mat & pointCloud);
};
#endif