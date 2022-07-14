#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <Windows.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;



int main() {
	//input
	char filename[50];
	cout << "Enter input filename : ";
	cin >> filename;
	vector<Mat> channels;
	Mat ImageBlueChannel;
	Mat ImageGreenChannel;
	Mat ImageRedChannel;
	Mat Img = imread(filename,1);
	Mat rotate;
	Mat Interchange;
	//process
	flip(Img, rotate, -1);
	split(rotate, channels);
	ImageBlueChannel = channels.at(0);
	ImageGreenChannel = channels.at(1);
	ImageRedChannel = channels.at(2);
	channels.at(2) = ImageBlueChannel;
	channels.at(0) = ImageGreenChannel;
	channels.at(1) = ImageRedChannel;
	merge(channels, Interchange);
	//output
	cout << "Enter output filename of original image : ";
	cin >> filename;
	imwrite(filename, Img);
	cout << "Enter output filename of rotate image : ";
	cin >> filename;
	imwrite(filename, rotate);
	cout << "Enter output filename of channel interchange image : ";
	cin >> filename;
	imwrite(filename, Interchange);

	return 0;
}