#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include "calibration.h"

using namespace cv;
using namespace std;

vector<Point2f> slidingWindow(Mat image, Rect window)
{
	vector<Point2f> points;
	const Size imgSize = image.size(); //CV8UC1
	bool shouldBreak = false;
	Mat timg = Mat::zeros(image.size().height, image.size().width, CV_8UC1);
	Mat result = Mat::zeros(image.size().height, image.size().width, CV_8UC1);

	while (true)
	{
		float currentX = window.x + window.width * 0.5f;

		Mat roi = image(window); //Extract region of interest
		vector<Point2f> locations;

		findNonZero(roi, locations); //Get all non-black pixels. All are white in our case

		float avgX = 0.0f;

		for (int i = 0; i < locations.size(); ++i) //Calculate average X position
		{
			float x = locations[i].x;
			avgX += window.x + x;
		}

		avgX = locations.empty() ? currentX : avgX / locations.size();
		
		Point point(avgX, window.y + window.height * 0.5f);
		points.push_back(point);

		// if pixel of loaction point is zero, delete that point
		if (locations.empty()) {
			points.pop_back();
		}

		// drawing windows
		/*
		Mat DrawResultHere = image.clone();
		Mat DrawResultGrid = image.clone();
		// Draw only rectangle
		rectangle(DrawResultHere, window, Scalar(255), 1, 8, 0);
		// Draw grid
		rectangle(DrawResultGrid, window, Scalar(255), 1, 8, 0);

		
		// Show  rectangle
		imshow("Step 2 draw Rectangle", DrawResultHere);
		result = timg + DrawResultHere;
		timg = result.clone();
		waitKey(100);
		
		// Select windows roi
		Mat Roi = image(window);

		//Show ROI
		imshow("Step 4 Draw selected Roi", Roi);
		waitKey(100);
		*/
		//Move the window up
		window.y -= window.height;

		//For the uppermost position
		if (window.y < 0)
		{
			window.y = 0;
			shouldBreak = true;
		}

		//Move x position
		window.x += (point.x - currentX);

		//Make sure the window doesn't overflow, we get an error if we try to get data outside the matrix
		if (window.x < 0) {
			window.x = 0;
		}
		if (window.x + window.width >= imgSize.width) {
			window.x = imgSize.width - window.width - 1;
		}
			
		if (shouldBreak) {
			//imshow("result", result);
			//waitKey(0);
			break;
		}
			
	}

	return points;
}
Mat Roi(Mat& image, Mat& edge) {
	// making zeros matrix for roi
	Mat mask = Mat::zeros(image.rows, image.cols, CV_8UC1); // CV_8UC3 to make it a 3 channel

	// trapezoid points of roi
	Point mask_points[1][4];

	mask_points[0][0] = Point(50, image.size().height);
	mask_points[0][1] = Point(image.size().width / 2 - 45, image.size().height / 2 + 60);
	mask_points[0][2] = Point(image.size().width / 2 + 45, image.size().height / 2 + 60);
	mask_points[0][3] = Point(image.size().width - 50, image.size().height);

	const Point* ppt[1] = { mask_points[0] }; 
	int npt[] = { 4 }; // points of trapezoid
	fillPoly(mask, ppt, npt, 1, Scalar(255, 255, 255), 8);

	//imshow("Mask", mask);
	//waitKey(0);


	Mat roi = edge.clone();
	bitwise_and(edge, mask, roi); // masking the original image with roi

	//imshow("ROI", roi);
	//waitKey(0);
	return roi;
}

void fillter_colors(Mat image, Mat& img_filtered) {
	Mat img_bgr = image.clone();
	Mat img_hsv, img_combine;
	Mat white_mask, white_image;
	Mat yellow_mask, yellow_image;

	Scalar lower_white = Scalar(200, 200, 200);
	Scalar upper_white = Scalar(255, 255, 255);
	Scalar lower_yellow = Scalar(10, 100, 100);
	Scalar upper_yellow = Scalar(40, 255, 255);

	//Filter white pixels
	inRange(img_bgr, lower_white, upper_white, white_mask);
	bitwise_and(img_bgr, img_bgr, white_image, white_mask);


	//Filter yellow pixels( Hue 30 )
	cvtColor(img_bgr, img_hsv, COLOR_BGR2HSV);


	inRange(img_hsv, lower_yellow, upper_yellow, yellow_mask);
	bitwise_and(img_bgr, img_bgr, yellow_image, yellow_mask);


	//Combine the two above images
	addWeighted(white_image, 1.0, yellow_image, 1.0, 0.0, img_combine);

	img_filtered = img_combine.clone();
	//imshow("filtered image", img_filtered);
	//waitKey(0);
}

