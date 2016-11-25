#include <stdio.h>
#include <time.h>
#include <direct.h>
#include <opencv2\opencv.hpp>
#pragma warning(disable :4996)

using namespace cv;
using namespace std;

int getMax(int a, int b);
int getMin(int a, int b);
Mat EntropyMap(Mat img_min_ch, int mask);
void colorful_show(Mat &src, vector<Mat> &channels, bool Is_show);
Mat getMedianDarkChannel(Mat src, int patch);
int estimateA(Mat DC);
Mat estimateTransmission(Mat DCP, int ac);
Mat getDehazed(Mat source, Mat t, int al);