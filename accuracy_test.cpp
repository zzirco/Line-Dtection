#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	int number = 1;
	int total_count = 0;
	double percent_sum = 0;

	while (true)
	{
		//정확도 검사
	A:
		string cv_img_0_fname = format("c:/T7/우천/20220629_211441_exposure_Auto/img_%06d.jpg", number);
		string fname = format("c:/T7/우천/20220629_211441_exposure_Auto/img_%06d.png", number);
		string compare_fname = format("c:/T7/20220629_211441_exposure_Auto/20220629_211441_exposure_Auto.mp4_%06d_tot.png", number - 1);
		Mat cv_img_0 = imread(cv_img_0_fname);
		if (cv_img_0.empty())
		{
			cout << "cv_img_0 image is not open" << endl;
			break;
		}
		Mat original = imread(fname, IMREAD_GRAYSCALE);
		if (original.empty())
		{
			cout << "original image is not open" << endl;
			break;
		}

		imshow("cv_img_0", cv_img_0);
		imshow("original", original);

		if (waitKey(1) == 27)
			break;

		Mat compare = imread(compare_fname, IMREAD_GRAYSCALE);
		if (compare.empty())
		{
			cout << "compare image is now loading" << endl;
			number++;
			goto A;
		}

		resize(compare, compare, Size(616, 514));
		int c_count = 0;
		int correct = 0;
		double c_percent = 0;

		for (int i = 0; i < 514; i++)
			for (int j = 0; j < 616; j++)
			{
				if (compare.at<uchar>(i, j) == 255)
				{
					c_count++;
					if (compare.at<uchar>(i, j) == original.at<uchar>(i, j))
						correct++;
				}
			}
		if (c_count != 0)
		{
			c_percent = (double)correct / (double)c_count * 100;
			percent_sum += c_percent;
			total_count++;
		}
		else
		{
			c_percent = 0;
		}
		cout << "c_count : " << c_count << "\tcorrect : " << correct;
		cout << "\tc_percent : " << c_percent << "%" << endl;

		
		imshow("compare", compare);

		number++;
		
	}

	double total_accuracy = (double)percent_sum / (double)total_count;
	//cout << "percent sum : " << percent_sum << endl;
	//cout << "total count : " << total_count << endl;
	cout << "Total accuracy : " << total_accuracy << "%" << endl;

	return 0;
}