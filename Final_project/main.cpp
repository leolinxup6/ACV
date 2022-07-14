#include "Header.h"

int num = 1;
int good_total = 0;
int bad_total = 0;
int main() {
	LARGE_INTEGER m_nFreq;
	LARGE_INTEGER m_nBeginTime;
	LARGE_INTEGER nEndTime;
	string type;
	type = "2";
	Mat after, before, correct, gt;
	gt = generate_gt();
	//imwrite("gt.jpg", gt);
	if (type == "1") {
		read_img(after, before, correct, num);

		QueryPerformanceFrequency(&m_nFreq);
		QueryPerformanceCounter(&m_nBeginTime);

		Mat detect = process(after, before);
		double iou = IOU_cul(detect, correct);

		QueryPerformanceCounter(&nEndTime);

		cout << "image" << num << " iou : " << iou << endl;
		bool GorD = classify(gt, detect);
		if (GorD) {
			cout << "image" << num << " is good ~" << endl;
			good_total++;
		}
		else {
			cout << "image" << num << " is bad !" << endl;
			bad_total++;
		}
		cout << "calculate time : " << (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) / m_nFreq.QuadPart << endl << endl;

		num++;
		if (num < 8)
			return main();
		else {
			cout << "good total : " << good_total << endl;
			cout << "bad total : " << bad_total << endl;
			return 0;
		}
	}
	else if (type == "2") {
		read_img(after, before, num);

		QueryPerformanceFrequency(&m_nFreq);
		QueryPerformanceCounter(&m_nBeginTime);

		Mat detect = process(after, before);
		bool GorD = classify(gt, detect);
		double iou = IOU_cul(detect, gt);

		QueryPerformanceCounter(&nEndTime);

		cout << "image" << num << " iou : " << iou << endl;

		if (GorD) {
			cout << "image" << num << " is good ~" << endl;
			good_total++;
		}
		else {
			cout << "image" << num << " is bad !" << endl;
			bad_total++;
		}

		cout << "calculate time : " << (double)(nEndTime.QuadPart - m_nBeginTime.QuadPart) / m_nFreq.QuadPart << endl << endl;

		num++;
		if (num < 27)
			return main();
		else {
			cout << "good total : " << good_total << endl;
			cout << "bad total : " << bad_total << endl;
			return 0;
		}
	}

	return 0;
}