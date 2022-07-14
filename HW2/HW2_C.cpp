#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <stack>
#include <opencv2\opencv.hpp> //測試用

using namespace std;
using namespace cv;
int area(unsigned char* newhand, int h, int w, int label);
double centroidX(unsigned char* newhand, int h, int w, int label, int area);
double centroidY(unsigned char* newhand, int h, int w, int label, int area);
int detect(unsigned char* hand, unsigned char* newhand, int w, int h);
void block(RGBTRIPLE* hand, unsigned char* newhand, int h, int w, int label);
void erosion(unsigned char* padding,  unsigned char* finger,int h, int w);
void blockblue(RGBTRIPLE* hand, unsigned char* newhand, int h, int w, int label);
void longest(unsigned char* finger, int h, int w, int label);
void savebmp(char filename[], RGBTRIPLE* hand, BITMAPFILEHEADER FileHeader, BITMAPINFOHEADER InfoHeader, int ImageX, int ImageY);


int main() {
    double time[4];
    LARGE_INTEGER m_nFreq;
    LARGE_INTEGER m_nBeginTime;
    LARGE_INTEGER nEndTime;
	BITMAPFILEHEADER FileHeader;
	BITMAPINFOHEADER InfoHeader;
    char filename[50];
	char handfilename[50] ="hand.bmp";
	char test1filename[20] = "test1.bmp";
	char test2filename[20] = "test2.bmp";
    FILE* fp;
    int size = 0,ImageX,ImageY;
    cout << "please enter the filename to load (.bmp) : ";
    cin >> handfilename;
    fp = fopen(handfilename, "rb");
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
    RGBTRIPLE* hand = new RGBTRIPLE[size];
    RGBTRIPLE* rotate = new RGBTRIPLE[size];

    for (int i = 0; i < ImageY; i++)
    {
        for (int j = 0; j < ImageX; j++)
        {
            fread(&rgb, sizeof(RGBTRIPLE), 1, fp);
            hand[i * ImageX + j] = rgb;
        }
    }
    fclose(fp);

    unsigned char* bihand = new unsigned char[size];
    unsigned char* newhand = new unsigned char[size];
    for (int i = 0; i < size; i++) {
        if (((hand[i].rgbtBlue + hand[i].rgbtGreen + hand[i].rgbtRed) / 3) < 230) {
            bihand[i] = 255;
            hand[i].rgbtBlue = 255;
            hand[i].rgbtGreen = 255;
            hand[i].rgbtRed = 255;
        }
        else {
            bihand[i] = 0;
            hand[i].rgbtBlue = 0;
            hand[i].rgbtGreen = 0;
            hand[i].rgbtRed = 0;
        }
    }

    Mat bihandmat = Mat(ImageX, ImageY, CV_8UC1, bihand);//CV
    Mat newhandmat = Mat(ImageX, ImageY, CV_8UC1, newhand);//CV
    Mat handmat = Mat(ImageX, ImageY, CV_8UC3, hand);//CV   

    cout << "please enter the filename to save binary hand (.bmp) : ";
    cin >> filename;
    imwrite(filename, bihandmat);
    //savebmp(filename, bihand, FileHeader, InfoHeader, ImageX, ImageY);
    QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
    QueryPerformanceCounter(&m_nBeginTime); // 獲取時鍾計數
    int labelvalue = detect(bihand, newhand, ImageX, ImageY);
    QueryPerformanceCounter(&nEndTime);
    time[0] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//偵測手掌
    int* labelarea = new int[labelvalue];
    double* labelcentroidX = new double[labelvalue];
    double* labelcentroidY = new double[labelvalue];
    QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
    QueryPerformanceCounter(&m_nBeginTime); // 獲取時鍾計數
    block(hand, newhand, ImageY, ImageX, labelvalue);
    for (int i = 2; i < labelvalue + 1; i++) {
        labelarea[i - 1] = area(newhand, ImageY, ImageX, i);
        labelcentroidX[i - 1] = centroidX(newhand, ImageY, ImageX, i, labelarea[i - 1]);
        labelcentroidY[i - 1] = centroidY(newhand, ImageY, ImageX, i, labelarea[i - 1]);
        //cout << "label hand" << i-1 << " : area = " << labelarea[i - 1] << " centroid = ( " << labelcentroidX[i - 1] << ", " << labelcentroidY[i - 1] << ")" << endl;
    }
    QueryPerformanceCounter(&nEndTime);
    time[1] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//劃出手掌

    cout << "please enter the filename to save label hand (.bmp) : ";
    cin >> filename;
    savebmp(filename, hand, FileHeader, InfoHeader, ImageX, ImageY);

    unsigned char* padding = new unsigned char[(ImageX+2)*(ImageY+2)];
    unsigned char* finger = new unsigned char[(ImageX) * (ImageY)];
    memset(finger, 0, sizeof(uchar) * (ImageX) * (ImageY));
    unsigned char* newfinger = new unsigned char[(ImageX) * (ImageY)];
    memset(padding, 0, (ImageX+2) * (ImageY+2));
    for (int i = 1; i < ImageX + 1; i++) {
        for (int j = 1; j < ImageY + 1; j++) {
            padding[i + j * (ImageX + 2)] = bihand[(i - 1) + (j - 1) * ImageX];
            //padding[i + j * (ImageX + 2)] = newhand[i - 1 + (j - 1) * ImageX];
        }
    }
    Mat paddingmat = Mat(ImageX+2, ImageY+2, CV_8UC1, padding);
    Mat fingermat = Mat(ImageX, ImageY, CV_8UC1, finger);
    QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
    QueryPerformanceCounter(&m_nBeginTime); // 獲取時鍾計數
    erosion(padding, finger, ImageY, ImageX);
    int labelvalue2 = detect(finger, newfinger, ImageY, ImageX);
    labelvalue2--;
    QueryPerformanceCounter(&nEndTime);
    time[2] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//偵測手指
    int* labelareaA = new int[labelvalue2];
    double* labelcentroidXX = new double[labelvalue2];
    double* labelcentroidYY = new double[labelvalue2];
    QueryPerformanceFrequency(&m_nFreq); // 獲取時鍾周期
    QueryPerformanceCounter(&m_nBeginTime); // 獲取時鍾計數
    blockblue(hand, newfinger, ImageY, ImageX, labelvalue2);
    for (int i = 1; i < labelvalue2 + 1; i++) {
        labelareaA[i - 1] = area(newfinger, ImageY, ImageX, i+1);
        labelcentroidXX[i - 1] = centroidX(newfinger, ImageY, ImageX, i+1, labelareaA[i - 1]);
        labelcentroidYY[i - 1] = centroidY(newfinger, ImageY, ImageX, i+1, labelareaA[i - 1]);
        //cout << "label finger" << i << " : area = " << labelareaA[i - 1] << " centroid = ( " << labelcentroidXX[i - 1] << ", " << labelcentroidYY[i - 1] << ")" << endl;
    }
    QueryPerformanceCounter(&nEndTime);
    time[3] = (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) * 1000 / m_nFreq.QuadPart;//畫出手指
     //reserve the finger
    cout << "please enter the filename to save finger (.bmp) : ";
    cin >> filename;
    imwrite(filename, fingermat);
    //savebmp(filename, newfinger, FileHeader, InfoHeader, ImageX, ImageY);

    cout << "please enter the filename to save label finger (.bmp) : ";
    cin >> filename;
    savebmp(filename, hand, FileHeader, InfoHeader, ImageX, ImageY);

    int* num = new int[labelvalue];
    memset(num, 0, sizeof(int) * labelvalue);
    for (int i = 0; i < labelvalue2; i++) {
        int x = labelcentroidXX[i];
        int y = labelcentroidYY[i];
        num[newhand[x + y * ImageX] - 1]++;
    }

    cout << "please enter the filename to save (.bmp) : ";
    cin >> filename;
    savebmp(filename, hand, FileHeader, InfoHeader, ImageX, ImageY);
    /*
    for (int i = 1; i < labelvalue; i++)
        cout << num[i] << endl;*/
    num[1]--;
    for (int i = 1; i < labelvalue; i++) {
        for (int j = 1; j < labelvalue+1; j++) {
            if (num[i] == j) {
                cout << "第 " << i << " 個手掌有 " << num[i] << " 根手指" << endl;
                
                cout<<"手掌面積 : "<<labelarea[j]<<"手掌質心 : (" << labelcentroidX[i] << ", " << labelcentroidY[i] << ")" << endl;
                for (int z = 0; z < labelvalue2; z++) {
                    int x = labelcentroidXX[z];
                    int y = labelcentroidYY[z];
                    
                    if ((newhand[x + y * ImageX]) == i+1&&labelareaA[z]>400) {
                        cout<<"   手指面積 : "<<labelareaA[z]<< "  手指質心 : (" << labelcentroidXX[z] << ", " << labelcentroidYY[z] << ")" << endl;
                        longest(newfinger, ImageY, ImageX, z+2);
                    }
                }
            }
        }

    }
    cout << "偵測手掌耗時 : " << time[0] << " ms 劃出手掌耗時 : " << time[1] << " ms 偵測手指耗時 : " << time[2] << " ms 劃出手指耗時 : " << time[3] << " ms" << endl;

    Mat newfingermat = Mat(ImageX, ImageY, CV_8UC1, newfinger);
    return 0;
}
void savebmp(char filename[],RGBTRIPLE* hand,BITMAPFILEHEADER FileHeader,BITMAPINFOHEADER InfoHeader,int ImageX,int ImageY) {
    FILE* fpw;
    RGBTRIPLE rgb;
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
            rgb = hand[i * ImageX + j];
            fwrite(&rgb, sizeof(RGBTRIPLE), 1, fpw);
        }
    }
    fclose(fpw);

}
void longest(unsigned char* finger, int h, int w, int label) {
    int RightX = 0, UpY = 0, LeftX = 512, downY = 512, RightY = 0, UpX = 0, LeftY = 512, downX = 512;
    int dist = 0;
    for (int a = 1; a < label + 1; a++) {
        RightX = 0;
        UpY = 0;
        LeftX = 512;
        downY = 512;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (finger[i * w + j] == a) {
                    if (j > RightX) {
                        RightX = j;
                        RightY = i;
                    }
                    else if (j < LeftX) {
                        LeftX = j;
                        LeftY = i;
                    }
                    if (i > UpY) {
                        UpY = i;
                        UpX = j;
                    }
                    else if (i < downY) {
                        downY = i;
                        downX = j;
                    }
                }
            }
        }
    }
    int x = 0, y = 0;
    x = sqrt(pow((RightX - LeftX), 2) + pow((RightY - LeftY), 2));
    y = sqrt(pow((UpX - downX), 2) + pow((UpY - downY), 2));
    if (x > y) {
        cout << "        longest distanse : " << x << " from ( " << RightX << ", " << RightY << " ) to ( " << LeftX << ", " << LeftY << " )" << endl;
    }
    else if (x < y) {
        cout << "        longest distanse : " << x << " from ( " << UpX << ", " << UpY << " ) to ( " << downX << ", " << downY << " )" << endl;
    }
}
void erosion(unsigned char* padding, unsigned char*finger,int h,int w) {

    unsigned char* pad1 = new unsigned char[(h + 2) * (w + 2)];
    unsigned char* pad2 = new unsigned char[(h + 2) * (w + 2)];
    memcpy(pad1, padding, sizeof(uchar) * (h + 2) * (w + 2));
    memcpy(pad2, padding, sizeof(uchar) * (h + 2) * (w + 2));
    Mat pad1mat = Mat(h + 2, w + 2, CV_8UC1, pad1);
    Mat pad2mat = Mat(h + 2, w + 2, CV_8UC1, pad2);
    Mat paddingmat = Mat(h + 2, w + 2, CV_8UC1, padding);
    int k, l;
    int a = 0;
    int step[9][2] = { {-1,-1},{0,-1},{1,-1},{-1,0},{0,0} , {1, 0}, { -1,1 }, { 0,1 }, { 1,1 } };
    //12 29
    for (int x = 0; x < 20; x++) {
        for (int i = 1; i < w + 1; i++) {
            for (int j = 1; j < h + 1; j++) {
                a = 0;
                for (int z = 0; z < 9; z++) { //for (int z = 0; z < 9; z++) {
                    k = i + step[z][0];
                    l = j + step[z][1];
                    if (pad1[k + l * (w + 2)] == 0)
                        a = 1;
                }
                if (a == 1)
                    pad2[i + j * (w + 2)] = 0;
                else
                    pad2[i + j * (w + 2)] = 255;
            }
        }
        memcpy(pad1, pad2, sizeof(uchar) * (h + 2) * (w + 2));
    }
    for (int x = 0; x < 26; x++) {
        for (int i = 1; i < w + 1; i++) {
            for (int j = 1; j < h + 1; j++) {
                a = 0;
                for (int z = 0; z < 9; z++) {
                    k = i + step[z][0];
                    l = j + step[z][1];
                    if (pad1[k + l * (w + 2)] >0)
                        a = 1;
                }
                if (a == 1)
                    pad2[i + j * (w + 2)] = 255;
                else
                    pad2[i + j * (w + 2)] = 0;
            }
        }
        memcpy(pad1, pad2, sizeof(uchar) * (h + 2) * (w + 2));
    }
    for (int i = 1; i < w + 1; i++) {
        for (int j = 1; j < h + 1; j++) {
            finger[i-1 + (j-1) * (w)] = padding[i + j * (w + 2)] - pad1[i + j * (w + 2)];
        }
    }
    Mat finmat = Mat(h, w, CV_8UC1, finger);
    return;
}
double centroidY(unsigned char* newhand, int h, int w, int label, int area) {
    int sumY = 0;
    int sum = 0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if (newhand[i*w+j] == label) {
                sumY = sumY + i;
                sum = sum + 1;
            }
        }
    }
    return (double)sumY/sum;
}
double centroidX(unsigned char* newhand, int h, int w, int label, int area) {
    int sumX = 0,sum =0;
    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            if (newhand[i*w+j] == label) {
                sumX = sumX + j;
                sum++;
            }
        }
    }
    return (double)sumX / sum;
}
int area(unsigned char* newhand, int h, int w, int label) {
    int sum = 0;
    for (int i = 0; i < h * w; i++) {
        if (newhand[i] == label)
            sum++;
    }
    return sum;
}
int detect(unsigned char* hand, unsigned char* newhand, int h, int w) {
    int labelValue = 1;
    int seed, neighbor;
    stack<int> intStack;    // 堆疊
    int area = 0;
    if (!intStack.empty()) intStack.pop();    // 清空Stack
    memcpy(newhand, hand, sizeof(uchar) * h * w);
    Mat blurshrinklenaMat = Mat(w, h, CV_8UC1, newhand);
    for (int i = 0; i < h; i++)
    {
        for (int j = 0; j < w; j++)
        {
            if (newhand[i * w + j] == 255)
            {
                area = 0;
                labelValue++;
                int k = i;
                int l = j;
                seed = i * w + j;     // Point（橫座標，縱座標）
                newhand[i * w + j] = labelValue;
                intStack.push(i);
                intStack.push(j);
                while (!intStack.empty())
                {
                    neighbor = (i + 1) * w + j;//下
                    if ((((i + 1) < h) && j < w) && (newhand[neighbor] == 255))
                    {
                        newhand[neighbor] = labelValue;
                        intStack.push(i + 1);
                        intStack.push(j);
                        area++;
                    }
                    neighbor = (i - 1) * w + j;//上
                    if (((i != 0) && j < w) && (newhand[neighbor] == 255))
                    {
                        newhand[neighbor] = labelValue;
                        intStack.push(i - 1);
                        intStack.push(j);
                        area++;
                    }
                    neighbor = i * w + j + 1;//右
                    if (((i < h) && (j + 1 < w)) && (newhand[neighbor] == 255))
                    {
                        newhand[neighbor] = labelValue;
                        intStack.push(i);
                        intStack.push(j + 1);
                        area++;
                    }
                    neighbor = i * w + j - 1;//左
                    if ((((i < h) && (j != 0)) && newhand[neighbor] == 255))
                    {
                        newhand[neighbor] = labelValue;
                        intStack.push(i);
                        intStack.push(j - 1);
                        area++;
                    }
                    j = intStack.top();
                    intStack.pop();
                    i = intStack.top();
                    intStack.pop();
                }

                //cout << "label " << labelValue << " : area = " << area << endl;
            }

        }
    }
    return labelValue;
}
void blockblue(RGBTRIPLE* hand, unsigned char* newhand, int h, int w, int label) {
    int maxX = 0, maxY = 0, minX = 512, minY = 512;
    int sum = 0;

    for (int a = 2; a < label + 2; a++) {
        if (a == 9)
            continue;
        maxX = 0;
        maxY = 0;
        minX = 512;
        minY = 512;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (newhand[i * w + j] == a) {

                    if (j > maxX)
                        maxX = j;
                    else if (j < minX)
                        minX = j;
                    if (i > maxY)
                        maxY = i;
                    else if (i < minY)
                        minY = i;
                }
            }
        }
        for (int i = minX; i < maxX; i++) {
            hand[i + minY * w].rgbtBlue = 255;
            hand[i + minY * w].rgbtGreen = 0;
            hand[i + minY * w].rgbtRed = 0;
        }
        for (int i = minX; i < maxX + 1; i++) {
            hand[i + maxY * w].rgbtBlue = 255;
            hand[i + maxY * w].rgbtGreen = 0;
            hand[i + maxY * w].rgbtRed = 0;
        }
        for (int i = minY; i < maxY; i++) {
            hand[minX + i * w].rgbtBlue = 255;
            hand[minX + i * w].rgbtGreen = 0;
            hand[minX + i * w].rgbtRed = 0;
        }
        for (int i = minY; i < maxY; i++) {
            hand[maxX + i * w].rgbtBlue = 255;
            hand[maxX + i * w].rgbtGreen = 0;
            hand[maxX + i * w].rgbtRed = 0;
        }

    }
}
void block(RGBTRIPLE* hand, unsigned char* newhand, int h, int w, int label) {
    int maxX = 0, maxY = 0, minX = 512, minY = 512;
    for (int a = 1; a < label + 1; a++) {
        maxX = 0;
        maxY = 0;
        minX = 512;
        minY = 512;
        for (int i = 0; i < h; i++) {
            for (int j = 0; j < w; j++) {
                if (newhand[i * w + j] == a) {
                    if (j > maxX)
                        maxX = j;
                    else if (j < minX)
                        minX = j;
                    if (i > maxY)
                        maxY = i;
                    else if (i < minY)
                        minY = i;
                }
            }
        }
        for (int i = minX; i < maxX; i++) {
            hand[i + minY * w].rgbtBlue = 0;
            hand[i + minY * w].rgbtGreen = 0;
            hand[i + minY * w].rgbtRed = 255;
        }
        for (int i = minX; i < maxX + 1; i++) {
            hand[i + maxY * w].rgbtBlue = 0;
            hand[i + maxY * w].rgbtGreen = 0;
            hand[i + maxY * w].rgbtRed = 255;
        }
        for (int i = minY; i < maxY; i++) {
            hand[minX + i * w].rgbtBlue = 0;
            hand[minX + i * w].rgbtGreen = 0;
            hand[minX + i * w].rgbtRed = 255;
        }
        for (int i = minY; i < maxY; i++) {
            hand[maxX + i * w].rgbtBlue = 0;
            hand[maxX + i * w].rgbtGreen = 0;
            hand[maxX + i * w].rgbtRed = 255;
        }
        // cout << a << " " << minX << " " << minY << " " << maxX << " " << maxY << endl;
    }
}
