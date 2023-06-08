#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class Split
{

private :
	// 원본 이미지 포인터
	__int32 g_Buffersize;
	uchar* g_pImage = NULL;

	// 분할 이미지 포인터
	__int32 g_DegreeBufferSize;
	uchar* g_pDegree0 = NULL;
	uchar* g_pDegree45 = NULL;
	uchar* g_pDegree90 = NULL;
	uchar* g_pDegree135 = NULL;

	// 원본 이미지 사이즈
	__int32 g_width = 0, g_height = 0;

	// 분할 이미지 사이즈
	__int32 g_split_width = 0, g_split_height = 0;

	//분할 이미지
	Mat cv_img, cv_img_0, cv_img_45, cv_img_90, cv_img_135;

public :
	Split(Mat image);  //생성자
	void Initialize();  //분할 이미지 메모리 할당 및 초기화 함수
	void SplitImages();  //이미지 분할 함수
	void ImageSave(); //분할된 이미지 저장 함수
	Mat Original_Image();  //분할된 이미지 리턴 함수
	Mat Image_0();
	Mat Image_45();
	Mat Image_90();
	Mat Image_135();
};
