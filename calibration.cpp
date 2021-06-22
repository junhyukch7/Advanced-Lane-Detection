#include <opencv2/opencv.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <stdio.h>
#include <iostream>

using namespace cv;
int CHECKERBOARD[2]{ 6,9 };
Mat calib(Mat& org) {
	// 처음부터 끝까지 cailbration과정 수행
	// Creating vector to store vectors of 3D points for each checkerboard image
	std::vector<std::vector<cv::Point3f> > objpoints;

	// Creating vector to store vectors of 2D points for each checkerboard image
	std::vector<std::vector<cv::Point2f> > imgpoints;

	// Defining the world coordinates for 3D points
	std::vector<cv::Point3f> objp;
	for (int i{ 0 }; i < CHECKERBOARD[1]; i++)
	{
		for (int j{ 0 }; j < CHECKERBOARD[0]; j++)
			objp.push_back(cv::Point3f(j, i, 0));
	}


	// Extracting path of individual image stored in a given directory
	std::vector<cv::String> images;
	// Path of the folder containing checkerboard images
	std::string path = "camera_cal/calibration*.jpg";

	cv::glob(path, images);

	cv::Mat frame, gray;
	// vector to store the pixel coordinates of detected checker board corners 
	std::vector<cv::Point2f> corner_pts;
	bool success;

	// Looping over all the images in the directory
	for (int i{ 0 }; i < images.size(); i++)
	{
		frame = cv::imread(images[i]);
		cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);

		// Finding checker board corners
		// If desired number of corners are found in the image then success = true  
		success = cv::findChessboardCorners(gray, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FAST_CHECK | CALIB_CB_NORMALIZE_IMAGE);

		/*
		 * If desired number of corner are detected,
		 * we refine the pixel coordinates and display
		 * them on the images of checker board
		*/
		if (success)
		{
			cv::TermCriteria criteria(TermCriteria::EPS | TermCriteria::MAX_ITER, 30, 0.001);

			// refining pixel coordinates for given 2d points.
			cv::cornerSubPix(gray, corner_pts, cv::Size(11, 11), cv::Size(-1, -1), criteria);

			// Displaying the detected corner points on the checker board
			cv::drawChessboardCorners(frame, cv::Size(CHECKERBOARD[0], CHECKERBOARD[1]), corner_pts, success);

			objpoints.push_back(objp);
			imgpoints.push_back(corner_pts);
		}

		cv::imshow("Image", frame);
		cv::waitKey(0);
	}

	cv::destroyAllWindows();

	cv::Mat cameraMatrix, distCoeffs, R, T;

	/*
	 * Performing camera calibration by
	 * passing the value of known 3D points (objpoints)
	 * and corresponding pixel coordinates of the
	 * detected corners (imgpoints)
	*/
	cv::calibrateCamera(objpoints, imgpoints, cv::Size(gray.rows, gray.cols), cameraMatrix, distCoeffs, R, T);

	std::cout << "cameraMatrix : " << cameraMatrix << std::endl;
	std::cout << "distCoeffs : " << distCoeffs << std::endl;
	std::cout << "Rotation vector : " << R << std::endl;
	std::cout << "Translation vector : " << T << std::endl;

	Mat dst = Mat::zeros(org.size().height, org.size().width, CV_8UC3);

	undistort(org, dst, cameraMatrix, distCoeffs);

	return dst;

}

Mat simple_calib(Mat& org) {
	//calib()함수에서 얻은 데이터를 기반으로 간단하게 왜곡보정
	Mat cameraMatrix = (Mat1d(3, 3) << 1156.915062142131, 0, 665.9387119551399, 0, 1152.114644805994, 388.7955240062732, 0, 0, 1);
	Mat distCoeffs = (Mat1d(1, 5) << -0.2371543906423672, -0.08871932875028064, -0.0007898871023714299, -0.0001171743090811421,0.1119047389586262);

	Mat dst = Mat::zeros(org.size().height, org.size().width, CV_8UC3);

	undistort(org, dst, cameraMatrix, distCoeffs);

	return dst;
}