#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <vector>
#include <stack>
#include <opencv2\opencv.hpp>
typedef struct _Feather
{
    int label;              // �s�q�쪺label��
    int area;               // �s�q�쪺���n
    double centroidX,centroidY;        // ����
    int num = 0;
} Feather;

using namespace std;
using namespace cv;
int bwLabel(Mat& ans, Mat& src, Mat& dst, vector<Feather>& featherList,bool red);
void fingerblock(Mat& img_Bi, Mat& finger);
void listlist(Mat& hand, Mat& finger, vector<Feather>& handlist, vector<Feather>& fingerlist);
void text(Mat& img, Mat& hand, Mat& finger, vector<Feather>& handlist, vector<Feather>& fingerlist);

int main() {
    double time[5];
    LARGE_INTEGER m_nFreq;
    LARGE_INTEGER m_nBeginTime;
    LARGE_INTEGER nEndTime;
	char filename[50] = "hand.bmp";
	cout << "Enter the input filename : ";
	cin >> filename;
    /*
    QueryPerformanceFrequency(&m_nFreq); // �������P��
    QueryPerformanceCounter(&m_nBeginTime); // �p�ɶ}�l
    QueryPerformanceCounter(&nEndTime);// �p�ɺI��
    time[x] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//
    */
    QueryPerformanceFrequency(&m_nFreq); // �������P��
    QueryPerformanceCounter(&m_nBeginTime); // �p�ɶ}�l
	Mat img = imread(filename, 1);
	Mat img_Gray, img_Bi, img_box_hand, img_box_finger;
    Mat finger;
    vector<Feather> handlist, fingerlist;
	if (!img.data) {
		cout << "�䤣���ɮ�!!" << endl;
        return -1;
	}
    QueryPerformanceCounter(&nEndTime);// �p�ɺI��
    time[0] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//  Ū��

    QueryPerformanceFrequency(&m_nFreq); // �������P��
    QueryPerformanceCounter(&m_nBeginTime); // �p�ɶ}�l
    cvtColor(img, img_Gray, COLOR_BGR2GRAY);
    threshold(img_Gray, img_Bi, 225, 255, THRESH_BINARY_INV/*|THRESH_OTSU*/);
    QueryPerformanceCounter(&nEndTime);// �p�ɺI��
    time[1] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//�G�Ȥ�

    QueryPerformanceFrequency(&m_nFreq); // �������P��
    QueryPerformanceCounter(&m_nBeginTime); // �p�ɶ}�l
    fingerblock(img_Bi, finger);
    int handnum = bwLabel(img, img_Bi, img_box_hand, handlist, true);
    imshow("label_hand", img);
    imwrite("label_hand.png", img);
    int fingernum = bwLabel(img, finger, img_box_finger, fingerlist, false);
    QueryPerformanceCounter(&nEndTime);// �p�ɺI��
    time[2] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//�κA��


    QueryPerformanceFrequency(&m_nFreq); // �������P��
    QueryPerformanceCounter(&m_nBeginTime); // �p�ɶ}�l
    listlist(img_box_hand, img_box_finger, handlist, fingerlist);
    cout << "��x�ƶq�G " << handnum << endl;
    cout << "�и�" << "\t" << "�����" << "\t" << "���n" << "\t" << "����" << endl;
    for (vector<Feather>::iterator it = handlist.begin(); it < handlist.end(); it++)
    {
        cout << it->label << "\t" << it->num << "\t"<< it->area << "\t( " << it->centroidX << ", " << it->centroidY << " )" << endl;
    }
    cout << "����ƶq�G " << fingernum << endl;
    cout << "�и�" << "\t" << "��x�и�" << "\t" << "���n" << "\t" << "����" << endl;
    for (vector<Feather>::iterator it = fingerlist.begin(); it < fingerlist.end(); it++)
    {
        cout << it->label << "\t" << it->num << "\t\t" << it->area << "\t( " << it->centroidX << ", " << it->centroidY << " )" << endl;
    }
    text(img, img_box_hand, img_box_finger, handlist, fingerlist);
    QueryPerformanceCounter(&nEndTime);// �p�ɺI��
    time[3] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//�e��+���

    QueryPerformanceFrequency(&m_nFreq); // �������P��
    QueryPerformanceCounter(&m_nBeginTime); // �p�ɶ}�l
    imshow("image_Bi", img_Bi);
    imwrite("Binary.png", img_Bi);
    imshow("image_finger", finger);
    imwrite("finger.png", finger);
    imshow("result", img);
    imwrite("result.png", img);
    QueryPerformanceCounter(&nEndTime);// �p�ɺI��
    time[4] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;// �s��
    cout << endl<< "�}�ɯӮ� : " << time[0] << " ms �G�ȤƯӮ� : " << time[1] << " ms" << endl << "�����x�M����Ӯ� : " << time[2] << " ms �e��+��ܯӮ� : " << time[3] << " ms �s�ɯӮ� : " << time[4] << " ms" << endl;
    waitKey();

    return 0;
}
void text(Mat& img, Mat& hand, Mat& finger, vector<Feather>& handlist, vector<Feather>& fingerlist) {
    int x, y;
    //putText(img, format("(%d,%d)", x, y), Point(x - 20, y), FONT_HERSHEY_SIMPLEX, 0.2, Scalar(0, 255, 0), 1, 8, false);
    for (vector<Feather>::iterator i = handlist.begin(); i < handlist.end(); i++) {
        x = i->centroidX;
        y = i->centroidY;
        putText(img, format("%d", i->label), Point(x - 10, y), FONT_HERSHEY_SIMPLEX, 0.8, Scalar(0, 0, 0), 3, 8, false);
        putText(img, format("( %d, %d )", x, y), Point(x - 10, y + 20), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(0, 255, 255), 1, 8, false);
    }
    for (vector<Feather>::iterator i = fingerlist.begin(); i < fingerlist.end(); i++) {
        x = i->centroidX;
        y = i->centroidY;
        putText(img, format("%d", i->label), Point(x - 10, y), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 0, 0), 2, 8, false);
        putText(img, format("( %d, %d )", x, y), Point(x - 10, y + 15), FONT_HERSHEY_SIMPLEX, 0.3, Scalar(255, 0, 255), 1, 8, false);
    }
}
void listlist(Mat& hand, Mat& finger, vector<Feather>& handlist, vector<Feather>& fingerlist) {
    int row = hand.rows;
    int col = hand.cols;
    int x, y;
    Feather temphandlist,tempfingerlist;
    for (vector<Feather>::iterator i = fingerlist.begin(); i < fingerlist.end(); i++)
    {
        x = i->centroidX;
        y = i->centroidY;
        Point c = Point(x, y);
        for (vector<Feather>::iterator j = handlist.begin(); j < handlist.end(); j++) {
            if (hand.at<uchar>(c)==j->label) {
                j->num++;
            }
        }
        i->num = hand.at<uchar>(c);
    }
}
void fingerblock(Mat& img_Bi, Mat& finger) {
    Mat temp;
    Mat element = getStructuringElement(MORPH_RECT, Size(3, 3));
    dilate(img_Bi, temp, element);
    for (int i = 0; i < 25; i++) {
        erode(temp, temp, element);
    }
    for (int i = 0; i < 25; i++) {
        dilate(temp, temp, element);
    }
    subtract(img_Bi, temp, finger);
    erode(finger, finger, element);
    erode(finger, finger, element);
    erode(finger, finger, element);
    dilate(finger, finger, element);
    dilate(finger, finger, element);
    dilate(finger, finger, element);

}
int bwLabel(Mat& ans, Mat& src, Mat& dst, vector<Feather>& featherList,bool red)
{
    int rows = src.rows;
    int cols = src.cols;

    int labelValue = 0;
    Point seed, neighbor;
    stack<Point> pointStack;    // ���|

    int area = 0;               // �Ω�p��s�q�쪺���n
    int x = 0, y = 0;
    int leftBoundary = 0;       // �s�q�쪺����ɡA�Y�~���̤p�x�Ϊ�����ءA��y�ЭȡA�̦�����
    int rightBoundary = 0;
    int topBoundary = 0;
    int bottomBoundary = 0;
    Rect box;                   // �~���x�ή�
    Feather feather;

    featherList.clear();    // �M���}�C

    dst.release();
    dst = src.clone();
    for (int i = 0; i < rows; i++)
    {
        uchar* pRow = dst.ptr<uchar>(i);
        for (int j = 0; j < cols; j++)
        {
            if (pRow[j] == 255)
            {
                area = 0;
                x = 0;
                y = 0;
                labelValue++;           // labelValue�̤j��254�A�̤p��1.
                seed = Point(j, i);     // Point�]��y�СA�a�y�С^
                dst.at<uchar>(seed) = labelValue;
                pointStack.push(seed);

                area++;
                x = x + j;
                y = y + i;
                leftBoundary = seed.x;
                rightBoundary = seed.x;
                topBoundary = seed.y;
                bottomBoundary = seed.y;

                while (!pointStack.empty())
                {
                    neighbor = Point(seed.x + 1, seed.y);
                    if ((seed.x != (cols - 1)) && (dst.at<uchar>(neighbor) == 255))
                    {
                        dst.at<uchar>(neighbor) = labelValue;
                        pointStack.push(neighbor);

                        area++;
                        x = x + seed.x + 1;
                        y = y + seed.y;
                        if (rightBoundary < neighbor.x)
                            rightBoundary = neighbor.x;
                    }

                    neighbor = Point(seed.x, seed.y + 1);
                    if ((seed.y != (rows - 1)) && (dst.at<uchar>(neighbor) == 255))
                    {
                        dst.at<uchar>(neighbor) = labelValue;
                        pointStack.push(neighbor);

                        area++;
                        x = x + seed.x;
                        y = y + seed.y + 1;
                        if (bottomBoundary < neighbor.y)
                            bottomBoundary = neighbor.y;

                    }

                    neighbor = Point(seed.x - 1, seed.y);
                    if ((seed.x != 0) && (dst.at<uchar>(neighbor) == 255))
                    {
                        dst.at<uchar>(neighbor) = labelValue;
                        pointStack.push(neighbor);

                        area++;
                        x = x + seed.x - 1;
                        y = y + seed.y;
                        if (leftBoundary > neighbor.x)
                            leftBoundary = neighbor.x;
                    }

                    neighbor = Point(seed.x, seed.y - 1);
                    if ((seed.y != 0) && (dst.at<uchar>(neighbor) == 255))
                    {
                        dst.at<uchar>(neighbor) = labelValue;
                        pointStack.push(neighbor);

                        area++;
                        x = x + seed.x;
                        y = y + seed.y - 1;
                        if (topBoundary > neighbor.y)
                            topBoundary = neighbor.y;
                    }

                    seed = pointStack.top();
                    pointStack.pop();
                }
                box = Rect(leftBoundary, topBoundary, rightBoundary - leftBoundary+1, bottomBoundary - topBoundary+1);

                if(red)
                    rectangle(ans, box, Scalar(0, 0, 255));
                else if (!red)
                    rectangle(ans, box, Scalar(255, 0, 0));
                feather.centroidX = x / area;
                feather.centroidY = y / area;
                feather.area = area;
                feather.label = labelValue;
                featherList.push_back(feather);
            }
        }
    }
    return labelValue;
}
