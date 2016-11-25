#include "func.h"
int getMax(int a, int b){
	if (a > b)
		return a;
	else
		return b;
}

int getMin(int a, int b){
	if (a > b)
		return b;
	else
		return a;
}
void colorful_show(Mat &src, vector<Mat> &channels, bool Is_show){
	if (src.channels() < 2){
		cout << "channel is very small" << endl;
		return;
	}
	int src_ch = src.channels();

	for (int i = 0; i < src_ch; i++){
		channels.push_back(src.clone());

		for (int h = 0; h < src.rows; h++){
			for (int w = 0; w < src.cols*src_ch; w++){
				if (w%src_ch != i) channels[i].data[h*(src.cols*src_ch) + w] = 0;
			}
		}
	}

	if (Is_show){
		char Ch_name[5];
		imshow("Init", src);
		for (int i = 0; i < src_ch; i++){
			sprintf(Ch_name, "Ch%d", i);
			imshow(Ch_name, channels[i]);
		}
		waitKey();
	}

}

Mat getMedianDarkChannel(Mat src, int patch)
{
	Mat rgbmin = Mat::zeros(src.rows, src.cols, CV_8UC1);
	Mat MDCP;
	Vec3b intensity;

	for (int m = 0; m < src.rows; m++)
	{
		for (int n = 0; n < src.cols; n++)
		{
			intensity = src.at<Vec3b>(m, n);
			rgbmin.at<uchar>(m, n) = min(min(intensity.val[0], intensity.val[1]), intensity.val[2]);
		}
	}

	Mat E = EntropyMap(rgbmin, patch);
	Mat result;
	subtract(rgbmin, E, result);
	

	medianBlur(result, MDCP, patch);

	return MDCP;

}


//estimate airlight by the brightest pixel in dark channel (proposed by He et al.)
int estimateA(Mat DC)
{
	double minDC, maxDC;
	minMaxLoc(DC, &minDC, &maxDC);
	
	//cout << "estimated airlight is:" << maxDC << endl;
	return maxDC;
}


//estimate transmission map
Mat estimateTransmission(Mat DCP, int ac)
{
	double w = 0.75;
	Mat transmission = Mat::zeros(DCP.rows, DCP.cols, CV_8UC1);
	Scalar intensity;

	for (int m = 0; m < DCP.rows; m++)
	{
		for (int n = 0; n < DCP.cols; n++)
		{
			intensity = DCP.at<uchar>(m, n);
			transmission.at<uchar>(m, n) = (1 - w * intensity.val[0] / ac) * 255;
		}
	}
	return transmission;
}


//dehazing foggy image
Mat getDehazed(Mat source, Mat t, int al)
{
	double tmin = 0.1;
	double tmax;

	Scalar inttran;
	Vec3b intsrc;
	Mat dehazed = Mat::zeros(source.rows, source.cols, CV_8UC3);

	for (int i = 0; i < source.rows; i++)
	{
		for (int j = 0; j < source.cols; j++)
		{
			inttran = t.at<uchar>(i, j);
			intsrc = source.at<Vec3b>(i, j);
			tmax = (inttran.val[0] / 255) < tmin ? tmin : (inttran.val[0] / 255);
			for (int k = 0; k<3; k++)
			{
				dehazed.at<Vec3b>(i, j)[k] = abs((intsrc.val[k] - al) / tmax + al) > 255 ? 255 : abs((intsrc.val[k] - al) / tmax + al);
			}
		}
	}
	return dehazed;
}
Mat EntropyMap(Mat img_min_ch, int mask){
	Mat img_entropy = img_min_ch.clone();

	int histogram[256];

	for (int h = 0; h < img_min_ch.rows; h += mask){
		for (int w = 0; w < img_min_ch.cols; w += mask){
			for (int i = 0; i < 256; i++){
				histogram[i] = 0;
			}
			int mask_size = 0;
			for (int i = getMax(0, h - mask / 2); i < getMin(h + mask / 2 + 1, img_min_ch.rows); i++){
				for (int j = getMax(0, w - mask / 2); j < getMin(w + mask / 2 + 1, img_min_ch.cols); j++){
					histogram[(int)img_min_ch.at<uchar>(i, j)]++;
					mask_size++;
				}
			}
			double entropy = 0;

			for (int i = 0; i < 256; i++){
				if (histogram[i] != 0){
					double p = histogram[i] / (double)mask_size;
					entropy += p*log2(p);
				}
			}

			entropy *= -1;

			for (int i = getMax(0, h - mask / 2); i < getMin(h + mask / 2 + 1, img_min_ch.rows); i++){
				for (int j = getMax(0, w - mask / 2); j < getMin(w + mask / 2 + 1, img_min_ch.cols); j++){
					img_entropy.at<uchar>(i, j) = entropy * 10;
				}
			}
		}
	}
	return img_entropy;
}