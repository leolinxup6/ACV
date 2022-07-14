#pragma once
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <stack>
#include <opencv2\opencv.hpp>
using namespace cv;
using namespace std;

const string position = "D:/Users/Desktop/ACV-final/";
const string position_good = "sample_good/";
const string position_bad = "sample_bad/";
const string position_gt = "sample_good_Groundtruth/";
const string name_after = "_after.jpg";
const string name_before = "_before.jpg";
const string name_gt = "_correct.jpg";


double IOU_cul(Mat src1, Mat src2);
Mat eq_rgb(Mat src);
Mat hsv_v(Mat src);
Mat without_noise(Mat src);
Mat sort(int num, Mat labels, Mat stats);
int read_img(Mat& after, Mat& before, Mat& correct,int num);
int read_img(Mat& after, Mat& before,int num);
Mat generate_gt();
Mat process(Mat after, Mat before);
bool classify(Mat gt, Mat detect);

