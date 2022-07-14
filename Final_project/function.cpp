#include"Header.h"
double IOU_cul(Mat src1, Mat src2) {
	int Inter = 0, Union = 0;
	double iou = 0;
	int value1, value2;
	for (int i = 0; i < src1.cols; i++) {
		for (int j = 0; j < src1.rows; j++) {
			value1 = src1.ptr<uchar>(j)[i];
			value2 = src2.ptr<uchar>(j)[i];
			if (value1 == 255 && value2 == 255)
				Inter++;
			if (value1 == 255 || value2 == 255)
				Union++;
		}
	}
	iou = (double)Inter * 1.0 / Union;
	return iou;
}
Mat eq_rgb(Mat src) {
	Mat eq = Mat::zeros(src.rows, src.cols, CV_8UC3);
	vector<Mat> channels, temp;
	split(src, channels);
	split(eq, temp);
	for (int i = 0; i < 3; i++)
		equalizeHist(channels[i], temp[i]);
	merge(temp, eq);
	return eq;
}
Mat hsv_v(Mat src) {
	Mat hsv;
	cvtColor(src, hsv, COLOR_HSV2RGB);
	vector<Mat> channels;
	split(hsv, channels);
	Mat v = channels[2];
	return v;
}
Mat sort(int num, Mat labels, Mat stats) {
	int max1 = -1, max2 = -1, max3 = -1, max4 = -1, max5 = -1;
	int num_max1 = 0, num_max2 = 0, num_max3 = 0, num_max4 = 0, num_max5 = 0;
	int value;
	for (int i = 1; i < num; i++) {
		value = stats.ptr<INT32>(i)[4];
		if (value > max5 && value < max4) {
			max5 = value;
			num_max5 = i;
		}
		else if (value > max4 && value < max3) {
			max5 = max4;
			max4 = value;
			num_max5 = num_max4;
			num_max4 = i;
		}
		else if (value > max3 && value < max2) {
			max5 = max4;
			max4 = max3;
			max3 = value;
			num_max5 = num_max4;
			num_max4 = num_max3;
			num_max3 = i;
		}
		else if (value > max2 && value < max1) {
			max5 = max4;
			max4 = max3;
			max3 = max2;
			max2 = value;
			num_max5 = num_max4;
			num_max4 = num_max3;
			num_max3 = num_max2;
			num_max2 = i;
		}
		else if (value > max1) {
			max5 = max4;
			max4 = max3;
			max3 = max2;
			max2 = max1;
			max1 = value;
			num_max5 = num_max4;
			num_max4 = num_max3;
			num_max3 = num_max2;
			num_max2 = num_max1;
			num_max1 = i;
		}
		value = value;
	}
	bool select;
	for (int i = 0; i < labels.cols; i++) {
		for (int j = 0; j < labels.rows; j++) {
			value = labels.ptr<INT32>(j)[i];
			select = (value == num_max1) || (value == num_max2) ||
				(value == num_max3) || (value == num_max4) || (value == num_max5);
			if (!select) {
				labels.ptr<INT32>(j)[i] = 0;
			}
			if (select) {
				labels.ptr<INT32>(j)[i] = 255;
			}
		}
	}
	return labels;
}
int read_img(Mat& after, Mat& before, Mat& correct, int num) {
	stringstream temp;
	temp << num;
	string name_num = temp.str();
	string filename;
	filename = position + position_good + name_num + name_after;
	after = imread(filename);
	filename = position + position_good + name_num + name_before;
	before = imread(filename);
	filename = position + position_gt + name_num + name_gt;
	correct = imread(filename);
	cvtColor(correct, correct, COLOR_RGB2GRAY);
	threshold(correct, correct,	128, 255, THRESH_BINARY);
	return 1;
}
int read_img(Mat& after, Mat& before, int num) {
	stringstream temp;
	temp << num;
	string name_num = temp.str();
	string filename;
	filename = position + position_bad + name_num + "b" + name_after;
	after = imread(filename);
	filename = position + position_bad + name_num + "b" + name_before;
	before = imread(filename);
	return 1;
}
Mat generate_gt() {
	stringstream temp;
	int number = 1;
	temp << number;
	string name_num = temp.str();
	string filename;
	Mat correct, gt;
	filename = position + position_gt + name_num + name_gt;
	correct = imread(filename);
	cvtColor(correct, correct, COLOR_RGB2GRAY);
	correct.copyTo(gt);
	for (int i = 1; i < 7; i++) {
		filename = position + position_gt + name_num + name_gt;
		correct = imread(filename);
		cvtColor(correct, correct, COLOR_RGB2GRAY);
		gt = gt | correct;
	}
	threshold(gt, gt, 128, 255, THRESH_BINARY);
	return gt;
}
Mat without_noise(Mat src) {
	Mat labels, stats, centroids;
	Mat element = getStructuringElement(MORPH_RECT, Size(7, 7), Point(-1, -1));
	Mat morph;
	erode(src, morph, element);
	//erode(morph, morph, element);
	erode(morph, morph, element);
	dilate(morph, morph, element);
	//dilate(morph, morph, element);
	//erode(morph, morph, element);
	//erode(morph, morph, element);

	threshold(morph, morph, 40, 255, THRESH_BINARY|THRESH_OTSU);
	int nccomps = cv::connectedComponentsWithStats(
		morph,     //二值圖像
		labels,    //和原圖一樣大的標記圖
		stats,     //nccomps*5的矩陣 表示每個連通域的外接矩形和面積
		centroids  //nccomps*2的矩陣 表示每個連通域區域的質心
	);
	Mat dst;
	dst = sort(nccomps, labels, stats);
	dst.convertTo(dst, CV_8UC1);
	dilate(dst, dst, element);
	erode(dst, dst, element);
	dilate(dst, dst, element);
	return dst;
}
Mat process(Mat after, Mat before) {

	Mat eq_after = eq_rgb(after);
	Mat eq_before = eq_rgb(before);
	Mat blur_eq_after, blur_eq_before;

	bilateralFilter(eq_after, blur_eq_after, 15, 150, 3);
	bilateralFilter(eq_before, blur_eq_before, 15, 150, 3);
	//bilateralFilter(eq_after, blur_eq_after, 15, 50, 3);
	//bilateralFilter(eq_before, blur_eq_before, 15, 50, 3);

	//GaussianBlur(eq_after, blur_eq_after, Size(3, 3), 1);
	//GaussianBlur(eq_before, blur_eq_before, Size(3, 3), 1);
	Mat hsv_after, hsv_before;
	cvtColor(blur_eq_after, hsv_after, COLOR_RGB2HSV);
	cvtColor(blur_eq_before, hsv_before, COLOR_RGB2HSV);

	Mat subtract = hsv_after - hsv_before;
	//Mat blur_subtract;
	//bilateralFilter(subtract, blur_subtract, 15, 150, 3);
	//GaussianBlur(subtract, blur_subtract, Size(3, 3), 1);
	//Mat morph = hsv_v(blur_subtract);
	Mat morph = hsv_v(subtract);
	Mat	detect = without_noise(morph);
	return detect;
}
bool classify(Mat gt, Mat detect) {
	Mat gt_labels, gt_stats, gt_centroids;
	Mat labels, stats, centroids;
	double centroidX, centroidY;
	int area;
	int standard;
	int nccomps;
	int gg = 33;
	nccomps = connectedComponentsWithStats(gt, gt_labels, gt_stats, gt_centroids);
	nccomps = connectedComponentsWithStats(detect, labels, stats, centroids);
	if (nccomps != 6)
		return false;
	for (int i = 1; i < nccomps; i++) {
		area = stats.ptr<INT32>(i)[4];
		standard = gt_stats.ptr<INT32>(i)[4];
		if (area > standard * 1.8 || area < standard * 0.2)
			return false;
		centroidX = centroids.ptr<double>(i)[0];
		centroidY = centroids.ptr<double>(i)[1];
		standard = gt_centroids.ptr<double>(i)[0];
		if (centroidX > standard + gg || centroidX < standard - gg)
			return false;
		standard = gt_centroids.ptr<double>(i)[1];
		if (centroidY > standard + gg || centroidY < standard - gg)
			return false;
	}
	return true;
}