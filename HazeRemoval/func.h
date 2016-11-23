#ifndef HAZEREMOVAL
#define HAZEREMOVAL

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <math.h>

using namespace cv;
using std::cout;
using std::cin;
using std::endl;

int getMax(int a, int b);
int getMin(int a, int b);

Mat GetMinChannel(Mat img_ori);
Mat GetDarkChannel(Mat img_ori, int mask);
uchar sorting(uchar arr[], int mask_size);
Mat localEntropy(Mat img_min_ch, int mask);
Mat TransmissionMap(Mat img_ori, double j);
Mat EntropyMap(Mat img_min_ch, int mask);
Mat result(Mat img_ori, Mat img_Cx);

#endif