#pragma once
#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

class Split
{

private :
	// ���� �̹��� ������
	__int32 g_Buffersize;
	uchar* g_pImage = NULL;

	// ���� �̹��� ������
	__int32 g_DegreeBufferSize;
	uchar* g_pDegree0 = NULL;
	uchar* g_pDegree45 = NULL;
	uchar* g_pDegree90 = NULL;
	uchar* g_pDegree135 = NULL;

	// ���� �̹��� ������
	__int32 g_width = 0, g_height = 0;

	// ���� �̹��� ������
	__int32 g_split_width = 0, g_split_height = 0;

	//���� �̹���
	Mat cv_img, cv_img_0, cv_img_45, cv_img_90, cv_img_135;

public :
	Split(Mat image);  //������
	void Initialize();  //���� �̹��� �޸� �Ҵ� �� �ʱ�ȭ �Լ�
	void SplitImages();  //�̹��� ���� �Լ�
	void ImageSave(); //���ҵ� �̹��� ���� �Լ�
	Mat Original_Image();  //���ҵ� �̹��� ���� �Լ�
	Mat Image_0();
	Mat Image_45();
	Mat Image_90();
	Mat Image_135();
};
