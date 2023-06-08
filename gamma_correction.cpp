#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	Mat src1, src2, dst;
	double gamma = 0.5;

	src1 = imread("c:/¿ìÃµ/Æí±¤/img_000290.jpg", IMREAD_GRAYSCALE);
	resize(src1, src1, Size(src1.cols / 4, src1.rows / 4));

	Mat table(1, 256, CV_8U);
	uchar* p = table.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma) * 255.0);

	LUT(src1, table, dst);
	imshow("src1", src1);
	imshow("dst", dst);
	waitKey(0);
	return 0;
}