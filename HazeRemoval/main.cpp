#include "func.h"

void main(){
	Mat img_ori = imread("forest1.jpg", IMREAD_COLOR);
	
	Mat img_min_ch;
	Mat img_dark_ch;
	Mat img_trans;
	Mat img_entropy;
	
	img_min_ch = GetMinChannel(img_ori);

	int mask;
	cout << "Mask size for dark channel : ";
	cin >> mask;

	//double w=0.5;

	/*namedWindow("Original", CV_WINDOW_AUTOSIZE);
	imshow("Original", img_ori);

	namedWindow("Min Channel", CV_WINDOW_AUTOSIZE);
	imshow("Min Channel", img_min_ch);*/

	img_dark_ch = GetDarkChannel(img_min_ch, mask);
	
	/*namedWindow("Dark Channel", CV_WINDOW_AUTOSIZE);
	imshow("Dark Channel", img_dark_ch);*/

	//img_trans = TransmissionMap(img_ori, w);//t(x)=1-wlog(min(I/A))

	/*namedWindow("Transmission Map", CV_WINDOW_AUTOSIZE);
	imshow("Transmission Map", img_trans);*/

	img_entropy = EntropyMap(img_min_ch, mask);
	Mat img_Cx;
	subtract(img_dark_ch, img_entropy, img_Cx);
	//namedWindow("Entropy", CV_WINDOW_AUTOSIZE);
	//imshow("Entropy", img_Cx);
	//Mat result_img;
	GaussianBlur(img_Cx, img_Cx, Size(7, 7), 0, 0);
	namedWindow("Entropy", CV_WINDOW_AUTOSIZE);
	imshow("Entropy", img_Cx);
	//Mat resulttt;
	//subtract(img_ori, result_img, resulttt);
	Mat result_img = result(img_ori, img_Cx);
	namedWindow("result", CV_WINDOW_AUTOSIZE);
	imshow("result", result_img);

	waitKey();
}