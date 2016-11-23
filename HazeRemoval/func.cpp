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

Mat GetMinChannel(Mat img_ori){
	Mat BGR[3];

	split(img_ori, BGR);	//원본 영상의 BGR성분을 분리

	Mat img_ori_B = BGR[0]; //배열에 B성분 저장
	Mat img_ori_G = BGR[1];	//배열에 G성분 저장
	Mat img_ori_R = BGR[2];	//배열에 R성분 저장

	//uchar B, G, R;

	uchar bgr[3] = { 0, 0, 0 };
	uchar minval=0;

	Mat img_min_ch;
	img_min_ch = img_ori_R.clone();

	for (int h = 0; h < img_ori.rows; h++){
		for (int w = 0; w < img_ori.cols; w++){
			bgr[0] = img_ori_B.at<uchar>(h, w);
			bgr[1] = img_ori_G.at<uchar>(h, w);
			bgr[2] = img_ori_R.at<uchar>(h, w);

			minval = sorting(bgr, 3);

			img_min_ch.at<uchar>(h, w) = minval;

			/*B = img_ori_B.at<uchar>(h, w);
			G = img_ori_G.at<uchar>(h, w);
			R = img_ori_R.at<uchar>(h, w);

			bgr[0] = B;
			bgr[1] = G;
			bgr[2] = R;

			minval = sorting(bgr, 3);*/

			//if ((B <= G && B > R) || (B <= R && B >= G)){	///////최소값 찾는 조건문 ///////
			//	img_min_ch.at<uchar>(h, w) = B;
			//}

			//else if ((G <= B && G >= R) || (G <= R && G >= B)){
			//	img_min_ch.at<uchar>(h, w) = G;
			//}

			//else if ((R <= B && R >= G) || (R <= G && R >= B)){
			//	img_min_ch.at<uchar>(h, w) = R;
			//}

			//else if (B == G == R){
			//	img_min_ch.at<uchar>(h, w) = B;
			//}	/////////////////////////////////////////////////////////////
		}
	}
	return img_min_ch;
}

Mat GetDarkChannel(Mat img_min_ch, int mask){
	Mat img_dark_ch;
	img_dark_ch = img_min_ch.clone();
	uchar* arr = (uchar*)calloc(mask*mask,sizeof(uchar));
	uchar minval;

	for (int h = 0; h < img_min_ch.rows; h++){
		for (int w = 0; w < img_min_ch.cols; w++){
			
			for (int i = getMax(0,h-mask/2); i < getMin(h+mask/2+1, img_min_ch.rows); i++){
				for (int j = getMax(0,w-mask/2); j < getMin(w+mask/2+1,img_min_ch.cols); j++){
					arr[(i - h)*mask + j - w + mask*mask / 2] = img_min_ch.at<uchar>(i, j);
					//cout << (i - h)*mask + j - w + mask*mask / 2 << endl;
					//cout << (h - i)*mask + w - j + mask*mask / 2 << endl;
				}
				//cout << endl;
			}	//마스크 사이즈를 입력 받아서 메모리 공간에 할당
			minval = sorting(arr, mask*mask);	//마스크 내부에서 가장 작은값을 반환
			img_dark_ch.at<uchar>(h, w) = minval;
		}
	}
	free(arr);	
	return img_dark_ch;
}

uchar sorting(uchar arr[], int mask_size){	//버블정렬(작은->큰)
	uchar temp;
	for (int i = 0; i < mask_size; i++){	
		for (int j = 0; j < mask_size - 1; j++){
			if (arr[j] > arr[j + 1]){
				temp = arr[j];
				arr[j] = arr[j + 1];
				arr[j + 1] = temp;
			}
		}
	}
	temp = arr[0];
	return temp;	
}

Mat TransmissionMap(Mat img_ori, double j){
	Mat img_trans = img_ori.clone();
	Mat img_buf = img_ori.clone();
	Mat img_min = img_ori.clone();

	int a;
	double b;
	double temp = 0;

	for (int h = 0; h < img_ori.rows; h++){
		for (int w = 0; w < img_ori.cols*img_ori.channels(); w++){
			//img_buf.at<uchar>(h, w) /= 255;
			a=img_buf.at<uchar>(h, w) / 255;
			b = img_buf.at<uchar>(h, w) % 255;
			img_buf.at<uchar>(h, w) = floor(a + b + 0.5);
			//img_buf.at<uchar>(h, w)=(img_buf.at<uchar>(h, w) * 256) / 255;
			//cout << img_buf.at<uchar>(h, w) << endl;
		}
	}

	img_min = GetMinChannel(img_buf);

	for (int h = 0; h < img_ori.rows; h++){
		for (int w = 0; w < img_ori.cols; w++){
			temp = 1 - j*log10(img_min.at<uchar>(h, w));
			assert(temp != 0);
			img_trans.at<uchar>(h,w) = temp;
		}
	}

	return img_trans;
}

