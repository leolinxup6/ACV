#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <opencv2\opencv.hpp> //´ú¸Õ¥Î
using namespace std;
int main()
{
    BITMAPFILEHEADER FileHeader;
    BITMAPINFOHEADER InfoHeader;
    char filename[50];
    FILE* fp;
    FILE* fpw;
    int size, ImageX, ImageY, choose;

    //input
    cout << "please enter the filename to load (.bmp) : ";
    cin >> filename;
    fp = fopen(filename, "rb");
    if (!fp) {
        cout << "file open fail!" << endl;
        return 0;
    }

    fread(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fp);
    fread(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fp);

    ImageX = InfoHeader.biWidth;
    ImageY = InfoHeader.biHeight;

    size = ImageX * ImageY;
    RGBTRIPLE rgb;
    RGBTRIPLE* color = new RGBTRIPLE[size];
    RGBTRIPLE* rotate = new RGBTRIPLE[size];
    RGBTRIPLE* inter = new RGBTRIPLE[size];

    for (int i = 0; i < ImageY; i++)
    {
        for (int j = 0; j < ImageX; j++)
        {
            fread(&rgb, sizeof(RGBTRIPLE), 1, fp);
            color[i * ImageX + j] = rgb;
        }
    }
    fclose(fp);

    //process
    cout << "choose which process want to do " << endl << "1.NO Process" << endl << "2.Image Rotate" << endl << "3.Channel Intercharge" << endl << "4.Exit" << endl;
    cin >> choose;
    switch (choose) {
    case 1:
        for (int i = 0; i < ImageY; i++) {
            for (int j = 0; j< ImageX; j++) {
                rotate[i * ImageX + j] = color[i * ImageX + j];
            }
        }

        break;

    case 2:
        for (int i = 0; i < ImageY; i++) {
            for (int j = 0; j < ImageX; j++) {
                rotate[size - (i * ImageX + j)-1] = color[i * ImageX + j];
            }
        }
        break;

    case 3:
        for (int i = 0; i < ImageY; i++) {
            for (int j = 0; j < ImageX; j++) {
                inter[size - (i * ImageX + j) - 1] = color[i * ImageX + j];
            }
        }
        for (int i = 0; i < ImageY; i++) {
            for (int j = 0; j < ImageX; j++) {
                rotate[i * ImageX + j].rgbtBlue = inter[i * ImageX + j].rgbtGreen;
                rotate[i * ImageX + j].rgbtGreen = inter[i * ImageX + j].rgbtRed;
                rotate[i * ImageX + j].rgbtRed = inter[i * ImageX + j].rgbtBlue;
            }
        }
        
        break;
    case 4:
        return 0;
    default:

        break;

    }

    //output

    cv::Mat r = cv::Mat(ImageX, ImageY, CV_8UC3, rotate);

    cout << "please enter the filename to save (.bmp) : ";
    cin >> filename;
    fpw = fopen(filename, "wb");
    if (!fpw) {
        cout << "file save fail!" << endl;
    }
    fwrite(&FileHeader, sizeof(BITMAPFILEHEADER), 1, fpw);
    fwrite(&InfoHeader, sizeof(BITMAPINFOHEADER), 1, fpw);

    for (int i = 0; i < ImageY; i++)
    {
        for (int j = 0; j < ImageX; j++)
        {
            rgb = rotate[i * ImageX + j];
            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fpw);
        }
    }
    fclose(fpw);

    return main();
}