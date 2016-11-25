#include "func.h"

void main(){

	Mat frame = imread("Haze_img.jpg", 1);
	resize(frame, frame, Size(), 0.5, 0.5);
	Mat darkChannel;
	Mat T;
	Mat fogfree;
	double alpha = 0.02;    //alpha smoothing
	int Airlight;           //airlight value of current frame

	namedWindow("before and after", CV_WINDOW_AUTOSIZE);

	
	//create mat for showing the frame before and after processing
	Mat beforeafter = Mat::zeros(frame.rows, 2 * frame.cols, CV_8UC3);
	Rect roil(0, 0, frame.cols, frame.rows);
	Rect roir(frame.cols, 0, frame.cols, frame.rows);
	time_t start = clock();

	int spending_time;
	while (true){
		darkChannel = getMedianDarkChannel(frame, 3);

		Airlight = estimateA(darkChannel);
		T = estimateTransmission(darkChannel, Airlight);
		fogfree = getDehazed(frame, T, Airlight);

		/////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 1

		darkChannel = getMedianDarkChannel(fogfree, 3);

		Airlight = estimateA(darkChannel);
		T = estimateTransmission(darkChannel, Airlight);
		fogfree = ; (fogfree, T, Airlight);

#endif
		/////////////////////////////////////////////////////////////////////////////////////////////////////////

		time_t end = clock();

		cout << "spending time : " << (float)(end - start) << "ms" << endl;
		spending_time = (float)(end - start);
		start = end;
		frame.copyTo(beforeafter(roil));
		fogfree.copyTo(beforeafter(roir));

		imshow("DarkChannel", darkChannel);
		imshow("before and after", beforeafter);

		int c = waitKey(30);
		if (c == 'x'){
			break;
		}
	}
	time_t timer = time(NULL);
	struct tm *Timer = localtime(&timer);
	char Directory[50];
	sprintf(Directory, "[%d. %d. %d] %d½Ã %dºÐ", Timer->tm_year + 1900, Timer->tm_mon + 1, Timer->tm_mday, Timer->tm_hour, Timer->tm_min);
	mkdir(Directory);
	char buf[100];
	sprintf(buf, "%s/Dehaze.jpg",Directory);
	imwrite(buf, beforeafter);

	sprintf(buf, "%s/time.txt", Directory);
	FILE* f = fopen(buf, "wt");
	fprintf(f, "Input width : %d\n", frame.cols);
	fprintf(f, "Input height : %d\n", frame.rows);
	fprintf(f, "Spending time : %d ms\n", spending_time);



}