Mat EntropyMap(Mat img_min_ch, int mask){
	Mat img_entropy = img_min_ch.clone();
		
	int histogram[256];

	for (int h = 0; h < img_min_ch.rows; h+=mask){
		for (int w = 0; w < img_min_ch.cols; w+=mask){
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
			//if (entropy > 255)	entropy = 255;
			//else if (entropy < 0)	entropy = 0;
		
			for (int i = getMax(0, h - mask / 2); i < getMin(h + mask / 2 + 1, img_min_ch.rows); i++){
				for (int j = getMax(0, w - mask / 2); j < getMin(w + mask / 2 + 1, img_min_ch.cols); j++){
					img_entropy.at<uchar>(i, j) = entropy*10;
				}
			}
		}
	}
	return img_entropy;
}

Mat result(Mat img_ori, Mat img_Cx){
	Mat t_x=img_Cx.clone();
	Mat Result_img = img_ori.clone();//Jc(x)
	double j=0.81;

	int a=0, b=0, c=0;
	double aa=0, bb=0, cc=0;
	
	vector<Mat> BGR;
	split(img_ori, BGR);

	Mat& img_B = BGR[0];
	Mat& img_G = BGR[1];
	Mat& img_R = BGR[2];
	t_x = TransmissionMap(img_ori, j);

	for (int h = 0; h < img_Cx.rows; h++){
		for (int w = 0; w < img_Cx.cols; w++){
			//cout << (int)t_x.at<uchar>(h, w) << endl;
			if (t_x.at<uchar>(h, w) == 0){ 
				t_x.at<uchar>(h, w) = 1; 
				//a = ((img_B.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) / 0.1);
				//aa = ((img_B.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) % 1);
				//
				//b = ((img_G.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) / 0.1);
				//bb = ((img_G.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) % 1);
				//
				//c = ((img_R.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) / 0.1);
				//cc = ((img_R.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) % 1);
			}

			//img_B.at<uchar>(h, w) = floor(a + aa )+t_x.at<uchar>(h, w);
			//img_G.at<uchar>(h, w) = floor(b + bb )+ t_x.at<uchar>(h, w);
			//img_R.at<uchar>(h, w) = floor(c + cc )+ t_x.at<uchar>(h, w);
			img_B.at<uchar>(h, w) = ((img_B.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) / t_x.at<uchar>(h, w)) + t_x.at<uchar>(h, w);
			img_G.at<uchar>(h, w) = ((img_G.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) / t_x.at<uchar>(h, w)) + t_x.at<uchar>(h, w);
			img_R.at<uchar>(h, w) = ((img_R.at<uchar>(h, w) - img_Cx.at<uchar>(h, w)) / t_x.at<uchar>(h, w)) + t_x.at<uchar>(h, w);
		}
	}

	merge(BGR, Result_img);

	/*for (int i = 0; i < Hazed_img.rows; i++){
		for (int j = 0; j < Hazed_img.cols*3; j+=3){
			
			for (int c = 0; c < Hazed_img.channels(); c++){
				int temp = Hazed_img.data[i*Hazed_img.rows*Hazed_img.channels() + 3 * j + c] - Haze_estimg.data[i*Hazed_img.rows + j];
				temp /= 3;
				if (Hazed_img.data[i*Hazed_img.rows*Hazed_img.channels() + 3 * j + c]- Haze_estimg.data[i*Hazed_img.rows + j] < 0)
					temp = 0;
				else if (Hazed_img.data[i*Hazed_img.rows*Hazed_img.channels() + 3 * j + c]- Haze_estimg.data[i*Hazed_img.rows + j] > 255)
					temp = 255;

				Result_img.data[i*Hazed_img.rows*Hazed_img.channels() + 3 * j + c] = temp;
			}
		}
	}*/
	return Result_img;
}

