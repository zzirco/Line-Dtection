#include "Polarization_Split.h"

Split::Split(Mat image)
{
	Initialize();
	g_pImage = image.data;
	SplitImages();
	ImageSave();
}
void Split::Initialize()
{
	g_width = 2464;
	g_height = 2056;

	// �̹��� �޸� �Ҵ� �� �ʱ�ȭ
	g_Buffersize = g_width * g_height;
	//cout << "g_Buffersize : " << g_Buffersize << endl;
	g_pImage = (uchar*)malloc(g_Buffersize);
	memset((void*)g_pImage, 0, g_Buffersize);

	// ���� �̹��� ������ = ���� �̹��� ������ / 2
	g_split_width = g_width / 2;
	g_split_height = g_height / 2;
	g_DegreeBufferSize = g_split_width * g_split_height;

	//cout << "g_width : " << g_width << endl;
	//cout << "g_height : " << g_height << endl;
	//cout << "g_split_width : " << g_split_width << endl;
	//cout << "g_split_height : " << g_split_height << endl;

	// ���� �̹��� �޸� �Ҵ� �� �ʱ�ȭ
	g_pDegree0 = (uchar*)malloc(g_DegreeBufferSize);
	g_pDegree45 = (uchar*)malloc(g_DegreeBufferSize);
	g_pDegree90 = (uchar*)malloc(g_DegreeBufferSize);
	g_pDegree135 = (uchar*)malloc(g_DegreeBufferSize);

	memset((void*)g_pDegree0, 0, g_DegreeBufferSize);
	memset((void*)g_pDegree45, 0, g_DegreeBufferSize);
	memset((void*)g_pDegree90, 0, g_DegreeBufferSize);
	memset((void*)g_pDegree135, 0, g_DegreeBufferSize);
}
void Split::SplitImages()
{
	int i, j;
	int idx1, idx2;
	int cnt = 0;

	for (j = 0; j < g_height; j += 2)
	{
		idx1 = g_width * j;
		idx2 = idx1 + g_width;

		for (i = 0; i < g_width; i += 2)
		{
			g_pDegree0[cnt] = g_pImage[idx1 + i];
			g_pDegree45[cnt] = g_pImage[idx1 + i + 1];
			g_pDegree90[cnt] = g_pImage[idx2 + i];
			g_pDegree135[cnt] = g_pImage[idx2 + i + 1];
			cnt++;
		}
	}
}
void Split::ImageSave()
{
	// ���� �̹���
	cv_img = cv::Mat(
		g_height,			// row : ���� ��(height)
		g_width,			// col : ���� ��(width)
		CV_8UC1,			// type : �̹��� Ÿ��
		g_pImage,			// *data : �̹��� ������
		g_width				// step : �� ���� ���� ��
	);

	// 0�� �̹���
	cv_img_0 = cv::Mat(
		g_split_height,
		g_split_width,
		CV_8UC1,
		g_pDegree0,
		g_split_width
	);

	// 45�� �̹���
	cv_img_45 = cv::Mat(
		g_split_height,
		g_split_width,
		CV_8UC1,
		g_pDegree45,
		g_split_width
	);

	// 90�� �̹���
	cv_img_90 = cv::Mat(
		g_split_height,
		g_split_width,
		CV_8UC1,
		g_pDegree90,
		g_split_width
	);

	// 135�� �̹���
	cv_img_135 = cv::Mat(
		g_split_height,
		g_split_width,
		CV_8UC1,
		g_pDegree135,
		g_split_width
	);
}
Mat Split::Original_Image()
{
	return cv_img;
}
Mat Split::Image_0()
{
	return cv_img_0;
}
Mat Split::Image_45()
{
	return cv_img_45;
}
Mat Split::Image_90()
{
	return cv_img_90;
}
Mat Split::Image_135()
{
	return cv_img_135;
}