int main()
{
	const String filename = "solidWhiteCurve.jpg";

	//points of original image for bird eye view.
	Point2f srcVertices[4];


	srcVertices[0] = Point(960 / 2 - 45, 540 / 2 + 60);
	srcVertices[1] = Point(960 / 2 + 45, 540 / 2 + 60);
	srcVertices[2] = Point(960 - 50, 540);
	srcVertices[3] = Point(50, 540);


	//Destination vertices. Output is 640 x 480
	Point2f dstVertices[4];
	dstVertices[0] = Point(0, 0);
	dstVertices[1] = Point(640, 0);
	dstVertices[2] = Point(640, 480);
	dstVertices[3] = Point(0, 480);

	//Prepare matrix for transform and get the warped image
	Mat perspectiveMatrix = getPerspectiveTransform(srcVertices, dstVertices);
	Mat dst(480, 640, CV_8UC3); //Destination for warped image

	//For transforming back into original image space
	Mat invertedPerspectiveMatrix;
	invert(perspectiveMatrix, invertedPerspectiveMatrix);


	Mat org; //Original image, modified only with result
	Mat img; //Working image

	// video
	
	VideoCapture cap("challenge.mp4");
	if (!cap.isOpened()) {
		cerr << "fail to load video" << endl;
		return -1;
	}
	cout << "Frame count : " << cvRound(cap.get(CAP_PROP_FRAME_COUNT)) << endl;
	int w = cvRound(cap.get(CAP_PROP_FRAME_WIDTH));
	int h = cvRound(cap.get(CAP_PROP_FRAME_HEIGHT));
	double fps = cap.get(CAP_PROP_FPS);
	int fourcc = VideoWriter::fourcc('D', 'I', 'V', 'X');
	cout << "FPS : " << fps << endl;
	int delay = cvRound(1000 / fps);
	VideoWriter outputvideo("birdeye.avi", fourcc, fps, Size(w, h));

	if (!outputvideo.isOpened()) {
		cout << "file open failed" << endl;
		return -1;
	}
	Mat image;
	while (true) {
		cap >> org;
		if (org.empty()) {
			break;
		}
		org = simple_calib(org);
		
		org.copyTo(img);
		//filtering color
		Mat img_fil;
		fillter_colors(img, img_fil);
		

		// change image to binary 
		Mat gray, binary;
		cvtColor(img_fil, gray, COLOR_BGR2GRAY);
		threshold(gray, binary, 0, 255, THRESH_BINARY | THRESH_OTSU);
		

		Mat roi_img = Roi(org, binary);
		
		roi_img.copyTo(img);
		//Generate bird's eye view
		warpPerspective(img, dst, perspectiveMatrix, dst.size(), INTER_LINEAR, BORDER_CONSTANT);
		
		Mat processed;
		dst.copyTo(processed);

		vector<Point2f> pts = slidingWindow(processed, Rect(0, 420, 120, 60)); // 480 to 420
		vector<Point> allPts; //Used for the end polygon at the end.

		vector<Point2f> outPts;
		perspectiveTransform(pts, outPts, invertedPerspectiveMatrix); //Transform points back into original image space

		//Draw the points onto the out image
		for (int i = 0; i < outPts.size() - 1; ++i)
		{
			line(org, outPts[i], outPts[i + 1], Scalar(255, 0, 0), 3);
			allPts.push_back(Point(outPts[i].x, outPts[i].y));
		}

		allPts.push_back(Point(outPts[outPts.size() - 1].x, outPts[outPts.size() - 1].y));


		Mat out;
		cvtColor(processed, out, COLOR_GRAY2BGR); //Conver the processing image to color so that we can visualise the lines

		//Draw a line on the processed image
		for (int i = 0; i < pts.size() - 1; ++i) {
			line(out, pts[i], pts[i + 1], Scalar(255, 0, 0));
		}
			
		//Sliding window for the right side
		pts = slidingWindow(processed, Rect(520, 420, 120, 60));
		perspectiveTransform(pts, outPts, invertedPerspectiveMatrix);

		//Draw the other lane and add points
		for (int i = 0; i < outPts.size() - 1; ++i)
		{
			line(org, outPts[i], outPts[i + 1], Scalar(0, 0, 255), 3);
			allPts.push_back(Point(outPts[outPts.size() - i - 1].x, outPts[outPts.size() - i - 1].y));
		}

		allPts.push_back(Point(outPts[0].x - (outPts.size() - 1), outPts[0].y));

		// drawing line on bird eye view
		for (int i = 0; i < pts.size() - 1; ++i) {
			line(out, pts[i], pts[i + 1], Scalar(0, 0, 255));
		}
			
		//Create a green-ish overlay
		vector<vector<Point>> arr;
		arr.push_back(allPts);
		Mat overlay = Mat::zeros(org.size(), org.type());
		fillPoly(overlay, arr, Scalar(0, 255, 100));
		addWeighted(org, 1, overlay, 0.5, 0, org); //Overlay it

		//Show results
		//imshow("Preprocess", out);
		//waitKey(0);
		imshow("src", org);
		//waitKey(0);

		outputvideo << org;
		if (waitKey(1) == 27) {
			break;
		}
	}
	
	destroyAllWindows();
	return 0;
}