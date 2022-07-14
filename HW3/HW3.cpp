#define _CRT_SECURE_NO_WARNINGS
#define _USE_MATH_DEFINES

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stdbool.h>
#include <math.h>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;

double rd_calculate(int x, int x0, int y, int y0) {
	return sqrt((x - x0) * (x - x0) + (y - y0) * (y - y0));
}

double theta_calculate(int x, int x0, int y, int y0) {
	return atan2(double(y - y0), double(x - x0));
}

double ru_calculate(double rd, double w) {
	return tan(rd * w) / (2 * tan(w / 2));
}
double cot(double x)
{
	return 1 / tan(x);
}
double uv_covert_x(int m, int n, double u, double v, int dz, double theta0, double a, int r0, int dx) {
	return dz * cot(theta0 - a + u * ((2 * a) / (m - 1))) * sin(r0 - a + v * ((2 * a) / (n - 1))) + dx;
}
double uv_covert_y(int m, int n, double u, double v, int dz, double theta0, double a, int r0, int dy) {
	return dz * cot(theta0 - a + u * ((2 * a) / (m - 1))) * cos(r0 - a + v * ((2 * a) / (n - 1))) + dy;
}
double xy_covert_u(int m, double x, double y, int dz, double theta0, double a, int r0, int dx, int dy) {
	return ((m - 1) / (2 * a)) * (atan((dz * sin(atan((x - dx) / (y - dy)))) / (x - dx)) - theta0 + a);
}
double xy_covert_v(int n, double x, double y, int dz, double theta0, double a, int r0, int dx, int dy) {
	return ((n - 1) / (2 * a)) * (atan((x - dx) / (y - dy)) - r0 + a);
}
void savebmp(char filename[], RGBTRIPLE* output, BITMAPFILEHEADER FileHeader, BITMAPINFOHEADER InfoHeader);
int main() {
	/***********************************  3_1  *****************************************/
	//load
	BITMAPFILEHEADER fileheader1;
	BITMAPINFOHEADER infoheader1;
	char filename_distortion[50] = "distortion.bmp";
	char out1_filename[50] = "out3_1.bmp";
	
	FILE* fp;
	int size = 0, ImageX, ImageY;
	cout << "3_1 please enter the filename to load (.bmp) : ";
	cin >> filename_distortion;
	fp = fopen(filename_distortion, "rb");
	if (!fp) {
		cout << "file open fail!" << endl;
		return 0;
	}
	fread(&fileheader1, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&infoheader1, sizeof(BITMAPINFOHEADER), 1, fp);
	ImageX = infoheader1.biWidth;
	ImageY = infoheader1.biHeight;

	size = ImageX * ImageY;
	RGBTRIPLE rgb;
	RGBTRIPLE* ori1_image = new RGBTRIPLE[size];
	for (int i = 0; i < ImageY; i++)
	{
		for (int j = 0; j < ImageX; j++)
		{
			fread(&rgb, sizeof(RGBTRIPLE), 1, fp);
			ori1_image[i * ImageX + j] = rgb;
		}
	}
	fclose(fp);

	// define
	int X0 = ImageX / 2;
	int Y0 = ImageY / 2;

	double rd = rd_calculate(ImageX - 1, X0, ImageY - 1, Y0);
	double theta = theta_calculate(ImageX - 1, X0, ImageY - 1, Y0);

	double ru = ru_calculate(rd, 0.0036);

	int Xmax = (int)(ceil((ru * cos(theta) * 2 / 10) / 4) * 4);
	int Ymax = (int)((ru * sin(theta)) * 2 / 10);

	RGBTRIPLE* unwarp = new RGBTRIPLE[Xmax * Ymax]();
	RGBTRIPLE padding;
	padding.rgbtBlue = 205;
	padding.rgbtGreen = 205;
	padding.rgbtRed = 205;
	//unwarp
	for (int y = 0; y < Ymax; y++) {
		for (int x = 0; x < Xmax; x++) {
			int xu = x * 10;
			int yu = y * 10;
			double tempx = (Xmax / 2 * 10);
			double tempy = (Ymax / 2 * 10);
			double ru = sqrt((tempx - xu) * (tempx - xu) + (tempy - yu) * (tempy - yu));
			double thetau = theta_calculate(tempx, xu, tempy, yu);
			rd = (1 / 0.0036) * atan(2 * ru * tan(0.0036 / 2));

			int xd = rd * cos(thetau) + X0;
			int yd = rd * sin(thetau) + Y0;
			if (xd < 0 || yd < 0 || xd > ImageX - 1 || yd > ImageY - 1) {
				unwarp[x + y * Xmax] = padding;
			}
			else
				unwarp[x + y * Xmax] = ori1_image[xd + yd * ImageX];
		}
	}
	//save
	RGBTRIPLE* out1 = new RGBTRIPLE[Xmax * Ymax];
	for (int i = 0; i < Xmax * Ymax; i++) {
		out1[i] = unwarp[Xmax * Ymax - i-1];
	}
	fileheader1.bfSize = Xmax * Ymax * 3 + 54;
	infoheader1.biWidth = Xmax;
	infoheader1.biHeight = Ymax;
	infoheader1.biSizeImage = Xmax * Ymax * 3;
	cout << "3_1 please enter the filename to save (.bmp) : ";
	cin >> out1_filename;
	savebmp(out1_filename, out1, fileheader1, infoheader1);

	/***********************************  3_2  *****************************************/
	//parameter
	const double alpha = 15 * M_PI / 180;
	const int dx = 4;
	const int dy = -10;
	const int dz = 5;
	const int r0 = 0;
	const double theta0 = 0.025;
	//load
	BITMAPFILEHEADER fileheader2;
	BITMAPINFOHEADER infoheader2;
	char filename_road[50] = "road.bmp";
	char out2_filename[50] = "out3_2.bmp";

	FILE* fp2;
	cout << "3_2 please enter the filename to load (.bmp) : ";
	cin >> filename_road;
	fp2 = fopen(filename_road, "rb");
	if (!fp2) {
		cout << "file open fail!" << endl;
		return 0;
	}
	fread(&fileheader2, sizeof(BITMAPFILEHEADER), 1, fp2);
	fread(&infoheader2, sizeof(BITMAPINFOHEADER), 1, fp2);
	ImageX = infoheader2.biWidth;
	ImageY = infoheader2.biHeight;

	size = ImageX * ImageY;
	RGBTRIPLE* ori2_image = new RGBTRIPLE[size];
	for (int i = 0; i < ImageY; i++)
	{
		for (int j = 0; j < ImageX; j++)
		{
			fread(&rgb, sizeof(RGBTRIPLE), 1, fp2);
			ori2_image[i * ImageX + j] = rgb;
		}
	}
	fclose(fp2);
	RGBTRIPLE* temp_image = new RGBTRIPLE[size];
	for (int i = 0; i < ImageY; i++) {
		for (int j = 0; j < ImageX; j++) {
			temp_image[size - (i * ImageX + j)] = ori2_image[i * ImageX + j];
		}
	}

	int Horizon = ceil((ImageY - 1) / (2 * alpha) * (-theta0 + alpha));

	int xmin = 0;
	int ymin = 0;
	int xmax = 0;
	int ymax = 0;
	double tempx;
	double tempy;

	for (int u = Horizon; u < ImageY; u++) {
		for (int v = 0; v < ImageX; v++) {
			tempx = uv_covert_x(ImageY, ImageX, u, v, dz, theta0, alpha, r0, dx);
			tempy = uv_covert_y(ImageY, ImageX, u, v, dz, theta0, alpha, r0, dy);
			if (tempx < xmin)
				xmin = tempx;
			if (tempy < ymin)
				ymin = tempy;
			if (tempx > xmax)
				xmax = tempx;
			if (tempy > ymax)
				ymax = tempy;
		}
	}

	int IPM_height = xmax - xmin;
	int IPM_width = ymax - ymin;
	int ratio = 100;
	IPM_width = ((IPM_width / ratio) / 4) * 4;
	IPM_height = IPM_height / ratio;

	RGBTRIPLE* image_IPM = new RGBTRIPLE[IPM_width * IPM_height]();

	int x, y, u, v;
	for (int x_p = 0; x_p < IPM_height; x_p++) {
		for (int y_p = 0; y_p < IPM_width; y_p++) {
			x = x_p * ratio + xmin;
			y = y_p * ratio + ymin;
			u = xy_covert_u(ImageY, x, y, dz, theta0, alpha, r0, dx, dy);
			v = xy_covert_v(ImageX, x, y, dz, theta0, alpha, r0, dx, dy);
			if (v<0 || v>ImageX) 
				image_IPM[(y_p + IPM_width * x_p)] = padding;
			else 
				image_IPM[(y_p + IPM_width * x_p)] = temp_image[v + ImageX * u];
		}
	}


	fileheader2.bfSize = IPM_height * IPM_width * 3 + 54;
	infoheader2.biWidth = IPM_width;
	infoheader2.biHeight = IPM_height;
	infoheader2.biSizeImage = IPM_height * IPM_width * 3;
	cout << "3_2 please enter the filename to save (.bmp) : ";
	cin >> out2_filename;
	savebmp(out2_filename, image_IPM, fileheader2, infoheader2);

	/***********************************  bonus  *****************************************/
	//read
	BITMAPFILEHEADER fileheader3;
	BITMAPINFOHEADER infoheader3;
	char filename_left[50] = "left.bmp";
	char filename_right[50] = "right.bmp";
	char bonus_filename[50] = "out_bonus.bmp";

	FILE* fp3;
	fp3 = fopen(filename_left, "rb");
	if (!fp3) {
		cout << "file open fail!" << endl;
		return 0;
	}
	fread(&fileheader3, sizeof(BITMAPFILEHEADER), 1, fp3);
	fread(&infoheader3, sizeof(BITMAPINFOHEADER), 1, fp3);
	ImageX = infoheader3.biWidth;
	ImageY = infoheader3.biHeight;

	size = ImageX * ImageY;
	Mat left = imread("left.bmp");
	fclose(fp3);

	FILE* fp4;
	fp4 = fopen(filename_right, "rb");
	if (!fp4) {
		cout << "file open fail!" << endl;
		return 0;
	}
	fread(&fileheader3, sizeof(BITMAPFILEHEADER), 1, fp4);
	fread(&infoheader3, sizeof(BITMAPINFOHEADER), 1, fp4);

	Mat right = imread("right.bmp");
	fclose(fp4);

	int width_out = 900;
	int  height_out = 480;
	int size_out = width_out * height_out;
	RGBTRIPLE* output = new RGBTRIPLE[size_out]();
	
	Mat outmat = Mat(height_out, width_out, CV_8UC3, output);

	for (int x = 0; x < ImageX; x++) {
		for (int y = 0; y < ImageY; y++) {
			output[x + (y + 80) * width_out].rgbtBlue = left.at<Vec3b>(y, x)[0];
			output[x + (y + 80) * width_out].rgbtGreen = left.at<Vec3b>(y, x)[1];
			output[x + (y + 80) * width_out].rgbtRed = left.at<Vec3b>(y, x)[2];
		}
	}
	int x0 = 130, y0 = 250, x1 = 470, y1 = 310, x2 = 475, y2 = 900, x3 = 0, y3 = 770;
	int dx1 = x1 - x2, dx2 = x3 - x2, all_x = x0 - x1 + x2 - x3;
	int dy1 = y1 - y2, dy2 = y3 - y2, all_y = y0 - y1 + y2 - y3;
	double g = (double)(((all_x * dy2 * 1.0) - (dx2 * all_y * 1.0)) / ((dx1 * dy2 * 1.0) - (dx2 * dy1 * 1.0)));
	double h = (double)(((dx1 * all_y * 1.0) - (all_x * dy1 * 1.0)) / ((dx1 * dy2 * 1.0) - (dx2 * dy1 * 1.0)));
	double a = (double)(x1 - x0 + g * x1 * 1.0);
	double b = (double)(x3 - x0 + h * x3 * 1.0);
	double c = (double)x0;
	double d = (double)(y1 - y0 + g * y1 * 1.0);
	double e = (double)(y3 - y0 + h * y3 * 1.0);
	double f = (double)y0;
	double i = (double)1.0;
	double matrix_A = e * i - f * h;
	double matrix_B = c * h - b * i;
	double matrix_C = b * f - c * e;
	double matrix_D = f * g - d * i;
	double matrix_E = a * i - c * g;
	double matrix_F = c * d - a * f;
	double matrix_G = d * h - e * g;
	double matrix_H = b * g - a * h;
	double matrix_I = a * e - b * d;
	for (int x = 0; x < height_out; x++)
	{
		for (int y = 0; y < width_out; y++)
		{
			u = (matrix_A * x + matrix_B * y + matrix_C) * 370 / (matrix_G * x + matrix_H * y + matrix_I);
			v = (matrix_D * x + matrix_E * y + matrix_F) * 510 / (matrix_G * x + matrix_H * y + matrix_I);
			if (u >= 0 && u < right.rows && v >= 0 && v < right.cols)
			{
				output[x * 900 + y].rgbtBlue = right.at<Vec3b>(u, v)[0];
				output[x * 900 + y].rgbtGreen = right.at<Vec3b>(u, v)[1];
				output[x * 900 + y].rgbtRed = right.at<Vec3b>(u, v)[2];
			}
		}
	}

	imwrite("out_bonus.bmp", outmat);
	return 1;
}
void savebmp(char filename[], RGBTRIPLE* output, BITMAPFILEHEADER FileHeader, BITMAPINFOHEADER InfoHeader) {
	FILE* fpw;
	RGBTRIPLE rgb;
	fpw = fopen(filename, "wb");
	if (!fpw) {
		cout << "file save fail!" << endl;
	}
	else
		cout << "save success~~" << endl;
	fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fpw);
	fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fpw);

	for (int i = 0; i < InfoHeader.biHeight; i++)
	{
		for (int j = 0; j < InfoHeader.biWidth; j++)
		{
			rgb = output[i * InfoHeader.biWidth + j];
			fwrite(&rgb, sizeof(RGBTRIPLE), 1, fpw);
		}
	}
	fclose(fpw);

}