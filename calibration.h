#pragma once
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

Mat calib(Mat& org);
Mat simple_calib(Mat& org);