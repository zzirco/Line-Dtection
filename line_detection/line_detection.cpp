#include <opencv2/opencv.hpp>
#include <iostream>
#include "Polarization_Split.h"

using namespace cv;
using namespace std;

Mat roi_background(Mat img_edges)
{
	int width = img_edges.cols;
	int height = img_edges.rows;
	Point points[6];

	points[0] = Point(150, 250);
	points[1] = Point(466, 250);
	points[2] = Point(615, 350);
	points[3] = Point(615, 513);
	points[4] = Point(0, 513);
	points[5] = Point(0, 350);

	Mat img_mask = Mat::zeros(img_edges.rows, img_edges.cols, CV_8UC1);
	const Point* ppt[1] = { points }; 
	int npt[] = { 6 }; 

	fillPoly(img_mask, ppt, npt, 1, Scalar(255, 255, 255), LINE_8);

	Mat img_masked;

	bitwise_and(img_edges, img_mask, img_masked); 
	return img_masked;
}
Mat roi_car(Mat img_edges)
{
	int width = img_edges.cols;
	int height = img_edges.rows;
	Point points[4];

	points[0] = Point(180, 400);
	points[1] = Point(435, 400);
	points[3] = Point(0, 513);
	points[2] = Point(615, 513);

	const Point* ppt[1] = { points }; 
	int npt[] = { 4 };

	fillPoly(img_edges, ppt, npt, 1, 0, LINE_8); 

	return img_edges; 
}
int main()
{
	VideoCapture cap("c:/T7/20220629_211820_exposure_Auto.mp4");
	if (!cap.isOpened())
	{
		cout << "ERROR" << endl;
		return 0;
	}

	while (true)
	{
		Mat Original, Gray, imgGray;

		cap >> Original;

		if (Original.empty())
		{
			cout << "video is end" << endl;
			break;
		}
		cvtColor(Original, Gray, CV_BGR2GRAY);

		//편광 분리

		Split Split(Gray);

		Mat cv_img, cv_img_0, cv_img_45, cv_img_90, cv_img_135;

		cv_img = Split.Original_Image();
		cv_img_0 = Split.Image_0();
		cv_img_45 = Split.Image_45();
		cv_img_90 = Split.Image_90();
		cv_img_135 = Split.Image_135();

		//이미지 크기 조절

		resize(cv_img_0, cv_img_0, Size(cv_img_0.cols / 2, cv_img_0.rows / 2));
		resize(cv_img_45, cv_img_45, Size(cv_img_45.cols / 2, cv_img_45.rows / 2));
		resize(cv_img_90, cv_img_90, Size(cv_img_90.cols / 2, cv_img_90.rows / 2));
		resize(cv_img_135, cv_img_135, Size(cv_img_135.cols / 2, cv_img_135.rows / 2));

		Mat frame = cv_img_0.clone();
		Mat Final = cv_img_0.clone();

		//감마 보정
		Mat src1, gamma_dst;
		double gamma = 0.5;

		src1 = cv_img_0.clone();
		Mat table(1, 256, CV_8U);
		uchar* p = table.ptr();
		for (int i = 0; i < 256; ++i)
			p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);

		LUT(src1, table, gamma_dst);

		//어파인 변환
		/*
		Point2f inputp[4];
		inputp[0] = Point2f(267,260);
		inputp[1] = Point2f(367, 260);
		inputp[2] = Point2f(609, 395);
		inputp[3] = Point2f(24, 395);

		Point2f outputp[4];
		outputp[0] = Point2f(0, 0);
		outputp[1] = Point2f(616, 0);
		outputp[2] = Point2f(616, 514);
		outputp[3] = Point2f(0, 514);

		Mat h = getPerspectiveTransform(inputp, outputp);

		Mat affine;
		warpPerspective(gamma_dst, affine, h, gamma_dst.size());
		//imshow("affine", affine);
		*/

		//적응적 이진화 테스트

		Mat adaptive, erosion;
		adaptiveThreshold(gamma_dst, adaptive, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 391, 5);
		//imshow("adaptive", adaptive);

		Mat element = getStructuringElement(MORPH_RECT, Size(3, 3), Point(-1, -1));

		//히스토그램
		vector<Mat> gray_planes;
		split(cv_img_0, gray_planes);
		vector<Mat> gray_planes2;
		split(gamma_dst, gray_planes2);

		int histSize = 256;
		float range[] = { 0, 256 };
		const float* histRange = { range };
		bool uniform = true, accumulate = false;
		Mat g_hist, g_hist2;

		calcHist(&gray_planes[0], 1, 0, Mat(), g_hist, 1, &histSize, &histRange, uniform, accumulate);
		calcHist(&gray_planes2[0], 1, 0, Mat(), g_hist2, 1, &histSize, &histRange, uniform, accumulate);

		int hist_w = cv_img_0.cols, hist_h = cv_img_0.rows;
		int bin_w = cvRound((double)hist_w / histSize);
		Mat histImage(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));
		Mat histImage2(hist_h, hist_w, CV_8UC3, Scalar(255, 255, 255));

		normalize(g_hist, g_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
		normalize(g_hist2, g_hist2, 0, histImage.rows, NORM_MINMAX, -1, Mat());

		for (int i = 0; i < 255; i++)
		{
			line(histImage, Point(bin_w * i, hist_h), Point(bin_w * i, hist_h - g_hist.at<float>(i)), Scalar(0, 0, 255));
			line(histImage2, Point(bin_w * i, hist_h), Point(bin_w * i, hist_h - g_hist2.at<float>(i)), Scalar(0, 0, 255));
		}

		//관심 영역 설정

		Mat g_frame = gamma_dst.clone();
		Mat blur, open, otsu, th;

		Size size = Size(5, 5);
		GaussianBlur(g_frame, blur, size, 0);

		Mat Roi = roi_background(g_frame);
		Roi = roi_car(Roi);

		//이진화
		
		int pixel_sum = 0, pixel_count = 0, pixel_average = 0;

		for (int i = 0; i < Roi.rows; i++)
			for (int j = 0; j < Roi.cols; j++)
			{
				if (Roi.at<uchar>(i, j) != 0)
				{
					pixel_sum += Roi.at<uchar>(i, j);
					pixel_count++;
				}
			}
		
		pixel_average = pixel_sum / pixel_count;
		
		Mat otsu_roi(Size(616, 514), CV_8UC1, pixel_average - 10);

		for (int i = 0; i < Roi.rows; i++)
			for (int j = 0; j < Roi.cols; j++)
			{
				if (Roi.at<uchar>(i, j) != 0)
				{
					otsu_roi.at<uchar>(i, j) = Roi.at<uchar>(i, j);
				}
			}
		
		threshold(otsu_roi, otsu, 0, 255, THRESH_BINARY | THRESH_OTSU);
		threshold(otsu_roi, th, 120, 255, THRESH_BINARY);
		morphologyEx(th, open, MORPH_OPEN, element);

		//erode(otsu, erosion, element);
		//imshow("erosion", erosion);

		//Canny 에지 검출

		Mat dst, cdst, otsu_dst;
		Canny(otsu_roi, dst, 100, 200);
		Canny(otsu, otsu_dst, 100, 200);

		cvtColor(dst, cdst, CV_GRAY2BGR);
		cvtColor(Final, Final, CV_GRAY2BGR);

		Mat Line(Final.rows, Final.cols, CV_8UC1, Scalar(0,0,0));

		//Hough 직선 검출

		vector<Vec4i> lines;
		HoughLinesP(dst, lines, 1, CV_PI / 180, 50, 130, 40);

		for (size_t i = 0; i < lines.size(); i++)
		{
			Vec4i l = lines[i];
			double hypotenuse = sqrt(pow(l[2] - l[0], 2) + pow(l[3] - l[1], 2));
			double base = fabs(l[2] - l[0]);
			double cos = base / hypotenuse;

			if (cos > 0.828 && cos < 0.860)
			{ 
				; //사다리꼴 에지 검출 x
			}
			else
			{
				if (cos < 0.9659)
				{
					//cout << cos << endl;
					line(Final, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
					line(cdst, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
					line(Line, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255, 255, 255), 3, CV_AA);
				}
			}	
		}
		
		//어파인 변환 복원
		/*
		Point2f inputp2[4];
		inputp2[0] = Point2f(0, 0);
		inputp2[1] = Point2f(616, 0);
		inputp2[2] = Point2f(616, 514);
		inputp2[3] = Point2f(0, 514);

		Point2f outputp2[4];
		outputp2[0] = Point2f(267, 260);
		outputp2[1] = Point2f(367, 260);
		outputp2[2] = Point2f(609, 395);
		outputp2[3] = Point2f(24, 395);
		
		Mat h2 = getPerspectiveTransform(inputp2, outputp2);

		Mat affine2;
		warpPerspective(cdst, affine2, h2, cdst.size());
		//imshow("affine2", affine2);
		*/

		//imshow("Frame", cv_img);
		//cv::imshow("0", cv_img_0);
		//cv::imshow("45", cv_img_45);
		//cv::imshow("90", cv_img_90);
		//cv::imshow("135", cv_img_135);
		
		//imshow("Original", cv_img_0);
		//imshow("Global Thresholding", th);
		imshow("otsu roi", otsu_roi);
		//imshow("otsu", otsu);
		//imshow("open", open);
		imshow("dst", dst);
		//imshow("ostu dst", otsu_dst);
		//imshow("cdst", cdst);
		imshow("Final", Final);
		//imshow("gamma dst", gamma_dst);
		//imshow("roi", Roi);
		//imshow("Line", Line);
		//imshow("compare", compare);
		//imshow("histImage", histImage);
		//imshow("gamma histImage", histImage2);

		//프레임 분할 저장
		/*
		sprintf_s(buf, "C:/우천/20220629_211820_exposure_Auto2/img_%06d.png", index);
		imwrite(buf, Line);

		index++;
		if (index == 999999)
			index = 0;
		*/
		
		if (waitKey(1) == 27)
			break;
	}
	return 0;
}